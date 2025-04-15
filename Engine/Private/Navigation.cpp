#include "Navigation.h"
#include "Cell.h"

#include "Shader.h"
#include "Collider.h"
#include "GameInstance.h"

#include <numeric>

#undef min

_float4x4 CNavigation::m_WorldMatrix = {};
const  _float CNavigation::s_fFriction = 3.5f;
//F = G + H
//G : 시작에서 현 노드까지
//H : 현 노드에서 도착점까지
struct AStarNode {
    bool operator<(const AStarNode& other) const {
        return F < other.F;
    }
    bool operator>(const AStarNode& other) const {
        return F > other.F;
    }

    _float F;
    _float G;
    _int pos;
};

struct LessNode {
    bool operator()(const AStarNode& lhs, const AStarNode& rhs) const {
        return lhs.F > rhs.F;
    }
};

bool PointInTriangle(XMVECTOR P0, XMVECTOR P1, XMVECTOR P2, XMVECTOR q)
{
    // 세 변을 정의
    XMVECTOR v0 = P2 - P0;
    XMVECTOR v1 = P1 - P0;
    XMVECTOR v2 = q - P0;

    // 배리센트릭 좌표 계산을 위한 벡터 도트 곱
    float dot00 = XMVectorGetX(XMVector3Dot(v0, v0));
    float dot01 = XMVectorGetX(XMVector3Dot(v0, v1));
    float dot02 = XMVectorGetX(XMVector3Dot(v0, v2));
    float dot11 = XMVectorGetX(XMVector3Dot(v1, v1));
    float dot12 = XMVectorGetX(XMVector3Dot(v1, v2));

    // 배리센트릭 좌표 (u, v) 계산
    float invDenom = 1.0f / (dot00 * dot11 - dot01 * dot01);
    float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

    // u >= 0, v >= 0, u + v <= 1 이면 삼각형 내부에 있음
    return (u >= 0) && (v >= 0) && (u + v <= 1);
}

CNavigation::CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CComponent{ pDevice, pContext }
{
}

CNavigation::CNavigation(const CNavigation& Prototype)
    : CComponent{ Prototype }
    , m_Cells{ Prototype.m_Cells }

#ifdef _DEBUG
    , m_pShader{ Prototype.m_pShader }
#endif
{
    for (auto& pCell : m_Cells)
        Safe_AddRef(pCell);

#ifdef _DEBUG
    Safe_AddRef(m_pShader);
#endif 
}

HRESULT CNavigation::Initialize_Prototype(const _wstring& strNavigationDataFile)
{
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

    _ulong dwByte = {};

    HANDLE hFile = CreateFile(strNavigationDataFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (0 == hFile)
        return E_FAIL;

    _uint cellSize = 0;
    _float3 vPoints[3];

    ReadFile(hFile, &cellSize, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < cellSize; i++)
    {
        ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
        if (0 == dwByte)
            break;
        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
        if (nullptr == pCell)
            return E_FAIL;
        m_Cells.emplace_back(pCell);
    }

    CloseHandle(hFile);

    if (FAILED(SetUp_Neighbors()))
        return E_FAIL;

    for (auto* pCell : m_Cells) {
        for (auto* pNCell : m_Cells) {
            if (pCell == pNCell)
                continue;
            pCell->Set_AboveCells(pNCell);
        }
    }

#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif

    return S_OK;
}

HRESULT CNavigation::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return S_OK;

    NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(pArg);

    m_iCurrentCellIndex = pDesc->iCurrentIndex;

    return S_OK;
}

void CNavigation::Update(_fmatrix TerrainWorldMatrix)
{
    XMStoreFloat4x4(&m_WorldMatrix, TerrainWorldMatrix);
}

