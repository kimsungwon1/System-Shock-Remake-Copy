#include "stdafx.h"
#include "DefaultMutant.h"
#include "Navigation.h"
#include "Collider.h"
#include "FPSPlayer.h"

#include "SoundMgr.h"

#include "GameInstance.h"

void CDefaultMutant::PlayAttackSound() {
    _float fRand = m_pGameInstance->Get_Random(3.f, 10.f);
    if (fRand < 4.f) {
        CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_04"), 1.0f);
    }
    else if (fRand < 5.f) {
        CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_05"), 1.0f);
    }
    else if (fRand < 6.f) {
        CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_06"), 1.0f);
    }
    else if (fRand < 7.f) {
        CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_07"), 1.0f);
    }
    else if (fRand < 8.f) {
        CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_08"), 1.0f);
    }
    else if (fRand < 9.f) {
        CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_09"), 1.0f);
    }
    else {
        CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_10"), 1.0f);
    }
}

CDefaultMutant::CDefaultMutant(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUnitObject(pDevice, pContext)
{
}

CDefaultMutant::CDefaultMutant(const CUnitObject& Prototype)
    : CUnitObject(Prototype)
{
}

HRESULT CDefaultMutant::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CDefaultMutant::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) {
        return E_FAIL;
    }

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_DefaultMutant"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

    aabbDesc.vExtents = { 0.5f, 0.75f, 0.5f };
    aabbDesc.vCenter = { 0.f, aabbDesc.vExtents.y, 0.f };
    aabbDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbDesc))) {
        return E_FAIL;
    }
    if (FAILED(m_pGameInstance->Add_Collider(m_pColliderCom))) {
        return E_FAIL;
    }

    CBounding_AABB::BOUNDING_AABB_DESC headDesc = {};
    headDesc.vExtents = { 0.15f, 0.17f, 0.15f };
    headDesc.vCenter = { 0.f, 0.f, 0.f };
    headDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider_Head"), reinterpret_cast<CComponent**>(&m_pHeadColliderCom), &headDesc))) {
        return E_FAIL;
    }

    CBounding_OBB::BOUNDING_OBB_DESC leftObbDesc = {};
    leftObbDesc.vExtents = { 0.3f, 0.3f, 0.3f };
    leftObbDesc.vCenter = { 0.f, 0.f, 0.f };
    leftObbDesc.vAngles = { 0.f, 0.f, 0.f };

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_LeftMelee"), reinterpret_cast<CComponent**>(&m_pLeftMeleeColliderCom), &leftObbDesc)))
        return E_FAIL;
    CBounding_OBB::BOUNDING_OBB_DESC rightObbDesc = {};
    rightObbDesc.vExtents = { 0.3f, 0.3f, 0.3f };
    rightObbDesc.vCenter = { 0.f, 0.f, 0.f };
    rightObbDesc.vAngles = { 0.f, 0.f, 0.f };
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider_RightMelee"), reinterpret_cast<CComponent**>(&m_pRightMeleeColliderCom), &rightObbDesc)))
        return E_FAIL;

    m_fDealDistance = 2.f;
    m_pModelCom->SetUp_Animation(Anim_State::Idle, true);

    m_iHP = 8;
    m_eUnitType = UNIT_DefaultMutant;

    return S_OK;
}

void CDefaultMutant::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CDefaultMutant::Update(_float fTimeDelta)
{
    _float3 pIn = {};
    
    if (m_bDead) {
        setDead(fTimeDelta);
        m_fDeadTime -= fTimeDelta;
        if (m_fDeadTime < -1.f) {
            m_fDeadTime = 0.f;
        }
        return;
    }


    switch (m_eActionState) {
    case STATE_STAND:
        setIdle(fTimeDelta);
        break;
    case STATE_TURN:
        turn_To_Player(fTimeDelta);
        break;
    case STATE_WALK:
        move_By_AStar(fTimeDelta);
        break;
    case STATE_ATTACK:
        attack_normal(fTimeDelta);
        break;
    case STATE_ATTACK_CHARGE:
        attack_charge(fTimeDelta);
        break;
    }
    hit_Player(fTimeDelta);
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
    update_TwoHands();
    _float4x4 rootMat = {};
    m_bAnimationFinished = m_pModelCom->Play_Animation(fTimeDelta, &rootMat, true);
}

void CDefaultMutant::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CDefaultMutant::Render()
{
#ifdef _DEBUG
    m_pLeftMeleeColliderCom->Render();
    m_pRightMeleeColliderCom->Render();
#endif // _DEBUG


    __super::Render();

    return S_OK;
}

