#include "stdafx.h"

#include "CortexReaver.h"

#include "Navigation.h"

#include "FPSPlayer.h"

#include "Rocket.h"
#include "Grenade.h"
#include "Thunder.h"

#include "Physics.h"

#include "SoundMgr.h"

#include "GameInstance.h"

CCortexReaver::CCortexReaver(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUnitObject{ pDevice, pContext}
{
}

CCortexReaver::CCortexReaver(const CUnitObject& Prototype)
    : CUnitObject { Prototype }
{
}

HRESULT CCortexReaver::Initialize_Prototype()
{
    return S_OK; 
}

HRESULT CCortexReaver::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CortexReaver"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

	aabbDesc.vExtents = { 0.5f, 1.f, 0.5f };
	aabbDesc.vCenter = { 0.f, aabbDesc.vExtents.y, 0.f };
	aabbDesc.pOwner = this;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbDesc))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Collider(m_pColliderCom))) {
		return E_FAIL;
	}

    m_fDealDistance = 25.f;

	m_eActionState = CortexReaver_State::STATE_STAND;

    m_pModelCom->SetUp_Animation(Anim_State::Idle, true);

	m_iHP = 500;

	m_eUnitType = UNIT_CortexReaver;

    return S_OK;
}

void CCortexReaver::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CCortexReaver::Update(_float fTimeDelta)
{
	if (m_bDead) {
		setDead(fTimeDelta);
		m_fDeadTime -= fTimeDelta;
		if (m_fDeadTime < -1.f) {
			m_fDeadTime = 0.f;
		}
		return;
	}

	m_vShootCoolTime.x += fTimeDelta;
    if (m_eActionState == STATE_STAND) {
        setIdle(fTimeDelta);
    }
	if (m_eActionState == STATE_DROP) {
		CinematicDrop(fTimeDelta);
	}
    if (m_eActionState == STATE_WALK) {
        move_By_AStar(fTimeDelta);
    }
	if (m_eActionState == STATE_TURN) {
		turn_To_Player(fTimeDelta);
	}
    if (m_eActionState == STATE_ATTACK_NORMAL_RANGE) {
        attack_normal(fTimeDelta);
    }
	if (m_eActionState == STATE_ATTACK_MELEE) {
		attack_melee(fTimeDelta);
	}
	if (m_eActionState == STATE_ATTACK_EXPLOSIVE) {
		attack_explosive(fTimeDelta);
	}
	if (m_eActionState == STATE_ATTACK_LIGHTNING) {
		attack_lightning(fTimeDelta);
	}
	if (m_eActionState == STATE_ATTACK_LIGHTNING_SHORT) {
		attack_lightning_short(fTimeDelta);
	}
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
	_float4x4 rootMat = {};
	m_bAnimationFinished = m_pModelCom->Play_Animation(fTimeDelta, &rootMat, true);
	m_vGrenadeCoolTime.x += fTimeDelta;
	m_vLightningCoolTime.x += fTimeDelta;
	m_vLightningShortCoolTime.x += fTimeDelta;
}

void CCortexReaver::Late_Update(_float fTimeDelta)
{
	if (m_eActionState == STATE_STAND || (m_eActionState == STATE_DROP 
		&& m_bAnimationFinished)) {
		return;
	}
	
	__super::Late_Update(fTimeDelta);
}

HRESULT CCortexReaver::Render()
{
    __super::Render();

    return S_OK;
}

_bool CCortexReaver::setIdle(_float fTimeDelta)
{
    m_pModelCom->SetUp_Animation(Anim_State::Idle, true, 0.f);

    if (!CUnitObject::find_Player(fTimeDelta)) { //false 리턴이 찾은거임
		m_eActionState = STATE_DROP;
		
		//find_path();
    }

    return true;
}

