#include "stdafx.h"

#include "Ammunition.h"
#include "UnitObject.h"
#include "FPSPlayer.h"
#include "TileObject.h"

#include "SoundMgr.h"

#include "GameInstance.h"

CAmmunition::CAmmunition(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CItem(pDevice, pContext)
{
}

CAmmunition::CAmmunition(const CAmmunition& Prototype)
    : CItem(Prototype)
{
}

HRESULT CAmmunition::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAmmunition::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    auto* desc = static_cast<Ammunition_Desc*>(pArg);

    m_bShot = desc->bShot;
    m_pShooter = desc->pShooter;
    m_iDamage = desc->iDamage;

    if(desc->bTargetSet)
    {
        _vector vLook = XMLoadFloat3(&desc->vTargetPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        m_pTransformCom->SetLookToRightUp(vLook);
    }
    else {
        m_pTransformCom->SetLookToRightUp(XMLoadFloat3(&desc->vTargetPos));
    }

    return S_OK;
}

void CAmmunition::Priority_Update(_float fTimeDelta)
{
}

void CAmmunition::Update(_float fTimeDelta)
{
    m_vBlowTime.x += fTimeDelta;
    if(m_bShot)
    {
        m_pTransformCom->Go_Straight(fTimeDelta);
        m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
    }
}

void CAmmunition::Late_Update(_float fTimeDelta)
{
    _bool bCollided = IsCollideAndHurt(fTimeDelta);
    if (m_bShot && bCollided || m_bShot && m_vBlowTime.x >= m_vBlowTime.y) {
        m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_FlyingBullet"), this);
    }
    
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
    
}

HRESULT CAmmunition::Render()
{
    return __super::Render();
}

_bool CAmmunition::IsCollideAndHurt(_float fTimeDelta)
{
    auto* pEnemyList = m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"));
    auto* pPlayerList = m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Player"));
    
    auto* pTileList = m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Tile"));
    
    auto pTileOctreed = m_pGameInstance->GetList_AtOctree_ByRadius(LEVEL_GAMEPLAY, TEXT("Layer_Tile"), m_pTransformCom->Get_State(CTransform::STATE_POSITION), 2.f);


    for (auto* pEnemy : *pEnemyList) {
        _bool headHit = false;
        _bool torsoHit = false;
        auto* dynamy = dynamic_cast<CUnitObject*>(pEnemy);
        _float fSpeed = m_pTransformCom->GetSpeedPerSec();
        _vector vToBehind = -m_pTransformCom->Get_State(CTransform::STATE_LOOK);
        _vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

        CCollider* headColl = dynamy->Get_HeadCollider();
        CCollider* torsoColl = dynamy->Get_Collider();

        if (dynamy->IsDead() || dynamy == m_pShooter)
            continue;

        if(headColl != nullptr && !dynamy->IsDead())
        {
            headHit = dynamy->Get_HeadCollider()->Intersect(m_pColliderCom);
            if (dynamy->Get_HeadCollider()->Intersect(vPosition, vToBehind, fSpeed)) {
                headHit = true;
            }
        }
        if (headHit) {
            dynamy->Set_Damage((_int)(_float(m_iDamage) * 1.5f));
        }

        torsoHit = torsoColl->Intersect(m_pColliderCom);
        if (torsoColl->Intersect(vPosition, vToBehind, fSpeed * fTimeDelta)) {
            torsoHit = true;
        }

        if (dynamy == m_pShooter || dynamy->IsDead())
            torsoHit = false;

        if (torsoHit) {
            // dynamy- damaged
            dynamy->Set_Damage(m_iDamage);

        }

        if (torsoHit || headHit) {
            CGameObject::GAMEOBJECT_DESC desc = {};
            XMStoreFloat3((_float3*)desc.transMat.m[3], m_pTransformCom->Get_State(CTransform::STATE_POSITION));

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BulletHit"), &desc);
            if(dynamy->GetUnitType() != UNIT_CortexReaver)
            {
                CSoundMgr::Get_Instance()->PlaySound(TEXT("Physics_Impact_Bullet_Shared_Flesh_Bullet_01"), 1.f);
            }
            else {
                CSoundMgr::Get_Instance()->PlaySound(TEXT("Physics_Impact_Bullet_Cortex"), 1.f);
            }

            return true;
        }
    }
    for (auto* pPlayer : *pPlayerList) {
        auto* dynamy = dynamic_cast<CFPSPlayer*>(pPlayer);
        if (!dynamy->IsDead() && dynamy->Get_Collider()->Intersect(m_pColliderCom) && dynamy != m_pShooter) {
            // dynamy- damaged
            dynamy->ReceiveDamage(m_iDamage);
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Physics_Impact_Bullet_Shared_Flesh_Bullet_01"), 1.f);
            return true;
        }
    }
    for (auto* pTile : pTileOctreed) {
        auto* dynamy = dynamic_cast<CTileObject*>(pTile);

        _vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * m_pColliderCom->Get_Extents()->z;

        _float distance;
        if (dynamy->LineIntersects(m_pTransformCom->Get_State(CTransform::STATE_POSITION),
            vLook, &distance) && distance <= 1.f) {
            
            /*CGameObject::GAMEOBJECT_DESC desc = {};
            desc.fRotationPerSec = 10.f; desc.fSpeedPerSec = 1.f;
            XMStoreFloat3((_float3*)desc.transMat.m[3], m_pTransformCom->Get_State(CTransform::STATE_POSITION) + distance * m_pTransformCom->Get_State(CTransform::STATE_LOOK));
            XMStoreFloat3((_float3*)desc.transMat.m[2], dynamy->GetTransformCom()->Get_State(CTransform::STATE_LOOK));

            m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BulletCrater"), &desc);*/
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Physics_Impact_Bullet_Tile"), 1.f);
            return true;
        }
    }
    return false;
}

void CAmmunition::Free()
{
    __super::Free();
}