_bool CDefaultMutant::setIdle(_float fTimeDelta)
{
    m_pModelCom->SetUp_Animation(Anim_State::Idle, true, 0.0f);

    if (!CUnitObject::find_Player(fTimeDelta)) {
        m_eActionState = STATE_STAND;
        find_path();
        m_eActionState = STATE_TURN;
    }

    return true;
}

_bool CDefaultMutant::setDead(_float fTimeDelta)
{
    if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::Death_h)
    {
        m_pModelCom->SetUp_Animation(Anim_State::Death_h, false);
        
        _float fRand = m_pGameInstance->Get_Random(0.f, 4.f);
        if(fRand < 1.f)
        {
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnDeath_01"), 1.f);
        }
        else if (fRand < 2.f)
        {
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnDeath_02"), 1.f);
        }
        else if (fRand < 3.f)
        {
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnDeath_03"), 1.f);
        }
        else if (fRand < 4.f)
        {
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnDeath_04"), 1.f);
        }
    }
    _float4x4 rootTransMat = {};
    
    m_bAnimationFinished = m_pModelCom->Play_Animation(fTimeDelta, &rootTransMat, true);
    return true;
}

_bool CDefaultMutant::turn_To_Player(_float fTimeDelta)
{
    if (m_pTargetObject == nullptr) {
        return false;
    }
    _float fDegree = 0.f;

    _vector UnitDirVec = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
    // 길 뚫려 있으면 길 뚫린 플레이어 방향으로, 길 안 뚫려 있음 다음 길 방향으로 바라봐야 함
    // 나중에 추가하고 적용해야함!!!
    _vector NextDirVec = {};
    _bool isDirectlyLookingToPlayer = false;

    // 네비게이션이 다다랐음
    if (m_pNavigationComs[m_iCurrentLevel]->IsReservedEmpty() || IsDestinationDirectly()) {
        isDirectlyLookingToPlayer = true;
        NextDirVec = m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION)
            - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    }
    // 네비게이션 참
    else {
        NextDirVec = m_pNavigationComs[m_iCurrentLevel]->Get_NextCellPos()
            - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    }
    XMVectorSetY(UnitDirVec, 0.f); XMVectorSetY(NextDirVec, 0.f);
    _float cos = XMVectorGetX(XMVector3Dot(UnitDirVec, NextDirVec)) / (XMVectorGetX((XMVector3Length(UnitDirVec))) * XMVectorGetX(XMVector3Length(NextDirVec)));
    fDegree = XMConvertToDegrees(acosf(cos));

    /*if (180.f <= fDegree && fDegree < 355.f) {
        m_pModelCom->SetUp_Animation(Anim_State::Turn_r_0, false);
    }
    else if (5 < fDegree && fDegree < 180.f) {
        m_pModelCom->SetUp_Animation(Anim_State::Turn_r_0, false);
    }*/
    m_pModelCom->SetUp_Animation(Anim_State::Turn_r_0, false);

    // 아직 각도가 벌어짐
    if ((fDegree < -5.f || 5.f < fDegree) && !isnan(fDegree))
    {
        _vector newVec = XMVectorLerp(UnitDirVec, NextDirVec, m_pTransformCom->GetRotationPerSec() * fTimeDelta / fDegree);
        m_pTransformCom->SetLookToRightUp_ground(newVec);
        return true;
    }
    // 각도가 충분히 좁혀짐
    else {
        //거리마저 좁혀지면 - 네비게이션 비면
        if (isDirectlyLookingToPlayer && XMVectorGetX(XMVector3Length(NextDirVec)) < m_fDealDistance) {
            m_eActionState = STATE_ATTACK;
        }
        else {
            //이제 걸을 시간
            m_eActionState = STATE_WALK;
        }

        return false;
    }
}

_bool CDefaultMutant::hit_Player(_float fTimeDelta)
{
    if (!m_bLeftHitting && !m_bRightHitting) {
        return false;
    }
    
    auto* pTarget = dynamic_cast<CFPSPlayer*>(m_pTargetObject);

    if (pTarget != nullptr) {
        _vector vTargetPosition = m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) +
            m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_UP) * 0.5f;
        _vector vEliteToTarget = vTargetPosition - (m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_UP) * 0.5f);
        _float fDistance = XMVectorGetX(XMVector3Length(vEliteToTarget));
        // elite와 상대 사이 벡터와 elite의 look vector의 각도차
        _float fDegree = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vEliteToTarget, m_pTransformCom->Get_State(CTransform::STATE_LOOK))) / XMVectorGetX(XMVector3Length(vEliteToTarget)) / XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_LOOK)))));
        
        if((m_bLeftHitting || m_bRightHitting) && fDegree < 90.f && fDegree > -90.f && fDistance < 2.0f){
            pTarget->ReceiveDamage(1);
            m_bLeftHitting = false;
            m_bRightHitting = false;
        }
    }
    return true;
}

