#include "Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager()
{
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint iIndex) const
{
    auto iter = begin(m_Lights);

    for (size_t i = 0; i < iIndex; i++)
    {
        ++iter;
    }

    return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Initialize()
{
    return S_OK;
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& LightDesc, _int iIndex)
{
    CLight* pLight = CLight::Create(LightDesc, iIndex);
    if (nullptr == pLight)
        return E_FAIL;

    m_Lights.push_back(pLight);

    return S_OK;
}

HRESULT CLight_Manager::Change_Light(const LIGHT_DESC& LightDesc, _int iIndex)
{
    for (auto* pLight : m_Lights) {
        if (pLight->Get_Index() == iIndex) {
            pLight->Set_LightDesc(LightDesc);
            return S_OK;
        }
    }

    return E_FAIL;
}

HRESULT CLight_Manager::Delete_Light(_int iIndex)
{
    for (auto light = m_Lights.begin(); light != m_Lights.end(); light++) {
        if ((*light)->Get_Index() == iIndex) {
            Safe_Release(*light);
            m_Lights.erase(light);
            return S_OK;
        }
    }
    return E_FAIL;
}

HRESULT CLight_Manager::Render(CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    for (auto& pLight : m_Lights)
        pLight->Render(pShader, pVIBuffer);

    return S_OK;
}

CLight_Manager* CLight_Manager::Create()
{
    CLight_Manager* pInstance = new CLight_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Cloned : CLight_Manager"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLight_Manager::Free()
{
    __super::Free();

    for (auto& pLight : m_Lights)
        Safe_Release(pLight);

    m_Lights.clear();
}
