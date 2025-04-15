#include "stdafx.h"
#include "Player.h"
#include "Body_Player.h"
#include "Weapon.h"
#include "Navigation.h"

CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainObject{ pDevice, pContext }
{
}

CPlayer::CPlayer(const CPlayer& Prototype)
    : CContainObject { Prototype }
{
}

HRESULT CPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
    CGameObject::GAMEOBJECT_DESC Desc{};

    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(180.f);

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    CNavigation::NAVIGATION_DESC			NaviDesc{};

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"),
        TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    if (FAILED(Ready_PartObjects())) {
        return E_FAIL;
    }

    m_iState = STATE_IDLE;

    return S_OK;
}

void CPlayer::Priority_Update(_float fTimeDelta)
{
    for (auto& pPartObject : m_vecParts)
        pPartObject->Priority_Update(fTimeDelta);
}

void CPlayer::Update(_float fTimeDelta)
{
    for (auto& pPartObject : m_vecParts)
        pPartObject->Update(fTimeDelta);
}

void CPlayer::Late_Update(_float fTimeDelta)
{
    for (auto& pPartObject : m_vecParts)
        pPartObject->Late_Update(fTimeDelta);
}

HRESULT CPlayer::Render()
{
    return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
    m_vecParts.resize(PART_END, nullptr);

    CBody_Player::Body_Player_DESC desc = {};
    desc.pParentTransform = m_pTransformCom->Get_WorldMatrix_Ptr();
    desc.iParentState = &m_iState;
    Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_Body_Player"), &desc);

    CWeapon::Weapon_DESC wDesc = {};
    wDesc.pParentTransform = m_pTransformCom->Get_WorldMatrix_Ptr();
    wDesc.pSocketTransform = dynamic_cast<CBody_Player*>(m_vecParts[PART_BODY])->Get_BoneMatrix_Ptr("SWORD");
    Add_PartObject(PART_WEAPON, TEXT("Prototype_GameObject_Weapon"), &wDesc);

    return S_OK;
}

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPlayer* pInstance = new CPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Created : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}



CGameObject* CPlayer::Clone(void* pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
    __super::Free();
    Safe_Release(m_pNavigationCom);
}