_bool CCortexReaver::setDead(_float fTimeDelta)
{
    if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::Death)
    {
        m_pModelCom->SetUp_Animation(Anim_State::Death, false);
		CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Death_Foley_1"), 1.f);
		CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Death_Sweet_1"), 1.f);
		CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Death_UI_1"), 1.f);
    }
    _float4x4 rootTransMat = {};

    m_bAnimationFinished = m_pModelCom->Play_Animation(fTimeDelta, &rootTransMat, true);
    return true;
}

_bool CCortexReaver::turn_To_Player(_float fTimeDelta)
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
	_float cos = XMVectorGetX(XMVector3Dot(UnitDirVec, NextDirVec)) / (XMVectorGetX((XMVector3Length(UnitDirVec))) * XMVectorGetX(XMVector3Length(NextDirVec)));
	fDegree = XMConvertToDegrees(acosf(cos));

	if (180.f <= fDegree && fDegree < 355.f) {
		m_pModelCom->SetUp_Animation(Anim_State::Turn_Left, false);
	}
	else if (5 < fDegree && fDegree < 180.f) {
		m_pModelCom->SetUp_Animation(Anim_State::Turn_Right, false);
	}

	// 아직 각도가 벌어짐
	XMVECTOR crossProduct = XMVector3Cross(UnitDirVec, NextDirVec);
	_float fy = XMVectorGetY(crossProduct);

	if ((fDegree < -5.f || 5.f < fDegree) && !isnan(fDegree))
	{
		if (fy > 0) {
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fTimeDelta);
		}
		else {
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), -fTimeDelta);
		}
	}
	/*if ((fDegree < -5.f || 5.f < fDegree) && !isnan(fDegree))
	{
		_vector newVec = XMVectorLerp(UnitDirVec, NextDirVec, m_pTransformCom->GetRotationPerSec() * fTimeDelta / fDegree);
		m_pTransformCom->SetLookToRightUp_ground(newVec);
		return true;
	}*/
	// 각도가 충분히 좁혀짐
	else {
		//거리마저 좁혀지면 - 네비게이션 비면
		if (isDirectlyLookingToPlayer && m_pNavigationComs[m_iCurrentLevel]->IsReservedEmpty()) {
			_float dist = XMVectorGetX(XMVector3Length(m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
			if (m_vLightningShortCoolTime.x >= m_vLightningShortCoolTime.y) {
				m_eActionState = STATE_ATTACK_LIGHTNING_SHORT;
				m_vLightningShortCoolTime.x = 0.f;
			}
			else if (m_vLightningCoolTime.x >= m_vLightningCoolTime.y) {
				m_eActionState = STATE_ATTACK_LIGHTNING;
				m_vLightningCoolTime.x = 0.f;
			}
			else if (m_vGrenadeCoolTime.x >= m_vGrenadeCoolTime.y) {
				m_eActionState = STATE_ATTACK_EXPLOSIVE;
				m_vGrenadeCoolTime.x = 0.f;
			}
			else if (dist <= m_fDealDistance) {
				if (dist <= m_fMeleeDistance) {
					m_eActionState = STATE_ATTACK_MELEE;
				}
				else {
					m_eActionState = STATE_ATTACK_NORMAL_RANGE;
				}
			}
			else {
				m_pNavigationComs[m_iCurrentLevel]->AStar();
				m_eActionState = STATE_WALK;
			}
			//m_eActionState = STATE_ATTACK_EXPLOSIVE;
		}
		else {
			//이제 걸을 시간
			m_eActionState = STATE_WALK;
		}

		return false;
	}
}

_bool CCortexReaver::move_By_AStar(_float fTimeDelta)
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
		m_eActionState = STATE_WALK;
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

	// 걷기 실행
	// 서로 벽 있는 것도 조건에 추가해야한다.
	if (XMVectorGetX(XMVector3Length(destPos - currentPos)) > m_fDealDistance || TileBetweenMeAndPlayer())
	{
		m_pModelCom->SetUp_Animation(Anim_State::Run_Front, true, 1.5f);

		_float3 fromNextToMine = {};

		_vector vMoved = m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &fromNextToMine, m_pColliderCom);

		// 상대 콜라이더에 의해 막혔을 경우.
		if (fromNextToMine.x != 0.f || fromNextToMine.y != 0.f || fromNextToMine.z != 0.f)
		{
			_vector oppositeCenter = XMLoadFloat3(m_pColliderCom->Get_Center()) + vMoved - XMLoadFloat3(&fromNextToMine);

			_float3 realFromNextToMine = {};
			XMStoreFloat3(&realFromNextToMine, XMLoadFloat3(m_pColliderCom->Get_Center()) - oppositeCenter);

			m_pModelCom->SetUp_Animation(Anim_State::Run_Front, true);

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
		// 상대가 치기에 가까이 있음
		if (m_vLightningShortCoolTime.x >= m_vLightningShortCoolTime.y) {
			m_eActionState = STATE_ATTACK_LIGHTNING_SHORT;
			m_vLightningShortCoolTime.x = 0.f;
		}
		else if (m_vLightningCoolTime.x >= m_vLightningCoolTime.y) {
			m_eActionState = STATE_ATTACK_LIGHTNING;
			m_vLightningCoolTime.x = 0.f;
		}
		else if (m_vGrenadeCoolTime.x >= m_vGrenadeCoolTime.y) {
			m_eActionState = STATE_ATTACK_EXPLOSIVE;
			m_vGrenadeCoolTime.x = 0.f;
		}
		if (m_fMeleeDistance > XMVectorGetX(XMVector3Length(destPos - currentPos))) {
			m_eActionState = STATE_ATTACK_MELEE;
		}
		else {
			m_eActionState = STATE_ATTACK_NORMAL_RANGE;
		}
	}

	return _bool();
}

