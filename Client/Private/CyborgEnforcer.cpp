#include "stdafx.h"

#include "CyborgEnforcer.h"

#include "Navigation.h"
#include "Collider.h"

#include "Fireball.h"

#include "SoundMgr.h"

#include "GameInstance.h"

#include "FPSPlayer.h"

CCyborgEnforcer::CCyborgEnforcer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnitObject{ pDevice, pContext }
{
}

CCyborgEnforcer::CCyborgEnforcer(const CUnitObject& Prototype)
	: CUnitObject{ Prototype }
{
}

HRESULT CCyborgEnforcer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCyborgEnforcer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Enforcer"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;
	
	CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

	aabbDesc.vExtents = { 0.5f, 0.7f, 0.5f };
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
	headDesc.vCenter = { 0.f, 0.f, 0.f };
	headDesc.pOwner = this;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_Head"), reinterpret_cast<CComponent**>(&m_pHeadColliderCom), &headDesc))) {
		return E_FAIL;
	}

	m_fDealDistance = 10.f;

	m_iActionState = ENFORCER_STATE::STATE_STAND;

	m_pModelCom->SetUp_Animation(Anim_State::Idle, true);

	m_iHP = 14;
	m_eUnitType = UNIT_Enforcer;

	return S_OK;
}

void CCyborgEnforcer::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CCyborgEnforcer::Update(_float fTimeDelta)
{
	m_vShootCoolTime.x += fTimeDelta;

	_float3 pIn = {};
	
	if (m_bDead) {
		setDead(fTimeDelta);
		m_fDeadTime -= fTimeDelta;
		if (m_fDeadTime < -1.f) {
			m_fDeadTime = 0.f;
		}
		return;
	}

	if (m_iActionState & STATE_STAND) {
		setIdle(fTimeDelta);
	}
	if (m_iActionState & STATE_TURN) {
		turn_To_Player(fTimeDelta);
	}
	if (m_iActionState & STATE_WALK) {
		move_By_AStar(fTimeDelta);
	}
	if (m_iActionState & STATE_ATTACK) {
		attack_normal(fTimeDelta);
	}
	_float4x4 rootMat = {};
	m_pModelCom->Play_Animation(fTimeDelta, &rootMat, true);
}

void CCyborgEnforcer::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CCyborgEnforcer::Render()
{
	__super::Render();

	return S_OK;
}

_bool CCyborgEnforcer::setIdle(_float fTimeDelta)
{
	m_pModelCom->SetUp_Animation(Anim_State::Idle, true, 0.f);

	if (!CUnitObject::find_Player(fTimeDelta)) { //false ������ ã������
		m_iActionState ^= STATE_STAND;
		find_path();
	}


	return true;
}

_bool CCyborgEnforcer::setDead(_float fTimeDelta)
{
	if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::Death_h)
	{
		_float fRand = m_pGameInstance->Get_Random(0.f, 6.f);
		if (fRand < 1.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgEnforcer_Vox_State_OnDeath_01"), 1.f);
		}
		else if (fRand < 2.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgEnforcer_Vox_State_OnDeath_02"), 1.f);
		}
		else if (fRand < 3.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgEnforcer_Vox_State_OnDeath_03"), 1.f);
		}
		else if (fRand < 4.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgEnforcer_Vox_State_OnDeath_04"), 1.f);
		}
		else if (fRand < 5.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgEnforcer_Vox_State_OnDeath_05"), 1.f);
		}
		else if (fRand < 6.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgEnforcer_Vox_State_OnDeath_06"), 1.f);
		}

		m_pModelCom->SetUp_Animation(Anim_State::Death_h, false);
	}
	_float4x4 rootTransMat = {};

	m_bAnimationFinished = m_pModelCom->Play_Animation(fTimeDelta, &rootTransMat, true);
	return true;
}

