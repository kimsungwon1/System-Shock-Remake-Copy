#include "MyCell_Line.h"
#include "MyNavigation.h"
#include "MyCell_Full.h"

#include "Shader.h"
#include "GameInstance.h"
#include "Transform.h"

CMyNavigation::CMyNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
}

CMyNavigation::CMyNavigation(const CMyNavigation& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CMyNavigation::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMyNavigation::Initialize(void* pArg)
{
    CGameObject::GAMEOBJECT_DESC desc = {};
    desc.fRotationPerSec = 0.f;
    desc.fSpeedPerSec = 0.f;
    XMStoreFloat4x4(&desc.transMat, XMMatrixIdentity());
    if (FAILED(__super::Initialize(&desc))) {
        return E_FAIL;
    }
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("Shader_Cell.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements);

    if (m_pShader == nullptr)
        return E_FAIL;

    return S_OK;
}

HRESULT CMyNavigation::LoadFile(const _wstring& strNavigationDataFile)
{
    _ulong dwByte = {};

    HANDLE hFile = CreateFile(strNavigationDataFile.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (0 == hFile)
        return E_FAIL;

    _float3 vPoints[3];

    while (true)
    {
        ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
        if (0 == dwByte)
            break;
        //CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
        CMyCell_Full* pCell = CMyCell_Full::Create(m_pDevice, m_pContext, vPoints, m_Cells.size());
        if (nullptr == pCell)
            return E_FAIL;
        m_Cells.emplace_back(pCell);
    }

    CloseHandle(hFile);

    if (FAILED(SetUp_Neighbors()))
        return E_FAIL;

    return S_OK;
}

void CMyNavigation::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CMyNavigation::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float4 vColor = _float4(0.f, 1.f, 0.f, 1.f);

    m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));

    m_pShader->Begin(0);

    for (auto& pCell : m_Cells)
        pCell->Render();

    // line draw
    vColor = _float4(1.f, 0.f, 0.f, 1.f);

    m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4));
    m_pShader->Begin(0);

    for (auto& pLine : m_Lines)
        pLine->Render();

    return S_OK;
}

HRESULT CMyNavigation::AddCellFull(const _float3* pVec)
{
    _float3 pRealVec[3] = {};
    pRealVec[0] = pVec[0];
    pRealVec[1] = pVec[1];
    pRealVec[2] = pVec[2];

    SortCell(pRealVec);
    CMyCell_Full* pCell = CMyCell_Full::Create(m_pDevice, m_pContext, pRealVec, m_Cells.size());
    if (nullptr == pCell)
        return E_FAIL;
    m_Cells.emplace_back(pCell);

    return S_OK;
}

HRESULT CMyNavigation::AddCellLine(const _float3* pVec)
{
    CMyCell_Full* pLine = CMyCell_Full::Create(m_pDevice, m_pContext, pVec, m_Cells.size());
    if (nullptr == pLine)
        return E_FAIL;
    m_Lines.emplace_back(pLine);

    return S_OK;
}

HRESULT CMyNavigation::DeleteCell(const CMyCell_Full* pDel)
{
    for (auto iter = begin(m_Cells); iter != end(m_Cells); iter++) {
        if (*iter == pDel) {
            Safe_Release(*iter);
            m_Cells.erase(iter);
            return S_OK;
        }

    }

    return E_FAIL;
}

HRESULT CMyNavigation::ClearLines()
{
    for (auto& pLine : m_Lines) {
        Safe_Release(pLine);
    }
    m_Lines.clear();

    return S_OK;
}

HRESULT CMyNavigation::ClearAll()
{
    for (auto& pCell : m_Cells) {
        Safe_Release(pCell);
    }
    for (auto& pLine : m_Lines) {
        Safe_Release(pLine);
    }
    m_Cells.clear(); m_Lines.clear();

    return S_OK;
}