_bool CNavigation::isMove(_fvector vPosition, _fvector vOriPosition, _float* fHeight, CCollider* pCollider, _float3* vLineOut)
{
    _int    iNeighborIndex = { -1 };
    _float3    vLine = {};

    _bool bIsIn = false;
    _float maxHeight = numeric_limits<_float>::min();

    if (m_iCurrentCellIndex == -1) {
        _bool bInCell = false;
        _int iIndex = 0;
        for (auto* pCell : m_Cells) {
            _bool b = PointInTriangle(pCell->Get_Point(CCell::POINT_A), pCell->Get_Point(CCell::POINT_B), pCell->Get_Point(CCell::POINT_C), vPosition);

            if (b) {
                _float fCellHeight = pCell->GetHeight(vPosition.m128_f32[0], vPosition.m128_f32[2]);
                if (fCellHeight <= vPosition.m128_f32[1] && maxHeight < fCellHeight) {
                    bInCell = true;
                    m_iCurrentCellIndex = iIndex;
                    maxHeight = fCellHeight;
                }
            }
            iIndex++;
        }

        return true;
    }

    // 삼각형 위치에
    for (_int i = 0; i < 3; i++) {
        _vector vSour, vDest;

        vSour = XMVector3Normalize(vPosition - m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT(i)));

        _vector vLine = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT((i + 1) % 3)) - m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT(i));
        vDest = XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f);

        if (0 < XMVectorGetX(XMVector3Dot(vSour, vDest)))
        {
            _int nei = m_Cells[m_iCurrentCellIndex]->Get_Neighbor((CCell::LINE)i);
            if (nei == -1) {
                continue;
            }
            _float fNextHeight = abs(m_Cells[nei]->GetHeight(XMVectorGetX(vPosition), XMVectorGetZ(vPosition)) - m_Cells[nei]->GetHeight(XMVectorGetX(vOriPosition), XMVectorGetZ(vOriPosition)));

            _float fXZLength = XMVectorGetX(XMVector3Length(vPosition - vOriPosition));

            /*if (XMVectorGetZ(vPosition) == XMVectorGetZ(vOriPosition) && XMVectorGetX(vPosition) == XMVectorGetX(vOriPosition)) {
                return false;
            }*/

            _vector vCellNormal = XMVector3Normalize(m_Cells[nei]->Get_Normal());

            //_float ratio_ytoxz = XMVectorGetY(vLine) / sqrtf(powf(XMVectorGetX(vLine), 2.f) + powf(XMVectorGetZ(vLine), 2.f));

            if (abs(XMVectorGetY(vCellNormal)) < abs(XMVectorGetX(vCellNormal)) || abs(XMVectorGetY(vCellNormal)) < abs(XMVectorGetZ(vCellNormal))) {
                XMStoreFloat3(vLineOut, vLine);
                return false;
            }
        }
    }

    // 원래 있던 삼각형 안에서 움직임
    if (true == m_Cells[m_iCurrentCellIndex]->isIn(vPosition, &iNeighborIndex, vLine, m_iCurrentCellIndex))
    {
        *fHeight = m_Cells[m_iCurrentCellIndex]->GetHeight(
            vPosition.m128_f32[0], vPosition.m128_f32[2]);
        return true;
    }
    // 원래 삼각형 벗어남
    else
    {
        //나간 쪽에 이웃 있음
        if (-1 != iNeighborIndex /*&& iNeighborIndex == m_reservedPath.front()*/)
        {
            if (m_reservedPath.empty() || iNeighborIndex != m_reservedPath.front()) {
                AStar();
            }

            /*for (int i = 0; i < 15; i++) {
                bIsIn = m_Cells[iNeighborIndex]->isIn(vPosition, &iNeighborIndex, vLine);
                if (bIsIn || iNeighborIndex == -1)
                    break;
            }*/

            _vector vComparePos = vPosition;
            while (!bIsIn && iNeighborIndex != -1) {
                vComparePos.m128_f32[1] = m_Cells[iNeighborIndex]->GetHeight(XMVectorGetX(vPosition), XMVectorGetZ(vPosition));
                for (_int i = 0; i < 3; i++) {
                    _vector vLine = m_Cells[iNeighborIndex]->Get_Point(CCell::POINT((i + 1) % 3)) - m_Cells[iNeighborIndex]->Get_Point(CCell::POINT(i));

                    _float fNextHeight = abs(m_Cells[iNeighborIndex]->GetHeight(XMVectorGetX(vPosition), XMVectorGetZ(vPosition)) - m_Cells[iNeighborIndex]->GetHeight(XMVectorGetX(vOriPosition), XMVectorGetZ(vOriPosition)));
                    _float fXZLength = XMVectorGetX(XMVector3Length(vPosition - vOriPosition));

                    /*if (XMVectorGetZ(vPosition) == XMVectorGetZ(vOriPosition) && XMVectorGetX(vPosition) == XMVectorGetX(vOriPosition)) {
                        return false;
                    }*/
                    _vector vCellNormal = XMVector3Normalize(m_Cells[iNeighborIndex]->Get_Normal());
                    //_float ratio_ytoxz = abs(XMVectorGetY(vLine)) / sqrtf(powf(XMVectorGetX(vLine), 2.f) + powf(XMVectorGetZ(vLine), 2.f));
                    if (abs(XMVectorGetY(vCellNormal)) < abs(XMVectorGetX(vCellNormal)) || abs(XMVectorGetY(vCellNormal)) < abs(XMVectorGetZ(vCellNormal))) {
                        XMStoreFloat3(vLineOut, vLine);
                        return false;
                    }
                }
                bIsIn = m_Cells[iNeighborIndex]->isIn(vComparePos, &iNeighborIndex, vLine, m_iCurrentCellIndex);

            }

            if (!bIsIn) {
                *vLineOut = vLine;
                return true;
            }

            if (pCollider != nullptr)
            {
                m_Cells[m_iCurrentCellIndex]->Erase_Collider(pCollider);
                m_Cells[iNeighborIndex]->Push_Collider(pCollider);
            }
            m_iCurrentCellIndex = iNeighborIndex;
            if (!m_reservedPath.empty())
            {
                m_reservedPath.pop_front();
            }
            *fHeight = m_Cells[m_iCurrentCellIndex]->GetHeight(
                vPosition.m128_f32[0], vPosition.m128_f32[2]);
            return true;
        }

        //나간 쪽에 이웃 없음
        else
        {
            _bool bInCell = false;
            _int iIndex = 0;
            for (auto* pCell : m_Cells) {
                _bool b = PointInTriangle(pCell->Get_Point(CCell::POINT_A), pCell->Get_Point(CCell::POINT_B), pCell->Get_Point(CCell::POINT_C), vPosition);

                if (b) {
                    _float fCellHeight = pCell->GetHeight(vPosition.m128_f32[0], vPosition.m128_f32[2]);
                    if (fCellHeight <= vPosition.m128_f32[1] && maxHeight < fCellHeight) {
                        bInCell = true;
                        m_iCurrentCellIndex = iIndex;
                        maxHeight = fCellHeight;
                    }
                }
                iIndex++;
            }
            *vLineOut = vLine;
            if (bInCell == false)
                m_iCurrentCellIndex = -1;
            return true;
        }
    }

}

