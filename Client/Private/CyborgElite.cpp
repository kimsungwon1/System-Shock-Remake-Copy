#include "stdafx.h"

#include "CyborgElite.h"

#include "Navigation.h"

#include "FPSPlayer.h"

#include "Rocket.h"
#include "Grenade.h"

#include "Physics.h"

#include "SoundMgr.h"

#include "GameInstance.h"

CCyborgElite::CCyborgElite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnitObject{ pDevice, pContext }
{
}

CCyborgElite::CCyborgElite(const CUnitObject& Prototype)
	: CUnitObject{ Prototype }
{
}

HRESULT CCyborgElite::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCyborgElite::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_CyborgElite"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

	aabbDesc.vExtents = { 0.5f, 0.9f, 0.5f };
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
	headDesc.vExtents = { 0.1f, 0.1f, 0.1f };
	headDesc.vCenter = { 0.f, headDesc.vExtents.y, 0.f };
	headDesc.pOwner = this;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_Head"), reinterpret_cast<CComponent**>(&m_pHeadColliderCom), &headDesc))) {
		return E_FAIL;
	}

	m_fDealDistance = 15.f;

	m_eActionState = Elite_State ::STATE_STAND;

	m_pModelCom->SetUp_Animation(Anim_State::Idle, true);

	m_iHP = 300;
	m_eUnitType = UNIT_Elite;

	return S_OK;
}

void CCyborgElite::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CCyborgElite::Update(_float fTimeDelta)
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
	else if (m_eActionState == STATE_TELEPORT_INTRO) {
		CinematicTeleport(fTimeDelta);
	}
	else if (m_eActionState == STATE_WALK) {
		move_By_AStar(fTimeDelta);
	}
	else if (m_eActionState == STATE_TURN) {
		turn_To_Player(fTimeDelta);
	}
	else if (m_eActionState == STATE_ATTACK_NORMAL_RANGE) {
		attack_normal(fTimeDelta);
	}
	else if (m_eActionState == STATE_ATTACK_MELEE) {
		attack_melee(fTimeDelta);
	}
	else if (m_eActionState == STATE_ATTACK_UPPERCUT) {
		attack_melee_Uppercut(fTimeDelta);
	}
	else if (m_eActionState == STATE_ATTACK_GRENADE) {
		attack_explosive(fTimeDelta);
	}
	else if (m_eActionState == STATE_DODGE) {
		dodge(fTimeDelta);
	}
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

	_float4x4 rootMat = {};
	m_bAnimationFinished = m_pModelCom->Play_Animation(fTimeDelta, &rootMat, true);
	m_vGrenadeCoolTime.x += fTimeDelta;
	m_vUppercutCoolTime.x += fTimeDelta;
}

void CCyborgElite::Late_Update(_float fTimeDelta)
{
	if (m_eActionState == STATE_STAND || (m_eActionState == STATE_TELEPORT_INTRO
		&& m_bAnimationFinished)) {
		return;
	}

	__super::Late_Update(fTimeDelta);
}

HRESULT CCyborgElite::Render()
{
	__super::Render();

	return S_OK;
}

_bool CCyborgElite::setIdle(_float fTimeDelta)
{
	m_pModelCom->SetUp_Animation(Anim_State::Idle, true, 0.f);

	if (!CUnitObject::find_Player(fTimeDelta)) { //false 리턴이 찾은거임
		m_eActionState = STATE_TELEPORT_INTRO;

		//find_path();
	}

	return true;
}

_bool CCyborgElite::setDead(_float fTimeDelta)
{
	if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::Death_h)
	{
		CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Vox_State_OnDeath_1"), 1.f);
		m_pModelCom->SetUp_Animation(Anim_State::Death_h, false);
	}
	_float4x4 rootTransMat = {};

	m_bAnimationFinished = m_pModelCom->Play_Animation(fTimeDelta, &rootTransMat, true);
	return true;
}