HRESULT CMyNavigation::SortCell(_float3* pVec)
{
    if (pVec[0].x > pVec[1].x) {
        _float3 tmp = pVec[1];
        pVec[1] = pVec[0];
        pVec[0] = tmp;
    }
    _float value = (pVec[1].z - pVec[0].z) / (pVec[1].x - pVec[0].x) * (pVec[2].x - pVec[0].x) + pVec[0].z;

    if (value < pVec[2].z) {
        _float3 tmp = pVec[2];
        pVec[2] = pVec[0];
        pVec[0] = tmp;
    }

    return S_OK;
}

CMyCell_Full* CMyNavigation::Cell_Picking(_fvector vCamPos, _float3* pOut)
{
    _float fMinDistance = 100000.f;
    CMyCell_Full* pDest = nullptr;
    for (auto* pCell : m_Cells) {

        if (pCell->IsPicking(m_pTransformCom->Get_WorldMatrix_Ptr(), pOut))
        {
            _float fDistance = XMVectorGetX(XMVector3Length(vCamPos - XMLoadFloat3(pOut)));
            if (fDistance < fMinDistance) {
                fMinDistance = fDistance;
                pDest = pCell;
            }
        }
    }

    return pDest;
}

_float3 CMyNavigation::Get_AlmostSamePoint(const _float3& pIn) const
{
    _float3 result = pIn;

    for (auto iter : m_Cells) {
        _vector vA = iter->Get_Point(CMyCell_Full::POINT_A);
        _vector vB = iter->Get_Point(CMyCell_Full::POINT_B);
        _vector vC = iter->Get_Point(CMyCell_Full::POINT_C);

        if (abs(vA.m128_f32[0] - pIn.x) < 0.1 &&
            abs(vA.m128_f32[1] - pIn.y) < 0.1 &&
            abs(vA.m128_f32[2] - pIn.z) < 0.1)
            XMStoreFloat3(&result, vA);
        if (abs(vC.m128_f32[0] - pIn.x) < 0.1 &&
            abs(vC.m128_f32[1] - pIn.y) < 0.1 &&
            abs(vC.m128_f32[2] - pIn.z) < 0.1)
            XMStoreFloat3(&result, vC);
        if (abs(vB.m128_f32[0] - pIn.x) < 0.1 &&
            abs(vB.m128_f32[1] - pIn.y) < 0.1 &&
            abs(vB.m128_f32[2] - pIn.z) < 0.1)
            XMStoreFloat3(&result, vB);
    }

    return result;
}

HRESULT CMyNavigation::SaveNavi(ofstream& ofs)
{
    _uint cellSize = m_Cells.size();

    ofs.write(reinterpret_cast<const char*>(&cellSize), sizeof(_uint));

    for (auto* pCell : m_Cells) {
        pCell->SaveCell(ofs);
    }

    return S_OK;
}

HRESULT CMyNavigation::LoadNavi(ifstream& ifs)
{
    _uint cellSize = {};

    ifs.read(reinterpret_cast<char*>(&cellSize), sizeof(_uint));


    for(_uint i = 0; i < cellSize; i++)
    {
        _float3 pRealVec[3] = {};

        ifs.read(reinterpret_cast<char*>(pRealVec), sizeof(_float3) * 3);

        CMyCell_Full* pCell = CMyCell_Full::Create(m_pDevice, m_pContext, pRealVec, m_Cells.size());
        m_Cells.emplace_back(pCell);
    }

    return S_OK;
}

HRESULT CMyNavigation::SetUp_Neighbors()
{
    return S_OK;
}

CMyNavigation* CMyNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMyNavigation* pInstance = new CMyNavigation(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CMyNavigation"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMyNavigation::Clone(void* pArg)
{
    CMyNavigation* pInstance = new CMyNavigation(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CMyNavigation"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMyNavigation::Free()
{
    __super::Free();

    Safe_Release(m_pShader);

    for (auto& pCell : m_Cells) {
        Safe_Release(pCell);
    }
    for (auto& pLine : m_Lines) {
        Safe_Release(pLine);
    }
    m_Cells.clear(); m_Lines.clear();
}