_int CNavigation::SetCurrentIndex(_vector vertex, CCollider* pCollider)
{
    int i = 0;
    for (; i < m_Cells.size(); i++) {
        auto* pCell = m_Cells[i];
        if (PointInTriangle(pCell->Get_Point(CCell::POINT_A), pCell->Get_Point(CCell::POINT_B), pCell->Get_Point(CCell::POINT_C), vertex))
        {
            m_iCurrentCellIndex = i;
            m_Cells[i]->Push_Collider(pCollider);

            return i;
        }
    }
    i = -1;
    return -1;
}

_bool CNavigation::IsFloating(_fvector vPosition, _float* fHeight)
{
    if (m_iCurrentCellIndex == -1) {
        return true;
    }

    _float height = m_Cells[m_iCurrentCellIndex]->GetHeight(vPosition.m128_f32[0],
        vPosition.m128_f32[2]);
    if (fHeight != nullptr) {
        *fHeight = vPosition.m128_f32[1] - height;
    }

    return vPosition.m128_f32[1] > height;
}

_vector CNavigation::Get_CurrentCell_Normal() const
{
    if (m_iCurrentCellIndex == -1) {
        return { 0.f, 0.f, 0.f };
    }

    _vector v1 = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_B) - m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_A);
    _vector v2 = m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_C) - m_Cells[m_iCurrentCellIndex]->Get_Point(CCell::POINT_B);
    return XMVector3Cross(v1, v2);
}

