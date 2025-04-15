#include "stdafx.h"

#include "UI_Info.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "FPSPlayer.h"

#include "GameInstance.h"

CUI_Info::CUI_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CUI_Info::CUI_Info(const CUI_Info& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CUI_Info::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Info::Initialize(void* pArg)
{
    auto* pDesc = static_cast<CUI_Info::INFO_DESCS*>(pArg);
    m_strText = pDesc->strQuote;
    m_fLifeTime.y = pDesc->fLifeTime;

    UI_DESC			Desc{};

    Desc.fSizeX = 500;
    Desc.fSizeY = 80;

    Desc.fX = (_float)g_iWinSizeX / 2.f;
    Desc.fY = (_float)g_iWinSizeY - 80.f;

    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);
    Desc.pParent = nullptr;

    if (FAILED(__super::Initialize(&Desc))) {
        return E_FAIL;
    }

    if (FAILED(Ready_Components())) {
        return E_FAIL;
    }

    m_eType = TYPE_HEALTHBAR;
    m_iRenderPass = 10;

    return S_OK;
}

void CUI_Info::Priority_Update(_float fTimeDelta)
{
    if (m_fFadeTime.x >= m_fFadeTime.y && m_fLifeTime.y != -1) {
        m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_UI_Info"), this);
    }
    if (m_fLifeTime.x >= m_fLifeTime.y && m_fLifeTime.y != -1) {
        m_fFadeTime.x += fTimeDelta;
    }
    m_fLifeTime.x += fTimeDelta;

    __super::Priority_Update(fTimeDelta);
}

void CUI_Info::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CUI_Info::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_Info::Render()
{
    /*if (FAILED(__super::Render()))
        return E_FAIL;*/

    if (FAILED(m_pGameInstance->Render_Text(TEXT("font1"), m_strText.c_str(), XMVectorSet(m_fX - m_fSizeX * 0.25f, m_fY, 0.f, 1.f))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Info::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TextBox"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CUI_Info* CUI_Info::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Info* pInstance = new CUI_Info(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CUI_Info"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Info::Clone(void* pArg)
{
    CUI_Info* pInstance = new CUI_Info(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CUI_Info"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Info::Free()
{
    __super::Free();
}
