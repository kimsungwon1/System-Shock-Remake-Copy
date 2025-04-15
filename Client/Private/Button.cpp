#include "stdafx.h"
#include "Button.h"

#include "GameInstance.h"

CButton::CButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{pDevice, pContext}
{
}

CButton::CButton(const CButton& Prototype)
    : CUIObject { Prototype }
{
}

HRESULT CButton::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CButton::Initialize(void* pArg)
{
    auto* pDesc = static_cast<BUTTON_DESC*>(pArg);

    m_strQuote = pDesc->strQuote;

    if (FAILED(__super::Initialize(pDesc))) {
        return E_FAIL;
    }
    if (FAILED(Ready_Components())) {
        return E_FAIL;
    }

    m_iRenderPass = 9;

    return S_OK;
}

void CButton::Priority_Update(_float fTimeDelta)
{
    POINT p = { };

    GetCursorPos(&p);
    ScreenToClient(g_hWnd, &p);
    RECT rect = { };
    SetRect(&rect, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);
    if (PtInRect(&rect, p)) {
        m_bMouseOn = true;
    }
    else {
        m_bMouseOn = false;
    }

}

void CButton::Update(_float fTimeDelta)
{
}

void CButton::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CButton::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    HRESULT ret = m_pGameInstance->Render_Text(TEXT("font1"), m_strQuote.c_str(), XMVectorSet(m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, 0.f, 1.f));
    return ret;
}

HRESULT CButton::Ready_PartUI()
{
    return S_OK;
}

HRESULT CButton::Ready_Components()
{
    /* FOR.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Button"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;
    return S_OK;
}

CButton* CButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CButton* pInstance = new CButton(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CButton"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CButton::Clone(void* pArg)
{
    CButton* pInstance = new CButton(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CButton"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CButton::Free()
{
    __super::Free();
}
