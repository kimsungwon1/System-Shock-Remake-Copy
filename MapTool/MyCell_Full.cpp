#include "MyCell_Full.h"
#include "VIBuffer_Cell.h"
#include "GameInstance.h"

CMyCell_Full::CMyCell_Full(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CMyCell_Part(pDevice, pContext)
{
}

HRESULT CMyCell_Full::Initialize(const _float3* pPoints, _int iIndex)
{
    //memcpy(m_vPoints, pPoints, sizeof(_float3) * POINT_END);
    m_vPoints[0] = pPoints[0];
    m_vPoints[1] = pPoints[1];
    m_vPoints[2] = pPoints[2];


    m_iIndex = iIndex;

    m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    return S_OK;
}

_bool CMyCell_Full::IsPicking(const _float4x4* naviWorldMat, _float3* pOut)
{
    auto* pGameInstance = CGameInstance::Get_Instance();
    pGameInstance->Transform_MouseRay_ToLocalSpace(*naviWorldMat);

    _vector v1 = XMLoadFloat3(&m_vPoints[0]);
    _vector v2 = XMLoadFloat3(&m_vPoints[1]);
    _vector v3 = XMLoadFloat3(&m_vPoints[2]);

    if (pGameInstance->isPicked_InLocalSpace(v1, v2, v3, pOut)) {
        XMStoreFloat3(pOut, XMVector3TransformCoord(XMLoadFloat3(pOut), XMLoadFloat4x4(naviWorldMat)));
        return true;
    }

    return false;
}

HRESULT CMyCell_Full::SaveCell(ofstream& ofs)
{
    ofs.write(reinterpret_cast<const char*>(m_vPoints), sizeof(_float3) * 3);

    return S_OK;
}

HRESULT CMyCell_Full::Render()
{
    m_pVIBuffer->Bind_Buffers();

    m_pVIBuffer->Render();

    return S_OK;
}

CMyCell_Full* CMyCell_Full::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
    CMyCell_Full* pInstance = new CMyCell_Full(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pPoints, iIndex)))
    {
        MSG_BOX(TEXT("Failed to Created : CMyCell_Full"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMyCell_Full::Free()
{
    __super::Free();
}