_bool CDefaultMutant::move_By_AStar(_float fTimeDelta)
{
    _vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector destPos = XMVectorSetW(XMLoadFloat3(&m_vDestPos), 1.f);
    _vector vDir = {};
    _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
    _float fDegree = {};

    // 방향 조절
    if (auto* pTarget = dynamic_cast<CFPSPlayer*>(m_pTargetObject)) {
        if (pTarget->IsMoving()) {
            XMStoreFloat3(&m_vDestPos, m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION));
            m_pNavigationComs[m_iCurrentLevel]->Set_FinalDest(m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION));
            m_pNavigationComs[m_iCurrentLevel]->AStar();
        }
    }
    if (m_pNavigationComs[m_iCurrentLevel]->IsReservedEmpty()) {
        m_eActionState = STATE_TURN;
    }
    else {
        if (IsDestinationDirectly()) {
            //m_pTransformCom->LookAt_ground(destPos);
            vDir = destPos - currentPos;
            fDegree = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDir, vLook))
                / (XMVectorGetX(XMVector3Length(vDir)) * XMVectorGetX(XMVector3Length(vLook)))));
        }
        else {
            //m_pTransformCom->LookAt_ground(m_pNavigationCom->Get_NextCellPos());
            _vector cellPos = m_pNavigationComs[m_iCurrentLevel]->Get_NextCellPos();
            vDir = cellPos - currentPos;
            fDegree = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDir, vLook))
                / (XMVectorGetX(XMVector3Length(vDir)) * XMVectorGetX(XMVector3Length(vLook)))));
        }

        if (fDegree != 0.f && !isnan(fDegree))
        {
            _vector newVec = XMVectorLerp(vLook, vDir, m_pTransformCom->GetRotationPerSec() * fTimeDelta / fDegree);

            m_pTransformCom->SetLookToRightUp_ground(newVec);
        }

    }

    _float cos = XMVectorGetX(XMVector3Dot(destPos - currentPos, vLook)) / (XMVectorGetX((XMVector3Length(destPos - currentPos))) * XMVectorGetX(XMVector3Length(vLook)));
    fDegree = XMConvertToDegrees(acosf(cos));

    // 걷기 실행
    // charge 조건문 추가해라.

    if(XMVectorGetX(XMVector3Length(destPos - currentPos)) <= m_fChargeDistance && !TileBetweenMeAndPlayer() 
        && !ThereIsAllyFront(vLook, m_fChargeDistance)
        && (-5.f < fDegree && fDegree < 5.f)) {
        m_eActionState = STATE_ATTACK_CHARGE;
    }
    // 서로 벽 있는 것도 조건에 추가해야한다.
    else if (XMVectorGetX(XMVector3Length(destPos - currentPos)) > m_fDealDistance || TileBetweenMeAndPlayer())
    {
        m_pModelCom->SetUp_Animation(Anim_State::Charge, true, 1.5f);

        _float3 fromNextToMine = {};

        _vector vMoved = m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &fromNextToMine, m_pColliderCom);

        // 상대 콜라이더에 의해 막혔을 경우.
        if (fromNextToMine.x != 0.f || fromNextToMine.y != 0.f || fromNextToMine.z != 0.f)
        {
            _vector oppositeCenter = XMLoadFloat3(m_pColliderCom->Get_Center()) + vMoved - XMLoadFloat3(&fromNextToMine);

            _float3 realFromNextToMine = {};
            XMStoreFloat3(&realFromNextToMine, XMLoadFloat3(m_pColliderCom->Get_Center()) - oppositeCenter);

            if (realFromNextToMine.x > 0 && realFromNextToMine.z > 0 && realFromNextToMine.z <= realFromNextToMine.x) {
                m_pTransformCom->SetLookToRightUp({ 0.f, 0.f, 1.f });
                m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &fromNextToMine, m_pColliderCom);
            }
            if (realFromNextToMine.x > 0 && realFromNextToMine.z > 0 && realFromNextToMine.z > realFromNextToMine.x) {
                m_pTransformCom->SetLookToRightUp({ -1.f,0.f,0.f });
                m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &fromNextToMine, m_pColliderCom);
            }
            if (realFromNextToMine.x <= 0 && realFromNextToMine.z > 0 && realFromNextToMine.z > abs(realFromNextToMine.x)) {
                m_pTransformCom->SetLookToRightUp({ -1.f,0.f,0.f });
                m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &fromNextToMine, m_pColliderCom);
            }
            if (realFromNextToMine.x <= 0 && realFromNextToMine.z > 0 && realFromNextToMine.z <= abs(realFromNextToMine.x)) {
                m_pTransformCom->SetLookToRightUp({ 0.f, 0.f, -1.f });
                m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &fromNextToMine, m_pColliderCom);
            }
            if (realFromNextToMine.x <= 0 && realFromNextToMine.z <= 0 && realFromNextToMine.z > realFromNextToMine.x) {
                m_pTransformCom->SetLookToRightUp({ 0.f,0.f,-1.f });
                m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &fromNextToMine, m_pColliderCom);
            }
            if (realFromNextToMine.x <= 0 && realFromNextToMine.z <= 0 && realFromNextToMine.z <= realFromNextToMine.x) {
                m_pTransformCom->SetLookToRightUp({ 1.f,0.f,0.f });
                m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &fromNextToMine, m_pColliderCom);
            }
            if (realFromNextToMine.x > 0 && realFromNextToMine.z <= 0 && abs(realFromNextToMine.z) > realFromNextToMine.x) {
                m_pTransformCom->SetLookToRightUp({ 1.f,0.f,0.f });
                m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &fromNextToMine, m_pColliderCom);
            }
            if (realFromNextToMine.x > 0 && realFromNextToMine.z <= 0 && abs(realFromNextToMine.z) <= realFromNextToMine.x) {
                m_pTransformCom->SetLookToRightUp({ 0.f,0.f,1.f });
                m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &fromNextToMine, m_pColliderCom);
            }
        }
    }
    // 서로 벽도 없고 거리도 충분함
    else {
        m_pNavigationComs[m_iCurrentLevel]->Initialize_AStar();
        m_eActionState = STATE_ATTACK;

    }

    return _bool();
}