_float CNavigation::Get_Height(_vector vPosition) const
{
    if (m_iCurrentCellIndex == -1) {
        return -1.f;
    }

    return m_Cells[m_iCurrentCellIndex]->GetHeight(vPosition.m128_f32[0], vPosition.m128_f32[2]);
}

#ifdef _DEBUG

HRESULT CNavigation::Render()
{
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float4 vColor = -1 == m_iCurrentCellIndex ? _float4(0.f, 1.f, 0.f, 1.f) : _float4(1.f, 0.f, 0.f, 1.f);
    _float4x4 WorldMatrix = m_WorldMatrix;

    if (-1 != m_iCurrentCellIndex)
        WorldMatrix._42 += 0.1f;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
        return E_FAIL;

    m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

    m_pShader->Begin(0);

    if (-1 == m_iCurrentCellIndex)
    {
        for (auto& pCell : m_Cells)
            pCell->Render();
    }
    else
    {
        m_Cells[m_iCurrentCellIndex]->Render();
    }

    return S_OK;
}

#endif

void CNavigation::AStar()
{
    if (m_iCurrentCellIndex == m_iDestCell || m_iDestCell == -1 || m_iCurrentCellIndex == -1) {
        return;
    }
    m_reservedPath.clear();

    priority_queue<AStarNode, vector<AStarNode>, greater<AStarNode>> pq;

    list<_int> visitedPos;

    map<_int, AStarNode> parentPos = {};

    AStarNode startPos;
    ZeroMemory(&startPos, sizeof(startPos));

    startPos.F = XMVectorGetX(XMVector3Length(m_Cells[m_iDestCell]->Get_Center() - m_Cells[m_iCurrentCellIndex]->Get_Center()));
    startPos.G = 0;
    startPos.pos = m_iCurrentCellIndex;
    pq.push(startPos);

    _bool bFound = false;

    while (!pq.empty())
    {
        AStarNode topNode = pq.top();
        pq.pop();
        visitedPos.push_back(topNode.pos);

        CCell* pTopCell = m_Cells[topNode.pos];

        if (topNode.pos == m_iDestCell) {
            bFound = true;
            break;
        }

        for (_int i = 0; i < CCell::LINE_END; i++) {
            // 이웃 인덱스 구하기
            _int iNeighbor = pTopCell->Get_Neighbor((CCell::LINE)i);
            if (iNeighbor == -1 || abs(XMVectorGetY(m_Cells[iNeighbor]->Get_Normal())) < abs(XMVectorGetX(m_Cells[iNeighbor]->Get_Normal()))
                || abs(XMVectorGetY(m_Cells[iNeighbor]->Get_Normal())) < abs(XMVectorGetZ(m_Cells[iNeighbor]->Get_Normal()))) {
                continue;
            }

            _bool visited = false;
            // 이미 방문했는지 확인
            for (_int index : visitedPos) {
                if (index == iNeighbor) {
                    visited = true;
                    break;
                }
            }
            if (visited == true) {
                continue;
            }

            _float g = topNode.G + XMVectorGetX(XMVector3Length(m_Cells[iNeighbor]->Get_Center() - m_Cells[topNode.pos]->Get_Center()));
            _float h = XMVectorGetX(XMVector3Length(m_Cells[m_iDestCell]->Get_Center() - m_Cells[iNeighbor]->Get_Center()));
            _float f = g + h;
            if (parentPos.find(iNeighbor) != end(parentPos)
                && parentPos[iNeighbor].G < topNode.G) {
                continue;
            }
            parentPos[iNeighbor] = topNode;

            pq.push({ f, g, iNeighbor });
        }
    }

    _int pos = m_iDestCell;

    while (pos != m_iCurrentCellIndex) {
        if (!bFound)
            break;
        m_reservedPath.push_front(pos);
        pos = parentPos.find(pos)->second.pos;
    }
}

