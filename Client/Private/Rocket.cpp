#include "stdafx.h"
#include "FPSPlayer.h"

#include "Rocket.h"

#include "Effect_Blast.h"

#include "TileObject.h"

#include "SoundMgr.h"

#include "UnitObject.h"
#include "GameInstance.h"

CRocket::CRocket(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAmmunition(pDevice, pContext)
{
}

CRocket::CRocket(const CRocket& Prototype)
    : CAmmunition(Prototype)
{
}

HRESULT CRocket::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRocket::Initialize(void* pArg)
{
    m_iDamage = 10;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rocket"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)))) {
        return E_FAIL;
    }

    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

    aabbDesc.vExtents = { 0.2f, 0.2f, 0.2f };
    aabbDesc.vCenter = { 0.f,0.f,0.f };
    aabbDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbDesc))) {
        return E_FAIL;
    }

    m_eAmmunitionType = Ammunition_Type::TYPE_ROCKET;

    CEffect_Blast::BLAST_DESC desc = {};
    desc.fRotationPerSec = 10.f; desc.fSpeedPerSec = 10.f; desc.fScale = 0.3f;
    desc.pOwner = this;
    XMStoreFloat3((_float3*)desc.transMat.m[3], m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_LOOK));

    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Blast"), &desc);

    m_pBlastEffect = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Effect"));

    return S_OK;
}

void CRocket::Priority_Update(_float fTimeDelta)
{
}

void CRocket::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CRocket::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CRocket::Render()
{
    return __super::Render();
}

void CRocket::blow(_float fTimeDelta)
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
        _float fDamage = abs(m_fBlowRange - fDistance);
        dynamy->Set_Damage((_int)fDamage);
    }
    for (auto* pPlayer : *pPlayerList) {
        auto* dynamy = dynamic_cast<CFPSPlayer*>(pPlayer);
        _float fDistance = XMVectorGetX(XMVector3Length(vPosition - dynamy->GetTransformCom()->Get_State(CTransform::STATE_POSITION)));

        if (dynamy->IsDead() || fDistance > m_fBlowRange) {
            continue;
        }
        _float fDamage = abs(m_fBlowRange - fDistance);
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
    // ÀÌÆåÆ® »ý¼º
    CGameObject::GAMEOBJECT_DESC desc = {};
    desc.fRotationPerSec = 10.f; desc.fSpeedPerSec = 10.f; 
    XMStoreFloat3((_float3*)desc.transMat.m[3], vPosition + m_pTransformCom->Get_State(CTransform::STATE_UP));

    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BombBlow"), &desc);
}

_bool CRocket::IsCollideAndHurt(_float fTimeDelta)
{
    auto* pEnemyList = m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"));
    auto* pPlayerList = m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
    auto pTileList = m_pGameInstance->GetList_AtOctree_ByRadius(LEVEL_GAMEPLAY, TEXT("Layer_Tile"), m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);

    for (auto* pEnemy : *pEnemyList) {
        auto* dynamy = dynamic_cast<CUnitObject*>(pEnemy);
        if (!dynamy->IsDead() && dynamy->Get_Collider()->Intersect(m_pColliderCom) && dynamy != m_pShooter) {
            // dynamy- damaged
            blow(fTimeDelta);
            dynamy->Set_Damage(m_iDamage);
            return true;
        }
    }
    for (auto* pPlayer : *pPlayerList) {
        auto* dynamy = dynamic_cast<CFPSPlayer*>(pPlayer);
        if (!dynamy->IsDead() && dynamy->Get_Collider()->Intersect(m_pColliderCom) && dynamy != m_pShooter) {
            // dynamy- damaged
            blow(fTimeDelta);
            dynamy->ReceiveDamage(m_iDamage);
            return true;
        }
    }

    for (auto* pTile : pTileList) {
        auto* dynamy = dynamic_cast<CTileObject*>(pTile);

        _vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * m_pColliderCom->Get_Extents()->z;

        _float distance;
        if (dynamy->LineIntersects(m_pTransformCom->Get_State(CTransform::STATE_POSITION), vLook, &distance) && distance <= 1.f) {
            blow(fTimeDelta);
            return true;
        }

    }

    return false;
}

CRocket* CRocket::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRocket* pInstance = new CRocket(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Initialize : Create CRocket"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRocket::Clone(void* pArg)
{
    CRocket* pInstance = new CRocket(*this);

    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX(TEXT("Failed to Initialize : Clone CRocket"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRocket::Free()
{
    m_pGameInstance->Delete_CloneObject_ByPointer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), m_pBlastEffect);

    __super::Free();


}
