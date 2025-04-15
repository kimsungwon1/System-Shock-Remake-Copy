#include "stdafx.h"
#include "FPSPlayer.h"

#include "Grenade.h"

#include "UnitObject.h"

#include "Navigation.h"
#include "Physics.h"

#include "SoundMgr.h"

#include "GameInstance.h"

CGrenade::CGrenade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAmmunition(pDevice, pContext)
{
}

CGrenade::CGrenade(const CGrenade& Prototype)
    : CAmmunition(Prototype)
{
}

HRESULT CGrenade::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CGrenade::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;
    auto* pDesc = static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Grenade"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)))) {
        return E_FAIL;
    }

    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

    aabbDesc.vExtents = { 0.2f, 0.2f, 0.2f };
    aabbDesc.vCenter = { aabbDesc.vExtents.x, aabbDesc.vExtents.y, aabbDesc.vExtents.z };
    aabbDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbDesc))) {
        return E_FAIL;
    }

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation1"), TEXT("Com_Navigation1"), reinterpret_cast<CComponent**>(&m_pNavigationComs[LEVEL1])))) {
        return E_FAIL;
    }
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation2"), TEXT("Com_Navigation2"), reinterpret_cast<CComponent**>(&m_pNavigationComs[LEVEL2])))) {
        return E_FAIL;
    }
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation3"), TEXT("Com_Navigation3"), reinterpret_cast<CComponent**>(&m_pNavigationComs[LEVEL_BOSS])))) {
        return E_FAIL;
    }
    m_iCurrentLevel = pDesc->iCurrentLevel;
    m_pNavigationComs[m_iCurrentLevel]->SetCurrentIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pColliderCom);

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physics"), TEXT("Com_Physics"), reinterpret_cast<CComponent**>(&m_pPhysicsCom)))) {
        return E_FAIL;
    }

    auto* desc = static_cast<Grenade_DESC*>(pArg);
    m_pPhysicsCom->ReceivePowerVector(XMLoadFloat3(&desc->vThrowdir));

    m_eAmmunitionType = Ammunition_Type::TYPE_GRENADE;

    return S_OK;
}

void CGrenade::Priority_Update(_float fTimeDelta)
{
}

void CGrenade::Update(_float fTimeDelta)
{
    if(m_bShot)
    {
        if (m_pPhysicsCom->IsCollided()) {
            // 너무 세게 부딫치면 데미지

            m_pPhysicsCom->PopPower(4);
        }
        //__super::Update(fTimeDelta);
        m_pTransformCom->Bind_Physics(fTimeDelta, m_pPhysicsCom, m_pNavigationComs[m_iCurrentLevel],
            nullptr);

        if (m_vBlowTime.x >= m_vBlowTime.y) {
            Blow(fTimeDelta);
        }
        m_vBlowTime.x += fTimeDelta;
    }
}

void CGrenade::Late_Update(_float fTimeDelta)
{
    //__super::Late_Update(fTimeDelta);
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CGrenade::Render()
{
    return __super::Render();
}

void CGrenade::Blow(_float fTimeDelta)
{
    auto vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    auto* pEnemyList = m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"));
    auto* pPlayerList = m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

    for (auto* pEnemy : *pEnemyList) {
        auto* dynamy = dynamic_cast<CUnitObject*>(pEnemy);
        _float fDistance = XMVectorGetX(XMVector3Length(vPosition - dynamy->GetTransformCom()->Get_State(CTransform::STATE_POSITION)));

        if (dynamy->IsDead() || fDistance > m_fBlowRange || dynamy == m_pShooter) {
            continue;
        }
        _float fDamage = powf(m_fBlowRange - fDistance, 2.f);
        dynamy->Set_Damage((_int)fDamage);
    }
    for (auto* pPlayer : *pPlayerList) {
        auto* dynamy = dynamic_cast<CFPSPlayer*>(pPlayer);
        _float fDistance = XMVectorGetX(XMVector3Length(vPosition - dynamy->GetTransformCom()->Get_State(CTransform::STATE_POSITION)));

        if (dynamy->IsDead() || fDistance > m_fBlowRange || dynamy == m_pShooter) {
            continue;
        }
        _float fDamage = powf(m_fBlowRange - fDistance, 2.f);
        dynamy->ReceiveDamage((_int)fDamage);
    }

    auto* pSound = CSoundMgr::Get_Instance();
    _float fRand = m_pGameInstance->Get_Random(0.f, 5.f);

    if (fRand < 1.f) {
        pSound->PlaySound(TEXT("Bomb_Explosion_Main_01"), 1.f);
    }
    else if (fRand < 2.f) {
        pSound->PlaySound(TEXT("Bomb_Explosion_Main_02"), 1.f);
    }
    else if (fRand < 3.f) {
        pSound->PlaySound(TEXT("Bomb_Explosion_Main_03"), 1.f);
    }
    else if (fRand < 4.f) {
        pSound->PlaySound(TEXT("Bomb_Explosion_Main_04"), 1.f);
    }
    else {
        pSound->PlaySound(TEXT("Bomb_Explosion_Main_05"), 1.f);
    }

    // 이펙트 생성
    CGameObject::GAMEOBJECT_DESC desc = {};
    desc.fRotationPerSec = 10.f; desc.fSpeedPerSec = 10.f;
    XMStoreFloat3((_float3*)desc.transMat.m[3], vPosition + m_pTransformCom->Get_State(CTransform::STATE_UP));

    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BombBlow"), &desc);
    m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_FlyingBullet"), this);
}

CGrenade* CGrenade::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGrenade* pInstance = new CGrenade(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Initialize : Create CGrenade"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CGrenade::Clone(void* pArg)
{
    CGrenade* pInstance = new CGrenade(*this);

    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX(TEXT("Failed to Initialize : Clone CGrenade"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGrenade::Free()
{
    __super::Free();

    for (auto* pNavi : m_pNavigationComs) {
        Safe_Release(pNavi);
    }
    Safe_Release(m_pPhysicsCom);
}