_bool CCyborgElite::turn_To_Player(_float fTimeDelta)
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
		m_pModelCom->SetUp_Animation(Anim_State::Turn_left_0, false);
	}
	else if (5 < fDegree && fDegree < 180.f) {
		m_pModelCom->SetUp_Animation(Anim_State::Turn_left_0, false);
	}
	
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

	// 아직 각도가 벌어짐
	/*if ((fDegree < -5.f || 5.f < fDegree) && !isnan(fDegree))
	{
		_vector newVec = XMVectorLerp(UnitDirVec, NextDirVec, m_pTransformCom->GetRotationPerSec() * fTimeDelta / fDegree);
		m_pTransformCom->SetLookToRightUp_ground(newVec);
		return true;
	}
	else if (isnan(fDegree) && -(UnitDirVec.m128_f32[0] / abs(UnitDirVec.m128_f32[0])) == NextDirVec.m128_f32[0] / NextDirVec.m128_f32[0]) {
		_vector newVec = XMVectorLerp(UnitDirVec, NextDirVec, m_pTransformCom->GetRotationPerSec() * fTimeDelta / 180.f);
		m_pTransformCom->SetLookToRightUp_ground(newVec);
	}*/
	// 각도가 충분히 좁혀짐
	else {
		//거리마저 좁혀지면 - 네비게이션 비면
		if (isDirectlyLookingToPlayer && m_pNavigationComs[m_iCurrentLevel]->IsReservedEmpty()) {
			_float dist = XMVectorGetX(XMVector3Length(m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

			if (m_vUppercutCoolTime.x >= m_vUppercutCoolTime.y) {
				m_eActionState = STATE_ATTACK_UPPERCUT;
				m_vUppercutCoolTime.x = 0.f;
			}

			else if (m_vGrenadeCoolTime.x >= m_vGrenadeCoolTime.y) {
				m_eActionState = STATE_ATTACK_GRENADE;
				m_vGrenadeCoolTime.x = 0.f;
			}
			else if (dist < m_fDealDistance) {
				if (dist < m_fMeleeDistance) {
					m_eActionState = STATE_ATTACK_MELEE;
				}
				else {
					m_eActionState = STATE_ATTACK_NORMAL_RANGE;
				}
			}

		}
		else {
			//이제 걸을 시간
			m_eActionState = STATE_WALK;
		}

		return false;
	}
}

_bool CCyborgElite::move_By_AStar(_float fTimeDelta)
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
		else if (isnan(fDegree) && -(vDir.m128_f32[0] / abs(vDir.m128_f32[0])) == vLook.m128_f32[0] / vLook.m128_f32[0]) {
			_vector newVec = XMVectorLerp(vLook, vDir, m_pTransformCom->GetRotationPerSec() * fTimeDelta / 180.f);
			m_pTransformCom->SetLookToRightUp_ground(newVec);
		}
	}

	// 걷기 실행
	// 서로 벽 있는 것도 조건에 추가해야한다.
	if (XMVectorGetX(XMVector3Length(destPos - currentPos)) > m_fDealDistance || TileBetweenMeAndPlayer())
	{
		m_pModelCom->SetUp_Animation(Anim_State::Run_front, true, 1.5f);

		_float3 fromNextToMine = {};

		_vector vMoved = m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &fromNextToMine, m_pColliderCom);

		// 상대 콜라이더에 의해 막혔을 경우.
		if (fromNextToMine.x != 0.f || fromNextToMine.y != 0.f || fromNextToMine.z != 0.f)
		{
			_vector oppositeCenter = XMLoadFloat3(m_pColliderCom->Get_Center()) + vMoved - XMLoadFloat3(&fromNextToMine);

			_float3 realFromNextToMine = {};
			XMStoreFloat3(&realFromNextToMine, XMLoadFloat3(m_pColliderCom->Get_Center()) - oppositeCenter);

			//m_pModelCom->SetUp_Animation(Anim_State::Run_, true);

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
		if (m_vUppercutCoolTime.x >= m_vUppercutCoolTime.y) {
			m_eActionState = STATE_ATTACK_UPPERCUT;
			m_vUppercutCoolTime.x = 0.f;
		}
		else if (m_vGrenadeCoolTime.x >= m_vGrenadeCoolTime.y) {
			m_eActionState = STATE_ATTACK_GRENADE;
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

_bool CCyborgElite::shoot_Player(_float fTimeDelta)
{
	m_pModelCom->SetUp_Animation(Anim_State::Attack_ranged, true);
	if (m_vShootCoolTime.x >= m_vShootCoolTime.y)
	{
		_float fRand = m_pGameInstance->Get_Random(0.f, 6.f);
		if (fRand < 1.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Range_A_Attack_Foley_1"), 1.f);
		}
		else if (fRand < 2.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Range_A_Attack_Foley_2"), 1.f);
		}
		else if (fRand < 3.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Range_A_Attack_Foley_3"), 1.f);
		}
		else if (fRand < 4.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Range_A_Attack_Foley_4"), 1.f);
		}
		else if (fRand < 5.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Range_A_Attack_Foley_5"), 1.f);
		}
		else 
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Range_A_Attack_Foley_6"), 1.f);
		}
		CRocket::Ammunition_Desc desc = {};
		desc.bShot = true; desc.fRotationPerSec = 10.f; desc.pShooter = this; desc.iDamage = 3;
		desc.fSpeedPerSec = 15.f; XMStoreFloat4x4(&desc.transMat, XMMatrixIdentity());

		XMStoreFloat3(&desc.vTargetPos, m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.f, 0.f, 0.f));

		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		position += m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 4.f + m_pTransformCom->Get_State(CTransform::STATE_UP) * 1.5f;
		XMStoreFloat3((_float3*)(&desc.transMat.m[3]), position);

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FlyingBullet"), TEXT("Prototype_GameObject_Rocket"), &desc))) {
			int a = 10;
		}

		m_vShootCoolTime.x = 0.f;
	}
	return _bool();
}