_bool CCortexReaver::shoot_Player(_float fTimeDelta)
{
	m_pModelCom->SetUp_Animation(Anim_State::Idle, true);
	if (m_vShootCoolTime.x >= m_vShootCoolTime.y)
	{
		_float fRand = m_pGameInstance->Get_Random(0.f, 3.f);

		if(fRand < 1.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Rocket_1"), 1.f);
		}
		else if (fRand < 2.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Rocket_2"), 1.f);
		}
		else if (fRand < 3.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Rocket_3"), 1.f);
		}

		CRocket::Ammunition_Desc desc = {};
		desc.bShot = true; desc.fRotationPerSec = 10.f; desc.pShooter = this;
		desc.fSpeedPerSec = 15.f; XMStoreFloat4x4(&desc.transMat, XMMatrixIdentity()); desc.iDamage = 3;

		XMStoreFloat3(&desc.vTargetPos, m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.f, 0.f, 0.f));

		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		position += m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 2.f + m_pTransformCom->Get_State(CTransform::STATE_UP) * 1.5f;
		XMStoreFloat3((_float3*)(&desc.transMat.m[3]), position);

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FlyingBullet"), TEXT("Prototype_GameObject_Rocket"), &desc))) {
			int a = 10;
		}

		m_vShootCoolTime.x = 0.f;
	}
	return _bool();
}

void CCortexReaver::calculate_Grenade_Shooting()
{
	_float fRand = m_pGameInstance->Get_Random(0.f, 4.f);
	if (fRand < 1.f) {
		CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Grenade_1"), 1.f);
	}
	else if (fRand < 2.f) {
		CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Grenade_2"), 1.f);
	}
	else if (fRand < 3.f) {
		CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Grenade_3"), 1.f);
	}
	else {
		CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Grenade_4"), 1.f);
	}

	_vector vDir = m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - (m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_UP) * 1.5f);
	_float distance = XMVectorGetX(XMVector3Length(vDir));

	_float v = 25.f;
	_float height = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_UP))) * 1.5f;
	_float minDiffer = 100000.f;
	_float ResultRadian = {};
	
	for (_float radianY = 0.f; radianY <= 10.f; radianY += 2.f) {
		for (_float radianX = -10.f; radianX <= 10.f; radianX += 2.f) {
			CGrenade::Grenade_DESC desc = {};
			desc.bShot = true; desc.fSpeedPerSec = v; desc.iCurrentLevel = m_iCurrentLevel;
			desc.fRotationPerSec = 1.f; desc.pShooter = this;
			XMStoreFloat4x4(&desc.transMat, XMMatrixIdentity());

			_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			position += m_pTransformCom->Get_State(CTransform::STATE_UP) * 1.5f;
			XMStoreFloat3((_float3*)(&desc.transMat.m[3]), position);

			XMStoreFloat3(&desc.vTargetPos, m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION));

			XMStoreFloat3(&desc.vThrowdir, XMVector3Normalize(XMVector3TransformNormal(vDir, XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(- radianX))
				* XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(-radianY)))) * v);

			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FlyingBullet"), TEXT("Prototype_GameObject_Grenade"), &desc))) {
				int a = 10;
			}
		}
	}
	
}