_bool CCyborgEnforcer::turn_To_Player(_float fTimeDelta)
{
	if (m_pTargetObject == nullptr) {
		return false;
	}
	_float fDegree = 0.f;

	_vector UnitDirVec = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	// �� �շ� ������ �� �ո� �÷��̾� ��������, �� �� �շ� ���� ���� �� �������� �ٶ���� ��
	// ���߿� �߰��ϰ� �����ؾ���!!!
	_vector NextDirVec = {};
	_bool isDirectlyLookingToPlayer = false;

	// �׺���̼��� �ٴٶ���
	if (m_pNavigationComs[m_iCurrentLevel]->IsReservedEmpty() || IsDestinationDirectly()) {
		isDirectlyLookingToPlayer = true;
		NextDirVec = m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION)
			- m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	}
	// �׺���̼� ��
	else {
		NextDirVec = m_pNavigationComs[m_iCurrentLevel]->Get_NextCellPos()
			- m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	}
	_float cos = XMVectorGetX(XMVector3Dot(UnitDirVec, NextDirVec)) / (XMVectorGetX((XMVector3Length(UnitDirVec))) * XMVectorGetX(XMVector3Length(NextDirVec)));
	fDegree = XMConvertToDegrees(acosf(cos));

	// ���� ������ ������
	if ((fDegree < -5.f || 5.f < fDegree) && !isnan(fDegree))
	{
		_vector newVec = XMVectorLerp(UnitDirVec, NextDirVec, m_pTransformCom->GetRotationPerSec() * fTimeDelta / fDegree);
		m_pTransformCom->SetLookToRightUp_ground(newVec);
		return true;
	}
	// ������ ����� ������
	else {
		//�Ÿ����� �������� - �׺���̼� ���
		if (isDirectlyLookingToPlayer && m_pNavigationComs[m_iCurrentLevel]->IsReservedEmpty()) {
			m_iActionState |= STATE_ATTACK;
		}
		else {
			//���� ���� �ð�
			m_iActionState |= STATE_WALK;
		}

		m_iActionState ^= STATE_TURN;
		return false;
	}
}

_bool CCyborgEnforcer::shoot_Player(_float fTimeDelta)
{
	if (m_vShootCoolTime.x > m_vShootCoolTime.y)
	{
		_float fRand = m_pGameInstance->Get_Random(0.f, 2.f);
		if (fRand < 1.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgEnforcer_Attack_Range_1"), 1.f);
		}
		else if (fRand < 2.f)
		{
			CSoundMgr::Get_Instance()->PlaySound(TEXT("Enemy_CyborgEnforcer_Attack_Range_2"), 1.f);
		}

		m_pModelCom->SetUp_Animation_Immediately(Anim_State::Range, false, 2.f);
		CAmmunition::Ammunition_Desc desc = {};
		desc.bShot = true; desc.fRotationPerSec = XMConvertToRadians(90.f); desc.fSpeedPerSec = 30.f;
		desc.iCurrentLevel = m_iCurrentLevel; desc.iDamage = 1; desc.pShooter = this; XMStoreFloat4x4(&desc.transMat, XMMatrixIdentity());
		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		position += m_pTransformCom->Get_State(CTransform::STATE_UP) * 1.55f - m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 0.20f;
		XMStoreFloat3((_float3*)(&desc.transMat.m[3]), position); desc.bTargetSet = false;
		XMStoreFloat3(&desc.vTargetPos, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FlyingBullet"), TEXT("Prototype_GameObject_Fireball"), &desc)))
		{
			int a = 10;
		}
		m_vShootCoolTime.x = 0.f;
	}


	/*if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::Range || 
		m_pModelCom->GetCurrentAnimIndex() == Anim_State::Range && m_pModelCom->IsAnimFinished()) {
		CAmmunition::Ammunition_Desc desc = {};
		desc.bShot = true; desc.fRotationPerSec = XMConvertToRadians(90.f); desc.fSpeedPerSec = 30.f;
		desc.iCurrentLevel = m_iCurrentLevel; desc.iDamage = 1; desc.pShooter = this; XMStoreFloat4x4(&desc.transMat, XMMatrixIdentity());
		_vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		position += m_pTransformCom->Get_State(CTransform::STATE_UP) * 1.55f - m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 0.20f;
		XMStoreFloat3((_float3*)(&desc.transMat.m[3]), position); desc.bTargetSet = false;
		XMStoreFloat3(&desc.vTargetPos, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FlyingBullet"), TEXT("Prototype_GameObject_Fireball"), &desc)))
		{
			int a = 10;
		}
	}
	m_pModelCom->SetUp_Animation(Anim_State::Range);*/

	return _bool();
}

_bool CCyborgEnforcer::attack_normal(_float fTimeDelta)
{
	_vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector destPos = m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION);
	_vector vDir = destPos - currentPos;
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float fDegree = XMConvertToDegrees(acosf(XMVectorGetX(XMVector3Dot(vDir, vLook))
		/ (XMVectorGetX(XMVector3Length(vDir)) * XMVectorGetX(XMVector3Length(vLook)))));

	auto* pPlayer = dynamic_cast<CFPSPlayer*>(m_pTargetObject);
	// �÷��̾ ������ ������
	// �̶��� �÷��̾�� ���� ������ �� ������ �� �ؾߵ�.
	if (!pPlayer->IsMoving()) {
		// ���� �ȸ¾� ��.
		if ((fDegree < -5.f || 5.f < fDegree) && !isnan(fDegree)) {
			_vector newVec = XMVectorLerp(vLook, vDir, m_pTransformCom->GetRotationPerSec() * fTimeDelta / fDegree);
			m_pTransformCom->SetLookToRightUp_ground(newVec);
		}

		// ���� �ȸ¾Ƶ� ��� ���.
		shoot_Player(fTimeDelta);
	}

	// �÷��̾ ������ �� �ؾ��Ҷ�
	else {
		if (fDegree != 0.f && !isnan(fDegree))
		{
			_vector newVec = XMVectorLerp(vLook, vDir, m_pTransformCom->GetRotationPerSec() * fTimeDelta / fDegree);
			m_pTransformCom->SetLookToRightUp_ground(newVec);
		}

		// �÷��̾ �����Ÿ� ������ �����ų� �� �ڷ� ����
		// �ٷ� ������ ������ ���� ���� �����ٰ� �ض�
		if (XMVectorGetX(XMVector3Length(vDir)) >= m_fDealDistance || TileBetweenMeAndPlayer()) {
			// ���� ��ã��

			find_path();
			m_iActionState ^= STATE_ATTACK;
		}
		else {
			shoot_Player(fTimeDelta);
		}
	}

	return _bool();
}