void CCyborgElite::calculate_Grenade_Shooting()
{
	_vector vDir = m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float distance = XMVectorGetX(XMVector3Length(vDir)) - XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_LOOK))) * 3;

	_float v = 15.f;
	//_float cal = (distance - 1) * CPhysics::s_fGravity / powf(v, 2.f);
	_float ResultRadian = 0;

	_float min = 100000.f;
	for (_float fDegree = 2.5f; fDegree <= 45.f; fDegree += 2.5f) {
		_float resultDistance = powf(v, 2.f) * sinf(XMConvertToRadians(fDegree) * 2) / CPhysics::s_fGravity;
		if (abs(resultDistance - distance) < min) {
			min = abs(resultDistance - distance);
			ResultRadian = XMConvertToRadians(fDegree);
		}
	}

	CGrenade::Grenade_DESC desc = {};
	desc.bShot = true; desc.fSpeedPerSec = v; desc.fRotationPerSec = 1.f; desc.pShooter = this; desc.iCurrentLevel = m_iCurrentLevel;
	XMStoreFloat4x4(&desc.transMat, XMMatrixIdentity());

	_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	position += m_pTransformCom->Get_State(CTransform::STATE_UP) * 2.f;
	XMStoreFloat3((_float3*)(&desc.transMat.m[3]), position);
	XMStoreFloat3(&desc.vTargetPos, m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION));

	XMStoreFloat3(&desc.vThrowdir, XMVector3Normalize(XMVector3TransformNormal(vDir, XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), -ResultRadian))) * v);
	
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FlyingBullet"), TEXT("Prototype_GameObject_Grenade"), &desc))) {
		int a = 10;
	}
}