_bool CCortexReaver::attack_normal(_float fTimeDelta)
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
		// 각도 안맞아 턴.
		if ((fDegree < -5.f || 5.f < fDegree) && !isnan(fDegree)) {
			_vector newVec = XMVectorLerp(vLook, vDir, m_pTransformCom->GetRotationPerSec() * fTimeDelta / fDegree);
			m_pTransformCom->SetLookToRightUp_ground(newVec);
		}
		else if (m_vLightningCoolTime.x >= m_vLightningCoolTime.y) {
			m_eActionState = STATE_ATTACK_LIGHTNING;
			m_vLightningCoolTime.x = 0.f;
		}
		else if (m_vGrenadeCoolTime.x >= m_vGrenadeCoolTime.y) {
			m_eActionState = STATE_ATTACK_EXPLOSIVE;
			m_vGrenadeCoolTime.x = 0.f;
		}
		else if (m_vLightningShortCoolTime.x >= m_vLightningShortCoolTime.y) {
			m_eActionState = STATE_ATTACK_LIGHTNING_SHORT;
			m_vLightningShortCoolTime.x = 0.f;
		}
		
		// 각도 안맞아도 공격하긴 한다.
		
		

		else if (XMVectorGetX(XMVector3Length(vDir)) < m_fMeleeDistance) {
			m_eActionState = CortexReaver_State::STATE_ATTACK_MELEE;
		}
		else {
			m_eActionState = STATE_ATTACK_NORMAL_RANGE;
			shoot_Player(fTimeDelta);
		}
	}

	// 플레이어가 움직여 턴 해야할때
	else {
		// 각도 안맞아도 공격하긴 한다.
		
		if (m_vLightningCoolTime.x >= m_vLightningCoolTime.y) {
			m_eActionState = STATE_ATTACK_LIGHTNING;
			m_vLightningCoolTime.x = 0.f;
		}
		else if (m_vGrenadeCoolTime.x >= m_vGrenadeCoolTime.y) {
			m_eActionState = STATE_ATTACK_EXPLOSIVE;
			m_vGrenadeCoolTime.x = 0.f;
		}
		else if (m_vLightningShortCoolTime.x >= m_vLightningShortCoolTime.y) {
			m_eActionState = STATE_ATTACK_LIGHTNING_SHORT;
			m_vLightningShortCoolTime.x = 0.f;
		}
		else if (XMVectorGetX(XMVector3Length(vDir)) < m_fMeleeDistance) {
			m_eActionState = CortexReaver_State::STATE_ATTACK_MELEE;
		}
		else if (fDegree != 0.f && !isnan(fDegree))
		{
			_vector newVec = XMVectorLerp(vLook, vDir, m_pTransformCom->GetRotationPerSec() * fTimeDelta / fDegree);
			m_pTransformCom->SetLookToRightUp_ground(newVec);
		}

		// 플레이어가 사정거리 밖으로 나가거나 벽 뒤로 갈때
		// 바로 턴으로 돌리지 말고 공격 끝났다가 해라
		if (XMVectorGetX(XMVector3Length(vDir)) >= m_fDealDistance || TileBetweenMeAndPlayer()) {
			// 새로 길찾기
			find_path();
			m_eActionState = STATE_WALK;
		}
		else {
			shoot_Player(fTimeDelta);
		}
	}

	return _bool();
}