_bool CDefaultMutant::attack_normal(_float fTimeDelta)
{
    _vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector destPos = m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION);
    _vector vDir = destPos - currentPos;
    _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
    _float fDegree = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDir, vLook))
        / (XMVectorGetX(XMVector3Length(vDir)) * XMVectorGetX(XMVector3Length(vLook)))));

    auto* pPlayer = dynamic_cast<CFPSPlayer*>(m_pTargetObject);
    // 플레이어가 가만히 있을때
    // 이때도 플레이어와 몬스터 각도가 안 맞으면 턴 해야됨.
    if (!pPlayer->IsMoving()) {
        if (XMVectorGetX(XMVector3Length(vDir)) >= m_fDealDistance || TileBetweenMeAndPlayer()) {
            // 새로 길찾기

            find_path();
            m_eActionState = STATE_TURN;
            return true;
        }
        // 각도 안맞아 턴.
        if ((fDegree < -5.f || 5.f < fDegree) && !isnan(fDegree)) {
            _vector newVec = XMVectorLerp(vLook, vDir, m_pTransformCom->GetRotationPerSec() * fTimeDelta / fDegree);
            m_pTransformCom->SetLookToRightUp_ground(newVec);
        }

        // 각도 안맞아도 때리긴 때린다.
        attack_hit(fTimeDelta);

    }

    // 플레이어가 움직여 턴 해야할때
    else {
        if (fDegree != 0.f && !isnan(fDegree))
        {
            _vector newVec = XMVectorLerp(vLook, vDir, m_pTransformCom->GetRotationPerSec() * fTimeDelta / fDegree);
            m_pTransformCom->SetLookToRightUp_ground(newVec);
        }

        // 플레이어가 사정거리 밖으로 나가거나 벽 뒤로 갈때
        // 바로 턴으로 돌리지 말고 공격 끝났다가 해라
        if (XMVectorGetX(XMVector3Length(vDir)) >= m_fDealDistance || TileBetweenMeAndPlayer()) {
            // 새로 길찾기

            find_path();
            m_eActionState = STATE_TURN;
        }
        else {
            attack_hit(fTimeDelta);
        }
    }

    return _bool();
}

