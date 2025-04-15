#include "stdafx.h"
#include "ElevatorUI.h"
#include "Button.h"

#include "GameInstance.h"

CElevatorUI::CElevatorUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject { pDevice, pContext }
{
}

CElevatorUI::CElevatorUI(const CElevatorUI& Prototype)
    : CUIObject(Prototype)
{
}

HRESULT CElevatorUI::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CElevatorUI::Initialize(void* pArg)
{
    UI_DESC			Desc{};

    Desc.fX = g_iWinSizeX >> 1;
    Desc.fY = g_iWinSizeY >> 1;
    Desc.fSizeX = 1;
    Desc.fSizeY = 1;

    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);
    Desc.pParent = nullptr;

    if (FAILED(__super::Initialize(&Desc))) {
        return E_FAIL;
    }

    if (FAILED(Ready_PartUI()))
        return E_FAIL;

    m_eType = TYPE_ELEVATOR;

    m_pGameInstance->Show_Cursor(true);

    return S_OK;
}

void CElevatorUI::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CElevatorUI::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    // LEVEL1 : vPlayerPosition = { 3.0, 0.1, 1.9 } / { 11.11f, 0.1f, 41.8f };
    // LEVEL2 : vPlayerPosition = { 57.93f, 0.1f, 37.2f };
    // LEVEL3 : vPlayerPosition = { 52.08f, 0.1f, 57.f };
    if (m_vecPartUIs[0]->IsMouseOn() && m_pGameInstance->Get_RealDIMouseStateDown(MOUSEKEYSTATE::DIMK_LBUTTON)) {
        m_pGameInstance->Load_SubLevel(LEVEL1, { 11.11f, 0.1f, 41.8f });
        m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_UI"), this);
        m_pGameInstance->Show_Cursor(false);
    }
    if (m_vecPartUIs[1]->IsMouseOn() && m_pGameInstance->Get_RealDIMouseStateDown(MOUSEKEYSTATE::DIMK_LBUTTON)) {
        m_pGameInstance->Load_SubLevel(LEVEL2, { 57.93f, 0.1f, 37.2f });
        m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_UI"), this);
        m_pGameInstance->Show_Cursor(false);
    }
    if (m_vecPartUIs[2]->IsMouseOn() && m_pGameInstance->Get_RealDIMouseStateDown(MOUSEKEYSTATE::DIMK_LBUTTON)) {
        m_pGameInstance->Load_SubLevel(LEVEL_BOSS, { 52.08f, 0.1f, 57.f });
        m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_UI"), this);
        m_pGameInstance->Show_Cursor(false);
    }
    if (m_pGameInstance->Get_RealDIKeyStateDown(DIK_E) || m_pGameInstance->Get_RealDIKeyStateDown(DIK_ESCAPE)) {
        m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_UI"), this);
        m_pGameInstance->Show_Cursor(false);
    }
}

void CElevatorUI::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CElevatorUI::Render()
{
    //return __super::Render();
    return S_OK;
}

HRESULT CElevatorUI::Ready_PartUI()
{
    m_vecPartUIs.resize(L_END);

    _float fSizeX = 120.f;
    _float fSizeY = 40.f;

    CButton::BUTTON_DESC desc1 = {};
    desc1.fX = m_fX;
    desc1.fY = m_fY - 70; desc1.fSizeX = fSizeX; desc1.fSizeY = fSizeY;
    desc1.pParent = this; desc1.fRotationPerSec = 10.f; desc1.fSpeedPerSec = 10.f;
    desc1.strQuote = TEXT("LEVEL 1");
    if (FAILED(Add_PartUI(0, TEXT("Prototype_GameObject_UI_Button"), &desc1)))
        return E_FAIL;
    CButton::BUTTON_DESC desc2 = {};
    desc2.fX = m_fX;
    desc2.fY = m_fY + 0; desc2.fSizeX = fSizeX; desc2.fSizeY = fSizeY;
    desc2.pParent = this; desc2.fRotationPerSec = 10.f; desc2.fSpeedPerSec = 10.f;
    desc2.strQuote = TEXT("LEVEL 2");
    if (FAILED(Add_PartUI(1, TEXT("Prototype_GameObject_UI_Button"), &desc2)))
        return E_FAIL;

    CButton::BUTTON_DESC desc3 = {};
    desc3.fX = m_fX;
    desc3.fY = m_fY + 70; desc3.fSizeX = fSizeX; desc3.fSizeY = fSizeY;
    desc3.pParent = this; desc3.fRotationPerSec = 10.f; desc3.fSpeedPerSec = 10.f;
    desc3.strQuote = TEXT("LEVEL 3");
    if (FAILED(Add_PartUI(2, TEXT("Prototype_GameObject_UI_Button"), &desc3)))
        return E_FAIL;
    return S_OK;
}

HRESULT CElevatorUI::Ready_Components()
{
    return S_OK;
}

CElevatorUI* CElevatorUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CElevatorUI* pInstance = new CElevatorUI(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CElevatorUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CElevatorUI::Clone(void* pArg)
{
    CElevatorUI* pInstance = new CElevatorUI(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CElevatorUI"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CElevatorUI::Free()
{
    __super::Free();

}