void CCyborgEnforcer::find_path()
{
	if (XMVectorGetX(XMVector3Length((m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)))) > m_fDealDistance)
	{
		XMStoreFloat3(&m_vDestPos, m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION));
		m_pNavigationComs[m_iCurrentLevel]->Set_FinalDest(m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION));
		m_pNavigationComs[m_iCurrentLevel]->AStar();
		m_bMoving_By_AStar = true;
	}
	m_iActionState |= STATE_TURN;
}

_bool CCyborgEnforcer::move_By_AStar(_float fTimeDelta)
{
	_vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector destPos = XMVectorSetW(XMLoadFloat3(&m_vDestPos), 1.f);
	_vector vDir = {};
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_float fDegree = {};

	// ���� ����
	if (auto* pTarget = dynamic_cast<CFPSPlayer*>(m_pTargetObject)) {
		if (pTarget->IsMoving()) {
			XMStoreFloat3(&m_vDestPos, m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION));
			m_pNavigationComs[m_iCurrentLevel]->Set_FinalDest(m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION));
			m_pNavigationComs[m_iCurrentLevel]->AStar();
		}
	}
	if (m_pNavigationComs[m_iCurrentLevel]->IsReservedEmpty()) {
		m_iActionState |= STATE_TURN;
		m_iActionState ^= STATE_WALK;
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

	// �ȱ� ����
	// ���� �� �ִ� �͵� ���ǿ� �߰��ؾ��Ѵ�.
	if (XMVectorGetX(XMVector3Length(destPos - currentPos)) > m_fDealDistance || TileBetweenMeAndPlayer())
	{
		m_pModelCom->SetUp_Animation(Anim_State::Walk_front, true, 1.5f);

		_float3 fromNextToMine = {};

		_vector vMoved = m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &fromNextToMine, m_pColliderCom);

		// ��� �ݶ��̴��� ���� ������ ���.
		if (fromNextToMine.x != 0.f || fromNextToMine.y != 0.f || fromNextToMine.z != 0.f)
		{
			_vector oppositeCenter = XMLoadFloat3(m_pColliderCom->Get_Center()) + vMoved - XMLoadFloat3(&fromNextToMine);

			_float3 realFromNextToMine = {};
			XMStoreFloat3(&realFromNextToMine, XMLoadFloat3(m_pColliderCom->Get_Center()) - oppositeCenter);

			//m_pModelCom->SetUp_Animation(Anim_State::Walk_front, true);

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
	// ���� ���� ���� �Ÿ��� �����
	else {
		m_pNavigationComs[m_iCurrentLevel]->Initialize_AStar();
		m_iActionState ^= STATE_WALK;
		m_iActionState |= STATE_ATTACK;

	}

	return _bool();
}

_bool CCyborgEnforcer::find_Player(_float fTimeDelta)
{
	return _bool();
}


CCyborgEnforcer* CCyborgEnforcer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCyborgEnforcer* pInstance = new CCyborgEnforcer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(TEXT("Failed to Created : CCyborgEnforcer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCyborgEnforcer::Clone(void* pArg)
{
	CCyborgEnforcer* pInstance = new CCyborgEnforcer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCyborgEnforcer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCyborgEnforcer::Free()
{
	__super::Free();
}