void CCortexReaver::attack_explosive(_float fTimeDelta)
{
	if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::explosive_attack_fire) {
		m_pModelCom->SetUp_Animation(explosive_attack_fire, false, 3.f);
	}
	else if (m_pModelCom->GetCurrentAnimIndex() == explosive_attack_fire && !m_bAnimationFinished && !m_GrenadeFired) {
		//m_pModelCom->SetUp_Animation(explosive_attack_fire, false, 0.3f);
		calculate_Grenade_Shooting();
		m_GrenadeFired = true;
	}
	else if (m_pModelCom->GetCurrentAnimIndex() == explosive_attack_fire && m_bAnimationFinished) {
		m_eActionState = STATE_TURN;
		m_GrenadeFired = false;
	}
}

void CCortexReaver::attack_melee(_float fTimeDelta)
{
	if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::Melee_Attack) {
		m_pModelCom->SetUp_Animation(Melee_Attack, false, 0.3f);
		CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Melee_A_Foley_1"), 1.f);
	}
	else if (m_pModelCom->GetCurrentAnimIndex() == Anim_State::Melee_Attack && !m_bAnimationFinished)
	{
		_float3 moved = {};
		m_pTransformCom->Walk(0, fTimeDelta * 0.5f, m_pNavigationComs[m_iCurrentLevel], &moved, m_pColliderCom);
	}
	else if (m_pModelCom->GetCurrentAnimIndex() == Anim_State::Melee_Attack && m_bAnimationFinished)
	{
		//공격한 다음, 가까우면 근거리 공격
		// 각도 안맞아도 공격하긴 한다.
		if (m_vLightningCoolTime.x >= m_vLightningCoolTime.y) {
			m_eActionState = STATE_ATTACK_LIGHTNING;
			m_vLightningCoolTime.x = 0.f;
		}
		else if (m_vGrenadeCoolTime.x >= m_vGrenadeCoolTime.y) {
			m_eActionState = STATE_ATTACK_EXPLOSIVE;
			m_vGrenadeCoolTime.x = 0.f;
		}
		else if (m_vLightningShortCoolTime.x >= m_vLightningShortCoolTime.y) {
			m_eActionState = STATE_ATTACK_LIGHTNING_SHORT;
			m_vLightningShortCoolTime.x = 0.f;
		}
		
		else if (XMVectorGetX(XMVector3Length(m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION))) < m_fDealDistance)
		{
			//if (XMVectorGetX(XMVector3Length(m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION))) < m_fMeleeDistance)
			//{
			//	m_eActionState = STATE_TURN;//STATE_ATTACK_MELEE;
			//	//m_pModelCom->SetUp_Animation_Immediately(Melee_Attack, false, 0.3f);
			//}
			//else {
			//	m_eActionState = STATE_ATTACK_NORMAL_RANGE;
			//}
			m_eActionState = STATE_TURN;
		}
		else
		{
			find_path();
			m_eActionState = STATE_WALK;
		}
	}
}

