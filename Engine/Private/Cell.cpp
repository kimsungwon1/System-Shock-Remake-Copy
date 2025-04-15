#include "Cell.h"

#ifdef _DEBUG
#include "VIBuffer_Cell.h"
#endif

_bool IsNewIntersect(_fvector vL1V1, _fvector vL1V2, _fvector vL2V1, _fvector vL2V2) {
    _vector VL1V1 = vL1V1; VL1V1 = XMVectorSetY(VL1V1, 0.f);
    _vector VL1V2 = vL1V2; VL1V2 = XMVectorSetY(VL1V2, 0.f);
    _vector VL2V1 = vL2V1; VL2V1 = XMVectorSetY(VL2V1, 0.f);
    _vector VL2V2 = vL2V2; VL2V2 = XMVectorSetY(VL2V2, 0.f);

    _vector vL1 = VL1V2 - VL1V1;

    _vector vL2 = VL2V2 - VL2V1;

    _vector vL1Normal = XMVector3Normalize(vL1);
    _vector vL2Normal = XMVector3Normalize(vL2);

    _float vL1Length = XMVectorGetX(XMVector3Length(vL1));
    _float vL2Length = XMVectorGetY(XMVector3Length(vL2));

    if (XMVector3Equal(vL1Normal, vL2Normal)) {
        return false;
    }

    _float fX1 = XMVectorGetX(VL1V1); _float fZ1 = XMVectorGetZ(VL1V1);
    _float fX2 = XMVectorGetX(VL2V1); _float fZ2 = XMVectorGetZ(VL2V1);

    _float fA1 = XMVectorGetX(vL1Normal); _float fB1 = XMVectorGetZ(vL1Normal);
    _float fA2 = XMVectorGetX(vL2Normal); _float fB2 = XMVectorGetZ(vL2Normal);

    _float fS = (fB1 * (fX2 - fX1) - fA1 * (fZ2 - fZ1)) / (fA1 * fB2 - fA2 * fB2);
    _float fT = (fX2 - fX1 + fS * fA2) / fA1;

    _vector vIntersectedPos = VL1V1 + fT * vL1Normal;

    _vector vInterV1 = vIntersectedPos - VL1V1; _vector vInterV2 = vIntersectedPos - VL1V2;
    _float f1 = XMVectorGetX(vInterV1) / XMVectorGetX(vL1Normal);
    _float f2 = XMVectorGetX(vInterV2) / XMVectorGetX(vL1Normal);
    if (f1 < 0 && f2 > 0 || f1 > 0 && f2 < 0) {
        return true;
    }
    else
        return false;
}

CCell::CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

_vector CCell::Get_Center() const
{
    _vector ret = {};
    ret.m128_f32[0] = (m_vPoints[0].x + m_vPoints[1].x + m_vPoints[2].x) / 3;
    ret.m128_f32[1] = (m_vPoints[0].y + m_vPoints[1].y + m_vPoints[2].y) / 3;
    ret.m128_f32[2] = (m_vPoints[0].z + m_vPoints[1].z + m_vPoints[2].z) / 3;

    return XMVectorSetW(ret, 1.f);
}

HRESULT CCell::Initialize(const _float3* pPoints, _int iIndex)
{
    memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);

    m_iIndex = iIndex;

#ifdef _DEBUG
    m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

#endif

    return S_OK;
}

_bool CCell::Compare_Points(_fvector vSour, _fvector vDest)
{
    if (XMVector3Equal(vSour, XMLoadFloat3(&m_vPoints[POINT_A])))
    {
        if (XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_B])))
            return true;
        if (XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_C])))
            return true;
    }
    if (XMVector3Equal(vSour, XMLoadFloat3(&m_vPoints[POINT_B])))
    {
        if (XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_C])))
            return true;
        if (XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_A])))
            return true;
    }
    if (XMVector3Equal(vSour, XMLoadFloat3(&m_vPoints[POINT_C])))
    {
        if (XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_A])))
            return true;
        if (XMVector3Equal(vDest, XMLoadFloat3(&m_vPoints[POINT_B])))
            return true;
    }

    return false;
}