void CNavigation::Set_FinalDest(_fvector pos)
{
    _int i = 0;
    for (auto* pCell : m_Cells) {
        if (PointInTriangle(pCell->Get_Point(CCell::POINT_A),
            pCell->Get_Point(CCell::POINT_B), pCell->Get_Point(CCell::POINT_C), pos))
        {
            m_iDestCell = i;
            return;
        }
        i++;
    }
}

pair<_float3, _float3> CNavigation::Get_NextLine() const
{
    if (m_iCurrentCellIndex == -1) {
        return { {0.f,0.f,0.f}, {0.f, 0.f, 0.f} };
    }

    _float3 ret1{}; _float3 ret2{};
    if (m_reservedPath.size() == 0) {
        return { {}, {} };
    }
    _int nextCell = m_reservedPath.front();
    CCell* pCurCell = m_Cells[m_iCurrentCellIndex];

    _int iLine = CCell::LINE_AB;
    for (; iLine < CCell::LINE_END; iLine++) {
        if (nextCell == pCurCell->Get_Neighbor((CCell::LINE)iLine)) {
            break;
        }
    }
    if (iLine == CCell::LINE_END) {
        ret1 = {}; ret2 = {};
    }
    else if (iLine == CCell::LINE_AB) {
        XMStoreFloat3(&ret1, pCurCell->Get_Point(CCell::POINT_A));
        XMStoreFloat3(&ret2, pCurCell->Get_Point(CCell::POINT_B));
    }
    else if (iLine == CCell::LINE_BC) {
        XMStoreFloat3(&ret1, pCurCell->Get_Point(CCell::POINT_B));
        XMStoreFloat3(&ret2, pCurCell->Get_Point(CCell::POINT_C));
    }
    else if (iLine == CCell::LINE_CA) {
        XMStoreFloat3(&ret1, pCurCell->Get_Point(CCell::POINT_C));
        XMStoreFloat3(&ret2, pCurCell->Get_Point(CCell::POINT_A));
    }

    return { ret1, ret2 };
}

_vector CNavigation::Get_NextCellPos() const
{
    return m_Cells[m_reservedPath.front()]->Get_Center();
}

list<class CCollider*> CNavigation::Get_ColliderBelonged() const
{
    list<class CCollider*> ret;
    if (m_iCurrentCellIndex) {
        return ret;
    }

    ret = m_Cells[m_iCurrentCellIndex]->Get_Colliders();
    for (_int i = 0; i < 3; i++) {
        _int neighbor = m_Cells[m_iCurrentCellIndex]->Get_Neighbor((CCell::LINE)i);
        if (neighbor == -1) {
            continue;
        }
        const list<class CCollider*>& next = m_Cells[neighbor]->Get_Colliders();
        ret.insert(ret.begin(), next.begin(), next.end());
    }

    /*if (!m_reservedPath.empty()) {
        const list<class CCollider*>& next = m_Cells[m_reservedPath.front()]->Get_Colliders();
        ret.insert(ret.begin(), next.begin(), next.end());
    }*/
    return ret;
}

HRESULT CNavigation::SetUp_Neighbors()
{
    int i = 0;
    for (auto& pSourCell : m_Cells)
    {
        int j = 0;
        for (auto& pDestCell : m_Cells)
        {
            if (pSourCell == pDestCell)
                continue;

            if (pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
            {
                pSourCell->Set_Neighbor(CCell::LINE_AB, pDestCell);
            }
            if (pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
            {
                pSourCell->Set_Neighbor(CCell::LINE_BC, pDestCell);
            }
            if (pDestCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
            {
                pSourCell->Set_Neighbor(CCell::LINE_CA, pDestCell);
                int a = 10;
            }
            j++;
        }
        i++;
    }

    return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& strNavigationDataFile)
{
    CNavigation* pInstance = new CNavigation(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(strNavigationDataFile)))
    {
        MSG_BOX(TEXT("Failed to Created : CNavigation"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CNavigation::Clone(void* pArg)
{
    CNavigation* pInstance = new CNavigation(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CNavigation"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNavigation::Free()
{
    __super::Free();

    for (auto& pCell : m_Cells)
        Safe_Release(pCell);

    m_Cells.clear();

#ifdef _DEBUG
    Safe_Release(m_pShader);
#endif
}