void CCortexReaver::attack_lightning(_float fTimeDelta)
{
	_vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector destPos = m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION);
	_vector vDir = destPos - currentPos;
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float fDegree = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDir, vLook))
		/ (XMVectorGetX(XMVector3Length(vDir)) * XMVectorGetX(XMVector3Length(vLook)))));
	
	XMVECTOR crossProduct = XMVector3Cross(vLook, vDir);
	_float fy = XMVectorGetY(crossProduct);

	if ((fDegree < -5.f || 5.f < fDegree) && !isnan(fDegree))
	{
		if (fy > 0) {
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fTimeDelta * 0.25f);
		}
		else {
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), -fTimeDelta * 0.25f);
		}
	}
	//if ((fDegree < -5.f || 5.f < fDegree) && !isnan(fDegree)) {
	//	/*_vector newVec = XMVectorLerp(vLook, vDir, m_pTransformCom->GetRotationPerSec() * fTimeDelta * 0.1f / fDegree);
	//	m_pTransformCom->SetLookToRightUp_ground(newVec);*/
	//	_float fCrossed = XMVectorGetX(vLook) * XMVectorGetZ(vDir) - XMVectorGetZ(vLook) * XMVectorGetX(vDir);
	//	if (fCrossed > 0)
	//		fDegree = -fDegree;
	//	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fTimeDelta * fDegree / abs(fDegree) / 50 * XMConvertToRadians(35.f));
	//}
	auto* refThun = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Thunder"));
	if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::explosive_attack_loop && refThun == nullptr) {
		m_pModelCom->SetUp_Animation(explosive_attack_loop, true, 2.f);
		_float fRand = m_pGameInstance->Get_Random(0.f, 4.f);

		if (fRand < 1.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Elec_1"), 1.f);
		}
		else if (fRand < 2.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Elec_2"), 1.f);
		}
		else if (fRand < 3.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Elec_3"), 1.f);
		}
		else if (fRand < 4.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Elec_4"), 1.f);
		}
		
		_float3 moved = {};
		CThunder::Ammunition_Desc desc = {};

		desc.fRotationPerSec = XMConvertToRadians(240.f); desc.fSpeedPerSec = 100.f; desc.iDamage = 1; desc.pShooter = this;
		XMStoreFloat3((_float3*)&desc.transMat.m[3], m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_UP)); 
		XMStoreFloat3(&desc.vTargetPos, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Thunder"), TEXT("Prototype_GameObject_BossScrew"), &desc);
		m_vLightningCoolTime.x = 0.f;
	}
	else if (m_vLightningCoolTime.x > m_vLightningCoolTime.z)
	{
		auto* pRef = m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Thunder"));

		if(pRef != nullptr)
		{
			auto& refThunders = *pRef;
			for (auto iter = begin(refThunders); iter != end(refThunders);) {
				Safe_Release(*iter);
				iter = refThunders.erase(iter);
			}
		}
		/*if (FAILED(m_pGameInstance->Delete_CloneObject_ByPointer(LEVEL_GAMEPLAY, TEXT("Layer_Thunder"),
			m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Thunder")))))
		{
			int a = 10;
		}*/
		m_vLightningCoolTime.x = 0;
		
		if (m_vLightningCoolTime.x >= m_vLightningCoolTime.y) {
			m_eActionState = STATE_ATTACK_LIGHTNING;
			m_vLightningCoolTime.x = 0.f;
		}
		else if (m_vGrenadeCoolTime.x >= m_vGrenadeCoolTime.y) {
			m_eActionState = STATE_ATTACK_EXPLOSIVE;
			m_vGrenadeCoolTime.x = 0.f;
		}
		else if (m_vLightningShortCoolTime.x >= m_vLightningShortCoolTime.y) {
			m_eActionState = STATE_ATTACK_LIGHTNING_SHORT;
			m_vLightningShortCoolTime.x = 0.f;
		}
		
		else if (m_fMeleeDistance > XMVectorGetX(XMVector3Length(destPos - currentPos))) {
			m_eActionState = STATE_ATTACK_MELEE;
		}
		else if (XMVectorGetX(XMVector3Length(m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION))) < m_fDealDistance)
		{
			m_eActionState = STATE_TURN;
		}
		else
		{
			find_path();
			m_eActionState = STATE_WALK;
		}
	}
}