_bool CDefaultMutant::attack_charge(_float fTimeDelta)
{
    if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::Charge_attack) {
        m_pModelCom->SetUp_Animation(Charge_attack, false, 0.3f);
        m_bLeftHitting = true;
        _float fRand = m_pGameInstance->Get_Random(0.f, 10.f);
        if (fRand < 1.f) {
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_01"), 1.f);
        }
        else if (fRand < 2.f) {
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_02"), 1.0f);
        }
        else if (fRand < 3.f) {
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_03"), 1.0f);
        }
        else if (fRand < 4.f) {
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_04"), 1.0f);
        }
        else if (fRand < 5.f) {
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_05"), 1.0f);
        }
        else if (fRand < 6.f) {
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_06"), 1.0f);
        }
        else if (fRand < 7.f) {
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_07"), 1.0f);
        }
        else if (fRand < 8.f) {
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_08"), 1.0f);
        }
        else if (fRand < 9.f) {
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_09"), 1.0f);
        }
        else {
            CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_HumanoidMutant_Vox_OnMeleeAttack_10"), 1.0f);
        }
    }
    else if (m_pModelCom->GetCurrentAnimIndex() == Anim_State::Charge_attack && !m_bAnimationFinished)
    {
        _float3 moved = {};
        m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &moved, m_pColliderCom);
    }
    else if (m_pModelCom->GetCurrentAnimIndex() == Anim_State::Charge_attack && m_bAnimationFinished)
    {
        m_bLeftHitting = false;
        //공격한 다음, 가까우면 근거리 공격
        if (XMVectorGetX(XMVector3Length(m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION))) < m_fDealDistance)
        {
            m_eActionState = STATE_ATTACK;
        }
        else
        {
            find_path();
            m_eActionState = STATE_TURN;
        }
    }
    return false;
}

_bool CDefaultMutant::attack_hit(_float fTimeDelta)
{
    if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::Attack_a 
        && m_pModelCom->GetCurrentAnimIndex() != Anim_State::Attack_b) {
        m_pModelCom->SetUp_Animation(Anim_State::Attack_a, 0.5f);
        m_bLeftHitting = true;

        PlayAttackSound();
    }
    else if (m_pModelCom->GetCurrentAnimIndex() == Anim_State::Attack_a 
        && m_bAnimationFinished) {
        m_pModelCom->SetUp_Animation(Anim_State::Attack_b, false, 0.05f);
        m_bLeftHitting = false;
        m_bRightHitting = true;

        PlayAttackSound();
    }
    else if (m_pModelCom->GetCurrentAnimIndex() == Anim_State::Attack_b
        && m_pModelCom->IsAnimFinished()) {
        m_bLeftHitting = true;
        m_bRightHitting = false;
        m_pModelCom->SetUp_Animation(Anim_State::Attack_a);

        PlayAttackSound();
    }


    auto* pTarget = dynamic_cast<CFPSPlayer*>(m_pTargetObject);

    if (m_pLeftMeleeColliderCom->Intersect(pTarget->Get_Collider()) && m_bLeftHitting) {
        // 데미지처리
        m_bLeftHitting = false;
        return false;
    }
    if (m_pRightMeleeColliderCom->Intersect(pTarget->Get_Collider()) && m_bRightHitting) {
        // 데미지처리
        m_bRightHitting = false;
        return false;
    }

    return _bool();
}

void CDefaultMutant::update_TwoHands()
{
    auto leftBoneMat = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformMatrix_Ptr("l_wrist"));
    auto rightBoneMat = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformMatrix_Ptr("r_wrist"));
    _float4x4 leftBone4x4 = {};
    _float4x4 rightBone4x4 = {};

    for (size_t i = 0; i < 3; i++) {
        leftBoneMat.r[i] = XMVector3Normalize(leftBoneMat.r[i]);
    }
    for (size_t i = 0; i < 3; i++) {
        rightBoneMat.r[i] = XMVector3Normalize(rightBoneMat.r[i]);
    }

    XMStoreFloat4x4(&leftBone4x4, leftBoneMat * m_pTransformCom->GetWorldMat_XMMat());
    XMStoreFloat4x4(&rightBone4x4, rightBoneMat * m_pTransformCom->GetWorldMat_XMMat());
    m_pLeftMeleeColliderCom->Update(&leftBone4x4);
    m_pRightMeleeColliderCom->Update(&rightBone4x4);
}

CDefaultMutant* CDefaultMutant::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDefaultMutant* pInstance = new CDefaultMutant(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Created : CDefaultMutant"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDefaultMutant::Clone(void* pArg)
{
    CDefaultMutant* pInstance = new CDefaultMutant(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CDefaultMutant"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDefaultMutant::Free()
{
    __super::Free();
    Safe_Release(m_pLeftMeleeColliderCom);
    Safe_Release(m_pRightMeleeColliderCom);
}
