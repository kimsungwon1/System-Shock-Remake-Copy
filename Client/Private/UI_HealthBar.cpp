#include "stdafx.h"
#include "UI_HealthBar.h"
#include "Shader.h"
#include "Texture.h"
#include "VIBuffer_Rect.h"
#include "FPSPlayer.h"

#include "GameInstance.h"

CUI_HealthBar::CUI_HealthBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CUI_HealthBar::CUI_HealthBar(const CUI_HealthBar& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CUI_HealthBar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_HealthBar::Initialize(void* pArg)
{
    UI_DESC			Desc{};

    Desc.fSizeX = 300;
    Desc.fSizeY = 80;

    Desc.fX = (_float)g_iWinSizeX - Desc.fSizeX;
    Desc.fY = Desc.fSizeY + 10.f;

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

    return S_OK;
}

void CUI_HealthBar::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CUI_HealthBar::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CUI_HealthBar::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);

    if (nullptr == m_pPlayer) {
        m_pPlayer = dynamic_cast<CFPSPlayer*>(m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
    }
}

HRESULT CUI_HealthBar::Render()
{
    if(m_pPlayer != nullptr)
    {
        _int iHP = m_pPlayer->Get_HP();
        if (FAILED(m_pShaderCom->Bind_RawValue("g_iHP", &iHP, sizeof(_int))))
            return E_FAIL;
    }

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(TYPE_HEALTHBAR)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_HealthBar::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HealthBar"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CUI_HealthBar* CUI_HealthBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_HealthBar* pInstance = new CUI_HealthBar(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CUI_HealthBar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_HealthBar::Clone(void* pArg)
{
    CUI_HealthBar* pInstance = new CUI_HealthBar(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CUI_HealthBar"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_HealthBar::Free()
{
    __super::Free();
}
