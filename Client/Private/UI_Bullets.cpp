#include "stdafx.h"
#include "UI_Bullets.h"

#include "FPSPlayer.h"
#include "Inventory.h"

#include "GameInstance.h"

CUI_Bullets::CUI_Bullets(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject {pDevice, pContext}
{
}

CUI_Bullets::CUI_Bullets(const CUI_Bullets& Prototype)
    : CUIObject { Prototype }
{
}

HRESULT CUI_Bullets::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Bullets::Initialize(void* pArg)
{
    UI_DESC			Desc{};

    Desc.fSizeX = 300;
    Desc.fSizeY = 80;

    Desc.fX = (_float)g_iWinSizeX - Desc.fSizeX - 10.f;
    Desc.fY = (_float)g_iWinSizeY - Desc.fSizeY - 50.f;

    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);
    Desc.pParent = nullptr;

    if (FAILED(__super::Initialize(&Desc))) {
        return E_FAIL;
    }

    if (FAILED(Ready_PartUI()))
        return E_FAIL;

    m_eType = TYPE_BULLETS;

    return S_OK;
}

void CUI_Bullets::Priority_Update(_float fTimeDelta)
{
    if (nullptr == m_pPlayer) {
        m_pPlayer = dynamic_cast<CFPSPlayer*>(m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
    }
    
    __super::Priority_Update(fTimeDelta);
}

void CUI_Bullets::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CUI_Bullets::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_Bullets::Render()
{
    _int iLoad = m_pPlayer->Get_CurrentWeaponLoad();
    if (iLoad == -1) {
        return S_OK;
    }
    _wstring wstr = to_wstring(iLoad);

    m_pGameInstance->Render_Text(TEXT("font1"), wstr.c_str(), XMVectorSet(m_fX, m_fY, 1.f, 1.f));

    return S_OK;
}

HRESULT CUI_Bullets::Ready_PartUI()
{
    return S_OK;
}

HRESULT CUI_Bullets::Ready_Components()
{
    /* FOR.Com_Shader */
    //if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
    //    TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    //    return E_FAIL;

    ///* FOR.Com_Texture */
    //if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Button"),
    //    TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
    //    return E_FAIL;

    ///* FOR.Com_VIBuffer */
    //if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
    //    TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
    //    return E_FAIL;
    return S_OK;
}

CUI_Bullets* CUI_Bullets::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Bullets* pInstance = new CUI_Bullets(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CUI_Bullets"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Bullets::Clone(void* pArg)
{
    CUI_Bullets* pInstance = new CUI_Bullets(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CUI_Bullets"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Bullets::Free()
{
    __super::Free();
}
