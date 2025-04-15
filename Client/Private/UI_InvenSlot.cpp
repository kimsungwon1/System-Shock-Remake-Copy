#include "stdafx.h"
#include "UI_InvenSlot.h"

#include "Client_Defines.h"

#include "GameInstance.h"

CUI_InvenSlot::CUI_InvenSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CUI_InvenSlot::CUI_InvenSlot(const CUI_InvenSlot& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CUI_InvenSlot::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_InvenSlot::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) {
        return E_FAIL;
    }
    if (FAILED(Ready_Components())) {
        return E_FAIL;
    }

    m_iRenderPass = 11;

    return S_OK;
}

void CUI_InvenSlot::Priority_Update(_float fTimeDelta)
{
    

}

void CUI_InvenSlot::Update(_float fTimeDelta)
{
}

void CUI_InvenSlot::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_InvenSlot::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    //HRESULT ret = m_pGameInstance->Render_Text(TEXT("font1"), m_strQuote.c_str(), XMVectorSet(m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, 0.f, 1.f));
    //return ret;
    return S_OK;
}

HRESULT CUI_InvenSlot::Ready_PartUI()
{
    return S_OK;
}

HRESULT CUI_InvenSlot::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenSlot"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;
    return S_OK;
}

CUI_InvenSlot* CUI_InvenSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_InvenSlot* pInstance = new CUI_InvenSlot(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CUI_InvenSlot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_InvenSlot::Clone(void* pArg)
{
    CUI_InvenSlot* pInstance = new CUI_InvenSlot(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CUI_InvenSlot"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_InvenSlot::Free()
{
    __super::Free();
}