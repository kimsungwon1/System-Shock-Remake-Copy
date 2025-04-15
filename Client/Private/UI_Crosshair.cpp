#include "stdafx.h"
#include "UI_Crosshair.h"
#include "Texture.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CUI_Crosshair::CUI_Crosshair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject { pDevice, pContext }
{
}

CUI_Crosshair::CUI_Crosshair(const CUI_Crosshair& Prototype)
    : CUIObject { Prototype }
{
}

HRESULT CUI_Crosshair::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Crosshair::Initialize(void* pArg)
{
    UI_DESC			Desc{};

    Desc.fSizeX = 20;
    Desc.fSizeY = 20;

    Desc.fX = g_iWinSizeX >> 1;
    Desc.fY = g_iWinSizeY >> 1;

    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);
    Desc.pParent = nullptr;

    if (FAILED(__super::Initialize(&Desc))) {
        return E_FAIL;
    }

    if (FAILED(Ready_Components())) {
        return E_FAIL;
    }

    m_eType = TYPE_CROSSHAIR;

    return S_OK;
}

void CUI_Crosshair::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CUI_Crosshair::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CUI_Crosshair::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_Crosshair::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(3)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Crosshair::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Crosshair"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CUI_Crosshair* CUI_Crosshair::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Crosshair* pInstance = new CUI_Crosshair(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CUI_Crosshair"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Crosshair::Clone(void* pArg)
{
    CUI_Crosshair* pInstance = new CUI_Crosshair(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CUI_Crosshair"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Crosshair::Free()
{
    __super::Free();
}