void CCortexReaver::attack_lightning_short(_float fTimeDelta)
{
	_vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector destPos = m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION);
	_vector vDir = destPos - currentPos;
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float fDegree = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDir, vLook))
		/ (XMVectorGetX(XMVector3Length(vDir)) * XMVectorGetX(XMVector3Length(vLook)))));

	XMVECTOR crossProduct = XMVector3Cross(vLook, vDir);
	_float fy = XMVectorGetY(crossProduct);

	if ((fDegree < -5.f || 5.f < fDegree) && !isnan(fDegree))
	{
		if (fy > 0) {
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), fTimeDelta * 0.25f);
		}
		else {
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), -fTimeDelta * 0.25f);
		}
	}

	if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::explosive_attack_loop) {
		m_pModelCom->SetUp_Animation(explosive_attack_loop, true, 2.f);
		_float3 moved = {};
		CThunder::Ammunition_Desc desc = {};

		desc.fRotationPerSec = XMConvertToRadians(240.f); desc.fSpeedPerSec = 100.f; desc.iDamage = 1; desc.pShooter = this;
		XMStoreFloat3((_float3*)&desc.transMat.m[3], m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_UP));
		XMStoreFloat3(&desc.vTargetPos, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		_float fRand = m_pGameInstance->Get_Random(0.f, 11.f);
		if (fRand < 1.f) {
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Shot_1"), 1.f);
		}
		else if (fRand < 2.f) {
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Shot_2"), 1.f);
		}
		else if (fRand < 3.f) {
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Shot_3"), 1.f);
		}
		else if (fRand < 4.f) {
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Shot_4"), 1.f);
		}
		else if (fRand < 5.f) {
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Shot_5"), 1.f);
		}
		else if (fRand < 6.f) {
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Shot_6"), 1.f);
		}
		else if (fRand < 7.f) {
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Shot_7"), 1.f);
		}
		else if (fRand < 8.f) {
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Shot_8"), 1.f);
		}
		else if (fRand < 9.f) {
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Shot_9"), 1.f);
		}
		else if (fRand < 10.f) {
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Shot_10"), 1.f);
		}
		else {
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CortexReaver_Attack_Range_Shot_11"), 1.f);
		}

		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_BossScrew_Short"), &desc);
		m_vLightningShortCoolTime.x = 0.f;
	}
	if (m_vLightningCoolTime.x >= m_vLightningCoolTime.y) {
		m_eActionState = STATE_ATTACK_LIGHTNING;
		m_vLightningCoolTime.x = 0.f;
	}

	else if (m_vGrenadeCoolTime.x >= m_vGrenadeCoolTime.y) {
		m_eActionState = STATE_ATTACK_EXPLOSIVE;
		m_vGrenadeCoolTime.x = 0.f;
	}
	else if (m_fMeleeDistance > XMVectorGetX(XMVector3Length(destPos - currentPos))) {
		m_eActionState = STATE_ATTACK_MELEE;
	}
	else if (XMVectorGetX(XMVector3Length(m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION))) < m_fDealDistance)
	{
		m_eActionState = STATE_TURN;
	}
	else
	{
		find_path();
		m_eActionState = STATE_WALK;
	}
	
}

void CCortexReaver::CinematicDrop(_float fTimeDelta)
{
	if (m_pModelCom->GetCurrentAnimIndex() != dropdown_1000u) {
		m_pModelCom->SetUp_Animation(dropdown_1000u, false, 0.f);
	}
	else if (m_pModelCom->GetCurrentAnimIndex() == dropdown_1000u && m_fDropTime < 4.f) {
		/*find_Player(fTimeDelta);
		m_eActionState = STATE_WALK;*/
		m_fDropTime += fTimeDelta;
	}
	else if (m_pModelCom->GetCurrentAnimIndex() == dropdown_1000u && m_fDropTime >= 4.f)
	{
		find_Player(fTimeDelta);
		m_eActionState = STATE_ATTACK_NORMAL_RANGE;
	}
}

CCortexReaver* CCortexReaver::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCortexReaver* pInstance = new CCortexReaver(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(TEXT("Failed to Created : CCortexReaver"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCortexReaver::Clone(void* pArg)
{
	CCortexReaver* pInstance = new CCortexReaver(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCortexReaver"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCortexReaver::Free()
{
	__super::Free();
}
