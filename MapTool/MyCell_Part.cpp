#include "MyCell_Part.h"
#include "VIBuffer.h"

CMyCell_Part::CMyCell_Part(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

void CMyCell_Part::Free()
{
    __super::Free();
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pVIBuffer);
}