_bool CCyborgElite::attack_normal(_float fTimeDelta)
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
		else if (isnan(fDegree) && -(vDir.m128_f32[0] / abs(vDir.m128_f32[0])) == vLook.m128_f32[0] / vLook.m128_f32[0]) {
			_vector newVec = XMVectorLerp(vLook, vDir, m_pTransformCom->GetRotationPerSec() * fTimeDelta / 180.f);
			m_pTransformCom->SetLookToRightUp_ground(newVec);
		}

		if (m_vUppercutCoolTime.x >= m_vUppercutCoolTime.y) {
			m_eActionState = STATE_ATTACK_UPPERCUT;
			m_vUppercutCoolTime.x = 0.f;
		}
		// 각도 안맞아도 공격하긴 한다.
		else if (m_vGrenadeCoolTime.x >= m_vGrenadeCoolTime.y) {
			m_eActionState = STATE_ATTACK_GRENADE;
			m_vGrenadeCoolTime.x = 0.f;
		}

		else if (XMVectorGetX(XMVector3Length(vDir)) < m_fMeleeDistance) {
			m_eActionState = Elite_State::STATE_ATTACK_MELEE;
		}
		else {
			shoot_Player(fTimeDelta);
		}
	}

	// 플레이어가 움직여 턴 해야할때
	else {
		if (fDegree != 0.f && !isnan(fDegree))
		{
			_vector newVec = XMVectorLerp(vLook, vDir, m_pTransformCom->GetRotationPerSec() * fTimeDelta / fDegree);
			m_pTransformCom->SetLookToRightUp_ground(newVec);
		}
		else if (isnan(fDegree) && -(vDir.m128_f32[0] / abs(vDir.m128_f32[0])) == vLook.m128_f32[0] / vLook.m128_f32[0]) {
			_vector newVec = XMVectorLerp(vLook, vDir, m_pTransformCom->GetRotationPerSec() * fTimeDelta / 180.f);
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

void CCyborgElite::attack_explosive(_float fTimeDelta)
{
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vDir = m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fDegree = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vLook, vDir)) / XMVectorGetX(XMVector3Length(vLook)) / XMVectorGetX(XMVector3Length(vDir))));
	if(fDegree != 0 && !isnan(fDegree) && fDegree < -5.f || 5.f < fDegree)
	{
		_vector newVec = XMVectorLerp(vLook, vDir, m_pTransformCom->GetRotationPerSec() * 2.f * fTimeDelta / fDegree);
		m_pTransformCom->SetLookToRightUp_ground(newVec);
	}

	
	if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::Grenade_throw_realThrow) {
		m_pModelCom->SetUp_Animation(Grenade_throw_realThrow, false, 1.f);
		_float fRand = m_pGameInstance->Get_Random(0.f, 5.f);
		if (fRand < 1.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Range_GrenadeThrow_Whoosh_1"), 1.f);
		}
		else if (fRand < 2.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Range_GrenadeThrow_Whoosh_2"), 1.f);
		}
		else if (fRand < 3.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Range_GrenadeThrow_Whoosh_3"), 1.f);
		}
		else if (fRand < 4.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Range_GrenadeThrow_Whoosh_4"), 1.f);
		}
		else
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Range_GrenadeThrow_Whoosh_5"), 1.f);
		}

		calculate_Grenade_Shooting();
	}
	else if (m_pModelCom->GetCurrentAnimIndex() == Grenade_throw_realThrow && m_bAnimationFinished) {
		m_eActionState = STATE_TURN;
	}
}