_bool CCell::isIn(_fvector vPosition, int* pNeighborIndex, _float3& vLineOut, _int iCurrentCellIndex)
{
    _float fMaxHeight = -1.f;
    if (iCurrentCellIndex != -1) {
        fMaxHeight = GetHeight(XMVectorGetX(vPosition), XMVectorGetZ(vPosition));
    }
    _float fOriMax = fMaxHeight;
    for (auto* pCell : m_vecAboveCells) {
        _float fHeight = pCell->GetHeight(XMVectorGetX(vPosition), XMVectorGetZ(vPosition));

        if (fHeight > XMVectorGetY(vPosition))
            continue;
        _vector vCellNormal = XMVector3Normalize(pCell->Get_Normal());
        if (abs(XMVectorGetY(vCellNormal)) < abs(XMVectorGetX(vCellNormal)) || abs(XMVectorGetY(vCellNormal)) < abs(XMVectorGetZ(vCellNormal))) {
            continue;
        }
        else if (fHeight > fMaxHeight) {
            _bool bIsInPCell = true;
            for (size_t i = 0; i < LINE_END; i++) {
                _vector vSour, vDest;

                vSour = XMVector3Normalize(vPosition - pCell->Get_Point((CCell::POINT)i));

                _vector vLine = pCell->Get_Point(CCell::POINT((i + 1) % 3)) - pCell->Get_Point((CCell::POINT)i);
                vDest = XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f);

                _float fDot = XMVectorGetX(XMVector3Dot(vSour, vDest));
                if (0 < fDot)
                {
                    bIsInPCell = false;
                }
            }
            if (bIsInPCell == true)
            {
                *pNeighborIndex = pCell->m_iIndex;
                fMaxHeight = fHeight;
            }
        }
    }
    if (fMaxHeight != fOriMax) {
        return false;
    }

    for (size_t i = 0; i < LINE_END; i++) {
        _vector vSour, vDest;

        vSour = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));

        _vector vLine = XMLoadFloat3(&m_vPoints[(i + 1) % 3]) - XMLoadFloat3(&m_vPoints[i]);
        vDest = XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f);

        if (0 < XMVectorGetX(XMVector3Dot(vSour, vDest)))
        {
            XMStoreFloat3(&vLineOut, vLine);

            *pNeighborIndex = m_iNeighborIndices[i];
            return false;
        }
    }
    return true;
}

_bool CCell::isIn(_fvector vPosition)
{
    for (size_t i = 0; i < LINE_END; i++) {
        _vector vSour, vDest;

        vSour = XMVector3Normalize(vPosition - XMLoadFloat3(&m_vPoints[i]));

        _vector vLine = XMLoadFloat3(&m_vPoints[(i + 1) % 3]) - XMLoadFloat3(&m_vPoints[i]);
        vDest = XMVectorSet(XMVectorGetZ(vLine) * -1.f, 0.f, XMVectorGetX(vLine), 0.f);

        if (0 < XMVectorGetX(XMVector3Dot(vSour, vDest)))
        {
            return false;
        }
    }
    return true;
}

_float CCell::GetHeight(_float x, _float z)
{
    auto plane = XMPlaneFromPoints(XMLoadFloat3(&m_vPoints[0]), XMLoadFloat3(&m_vPoints[1]), XMLoadFloat3(&m_vPoints[2]));
    return (-plane.m128_f32[0] * x - plane.m128_f32[2] * z - plane.m128_f32[3]) / plane.m128_f32[1];
}

void CCell::Erase_Collider(CCollider* pCollider)
{
    for (auto iter = begin(m_CollidersBelonged); iter != end(m_CollidersBelonged);) {
        if (*iter == pCollider) {
            iter = m_CollidersBelonged.erase(iter);
        }
        else {
            iter++;
        }
    }
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
    m_pVIBuffer->Bind_Buffers();

    m_pVIBuffer->Render();

    return S_OK;
}
#endif

void CCell::Set_AboveCells(CCell* pCell)
{
    pair<int, int> lines[3] = { {0,1}, {1, 2}, {2, 0} };
    if (pCell->isIn(this->Get_Center()) || isIn(pCell->Get_Center()))
    {
        m_vecAboveCells.push_back(pCell);
        return;
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (IsNewIntersect(XMLoadFloat3(&m_vPoints[lines[i].first]), XMLoadFloat3(&m_vPoints[lines[i].second]),
                pCell->Get_Point((CCell::POINT)lines[j].first), pCell->Get_Point((CCell::POINT)lines[j].second))) {
                m_vecAboveCells.push_back(pCell);
                return;
            }
        }
    }
}

CCell* CCell::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
    CCell* pInstance = new CCell(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pPoints, iIndex)))
    {
        MSG_BOX(TEXT("Failed to Created : CCell"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCell::Free()
{
    __super::Free();

#ifdef _DEBUG
    Safe_Release(m_pVIBuffer);
#endif

    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}

