#include "MyOneVertex.h"
#include "VIBuffer.h"

CMyOneVertex::CMyOneVertex(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CMyCell_Part(pDevice, pContext)
{
}

HRESULT CMyOneVertex::Initialize(const _float3* pPoints, _int iIndex)
{
    m_vPosition = *pPoints;

    m_iIndex = iIndex;

    //m_pVIBuffer = 

    return S_OK;
}

HRESULT CMyOneVertex::Render()
{
    m_pVIBuffer->Bind_Buffers();

    m_pVIBuffer->Render();

    return S_OK;
}

CMyOneVertex* CMyOneVertex::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex)
{
    CMyOneVertex* pInstance = new CMyOneVertex(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pPoints, iIndex))) {
        MSG_BOX(TEXT("Failed to Created : CMyOneVertex"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMyOneVertex::Free()
{
    __super::Free();
}