void CCyborgElite::attack_melee(_float fTimeDelta)
{
	if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::Attack_melee_a && m_pModelCom->GetCurrentAnimIndex() != Anim_State::Attack_melee_b) {
		m_pModelCom->SetUp_Animation(Attack_melee_a, false, 0.3f);
	}
	else if (m_pModelCom->GetCurrentAnimIndex() == Anim_State::Attack_melee_a && !m_bAnimationFinished)
	{
		_float3 moved = {};
		m_pTransformCom->Walk(0, fTimeDelta * 0.2f, m_pNavigationComs[m_iCurrentLevel], &moved, m_pColliderCom);
	}
	else if (m_pModelCom->GetCurrentAnimIndex() == Anim_State::Attack_melee_a && m_bAnimationFinished)
	{
		//m_pTransformCom->LookAt(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pModelCom->SetUp_Animation(Attack_melee_b, false, 0.3f);
	}
	else if (m_pModelCom->GetCurrentAnimIndex() == Anim_State::Attack_melee_b && !m_bAnimationFinished)
	{
		_float3 moved = {};
		m_pTransformCom->Walk(0, fTimeDelta * 0.2f, m_pNavigationComs[m_iCurrentLevel], &moved, m_pColliderCom);
	}
	else if (m_pModelCom->GetCurrentAnimIndex() == Anim_State::Attack_melee_b && m_bAnimationFinished)
	{
		//공격한 다음, 가까우면 근거리 공격
		// 각도 안맞아도 공격하긴 한다.
		if (m_vUppercutCoolTime.x >= m_vUppercutCoolTime.y) {
			m_eActionState = STATE_ATTACK_UPPERCUT;
			m_vUppercutCoolTime.x = 0.f;
		}
		else if (m_vGrenadeCoolTime.x >= m_vGrenadeCoolTime.y) {
			m_eActionState = STATE_ATTACK_GRENADE;
			m_vGrenadeCoolTime.x = 0.f;
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

void CCyborgElite::attack_melee_Uppercut(_float fTimeDelta)
{
	if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::Run_Attack_Uppercut) {
		m_pModelCom->SetUp_Animation(Run_Attack_Uppercut, false, 0.3f);
		_float fRand = m_pGameInstance->Get_Random(0.f, 5.f);
		if (fRand < 1.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Servo_1"), 1.f);
		}
		else if (fRand < 2.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Servo_2"), 1.f);
		}
		else if (fRand < 3.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Servo_3"), 1.f);
		}
		else if (fRand < 4.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Servo_4"), 1.f);
		}
		else
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Servo_5"), 1.f);
		}
	}
	else if (m_pModelCom->GetCurrentAnimIndex() == Anim_State::Run_Attack_Uppercut && !m_bAnimationFinished)
	{
		_float3 moved = {};
		m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &moved, m_pColliderCom);

		// 플레이어가 코앞에 있는 어퍼컷 판정
		_vector vTargetPosition = m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) +
			m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_UP) * 0.5f;
		_vector vEliteToTarget = vTargetPosition - (m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_UP) * 0.5f);
		_float fDistance = XMVectorGetX(XMVector3Length(vEliteToTarget));
		// elite와 상대 사이 벡터와 elite의 look vector의 각도차
		_float fDegree = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vEliteToTarget, m_pTransformCom->Get_State(CTransform::STATE_LOOK))) / XMVectorGetX(XMVector3Length(vEliteToTarget)) / XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_LOOK)))));
		if (fDegree < 90.f && fDegree > -90.f && fDistance < 2.0f) {
			//어퍼컷 발동
			auto* pPlayer = dynamic_cast<CFPSPlayer*>(m_pTargetObject);

			pPlayer->ReceivePhysicsPower(_vector{ 0.f, 0.1f, 0.f });
			pPlayer->ReceiveDamage(5);
		}
	}
	else if (m_pModelCom->GetCurrentAnimIndex() == Anim_State::Run_Attack_Uppercut && m_bAnimationFinished)
	{
		if (m_vGrenadeCoolTime.x >= m_vGrenadeCoolTime.y) {
			m_eActionState = STATE_ATTACK_GRENADE;
			m_vGrenadeCoolTime.x = 0.f;
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

void CCyborgElite::CinematicTeleport(_float fTimeDelta)
{
	if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::Teleport_intro) {
		m_pModelCom->SetUp_Animation(Teleport_intro, false, 0.f);
		CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgElite_Vox_Appear"), 1.f);
	}
	else if (m_pModelCom->GetCurrentAnimIndex() == Teleport_intro && m_fDropTime < 4.f) {
		/*find_Player(fTimeDelta);
		m_eActionState = STATE_WALK;*/
		m_fDropTime += fTimeDelta;
	}
	else if (m_pModelCom->GetCurrentAnimIndex() == Teleport_intro && m_fDropTime >= 4.f)
	{
		find_Player(fTimeDelta);
		m_eActionState = STATE_ATTACK_NORMAL_RANGE;
	}
}

void CCyborgElite::dodge(_float fTimeDelta)
{
}

CCyborgElite* CCyborgElite::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCyborgElite* pInstance = new CCyborgElite(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(TEXT("Failed to Created : CCyborgElite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCyborgElite::Clone(void* pArg)
{
	CCyborgElite* pInstance = new CCyborgElite(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCyborgElite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCyborgElite::Free()
{
	__super::Free();
}