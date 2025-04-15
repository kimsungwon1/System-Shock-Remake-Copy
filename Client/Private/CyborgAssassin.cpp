#include "stdafx.h"
#include "CyborgAssassin.h"
#include "Navigation.h"
#include "Collider.h"

#include "Bullet.h"
#include "Effect_Muzzle.h"

#include "SoundMgr.h"

#include "GameInstance.h"

#include "FPSPlayer.h"

CCyborgAssassin::CCyborgAssassin(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUnitObject { pDevice, pContext }
{
}

CCyborgAssassin::CCyborgAssassin(const CUnitObject& Prototype)
	: CUnitObject { Prototype }
{
}

HRESULT CCyborgAssassin::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCyborgAssassin::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Assassin"),
		TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
		return E_FAIL;

	CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

	aabbDesc.vExtents = { 0.5f, 0.8f, 0.5f };
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
	headDesc.vExtents = { 0.13f, 0.16f, 0.13f };
	headDesc.vCenter = { 0.f, 0.f, 0.f };
	headDesc.pOwner = this;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_Head"), reinterpret_cast<CComponent**>(&m_pHeadColliderCom), &headDesc))) {
		return E_FAIL;
	}

	m_fDealDistance = 10.f;

	m_iActionState = ASSASSIN_STATE::STATE_STAND;

	m_pModelCom->SetUp_Animation(Anim_State::Idle, true);

	m_iHP = 10;

	m_eUnitType = UNIT_Assassin;

	return S_OK;
}

void CCyborgAssassin::Priority_Update(_float fTimeDelta)
{
	__super::Priority_Update(fTimeDelta);
}

void CCyborgAssassin::Update(_float fTimeDelta)
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
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());
	_float4x4 rootMat = {};
	m_bAnimationFinished = m_pModelCom->Play_Animation(fTimeDelta, &rootMat, true);
}

void CCyborgAssassin::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

HRESULT CCyborgAssassin::Render()
{
	__super::Render();

	return S_OK;
}

_bool CCyborgAssassin::setIdle(_float fTimeDelta)
{
	m_pModelCom->SetUp_Animation(Anim_State::Idle, true, 0.f);

	if (!CUnitObject::find_Player(fTimeDelta)) { //false 리턴이 찾은거임
		m_iActionState ^= STATE_STAND;
		find_path();
	}


	return true;
}

_bool CCyborgAssassin::setDead(_float fTimeDelta)
{
	if (m_pModelCom->GetCurrentAnimIndex() != Anim_State::Death)
	{
		_float fRand = m_pGameInstance->Get_Random(0.f, 5.f);
		if (fRand < 1.f) {
			CSoundMgr::Get_Instance()->PlaySound(TEXT("OnDeath_1"), 1.f);
		}
		else if (fRand < 2.f) {
			CSoundMgr::Get_Instance()->PlaySound(TEXT("OnDeath_2"), 1.0f);
		}
		else if (fRand < 3.f) {
			CSoundMgr::Get_Instance()->PlaySound(TEXT("OnDeath_3"), 1.0f);
		}
		else if(fRand < 4.f){
			CSoundMgr::Get_Instance()->PlaySound(TEXT("OnDeath_4"), 1.0f);
		}
		else if(fRand < 5.f){
			CSoundMgr::Get_Instance()->PlaySound(TEXT("OnDeath_5"), 1.0f);
		}
		m_pModelCom->SetUp_Animation(Anim_State::Death, false);
	}
	_float4x4 rootTransMat = {};

	m_bAnimationFinished = m_pModelCom->Play_Animation(fTimeDelta, &rootTransMat, true);

	return true;
}

_bool CCyborgAssassin::turn_To_Player(_float fTimeDelta)
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
		m_pModelCom->SetUp_Animation(Anim_State::Turn_Right_0, false);
	}
	else if (5 < fDegree && fDegree < 180.f) {
		m_pModelCom->SetUp_Animation(Anim_State::Turn_Right_0, false);
	}

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
		if (isDirectlyLookingToPlayer && m_pNavigationComs[m_iCurrentLevel]->IsReservedEmpty()) {
			m_iActionState |= STATE_ATTACK;
		}
		else {
			//이제 걸을 시간
			m_iActionState |= STATE_WALK;
		}

		m_iActionState ^= STATE_TURN;
		return false;
	}
}

_bool CCyborgAssassin::shoot_Player(_float fTimeDelta)
{
	m_pModelCom->SetUp_Animation(Anim_State::Range_Attack);

	if (m_pModelCom->GetCurrentAnimIndex() == Anim_State::Range_Attack
		&& m_bAnimationFinished == false) {
		m_vShootDuration.x += fTimeDelta;
		
		if (m_vShootDuration.x < m_vShootDuration.y) {
			m_vShootDuration.w += fTimeDelta;
			if(m_vShootDuration.w > m_vShootDuration.y / 3)
			{
				CBullet::Ammunition_Desc desc = {};
				desc.bShot = true; desc.bTargetSet = false; desc.fRotationPerSec = 1.f; desc.fSpeedPerSec = 70.f;
				desc.iDamage = 1.f; desc.pShooter = this; XMStoreFloat4x4(&desc.transMat, XMMatrixIdentity());
				
				auto rifleMat = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformMatrix_Ptr("rifle"));
				
				_vector position = XMVector3TransformCoord(rifleMat.r[3], m_pTransformCom->GetWorldMat_XMMat());
				position += m_pTransformCom->Get_State(CTransform::STATE_UP) * 0.1f + m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 0.7f + m_pTransformCom->Get_State(CTransform::STATE_RIGHT) * 0.03f;

				XMStoreFloat3((_float3*)(&desc.transMat.m[3]), position); desc.bTargetSet = false;
				XMStoreFloat3(&desc.vTargetPos, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

				CEffect_Muzzle::MUZZLE_DESC mDesc = {};
				mDesc.fRotationPerSec = 1.f; mDesc.fSpeedPerSec = 1.f; mDesc.fScale = 0.3f;
				mDesc.pOwner = this; mDesc.transMat = desc.transMat;

				_float fRand = m_pGameInstance->Get_Random(0.f, 4.f);
				if (fRand < 1.f) {
					CSoundMgr::Get_Instance()->PlaySound(TEXT("Fire_Close_1"), 1.f);
				}
				if (fRand < 2.f) {
					CSoundMgr::Get_Instance()->PlaySound(TEXT("Fire_Close_2"), 1.f);
				}
				if (fRand < 3.f) {
					CSoundMgr::Get_Instance()->PlaySound(TEXT("Fire_Close_3"), 1.f);
				}
				if (fRand < 4.f) {
					CSoundMgr::Get_Instance()->PlaySound(TEXT("Fire_Close_4"), 1.f);
				}
				m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FlyingBullet"), TEXT("Prototype_GameObject_Bullet"), &desc);
				m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Muzzle"), &mDesc);
				m_vShootDuration.w = 0.f;
			}
		}
	}
	if (m_pModelCom->GetCurrentAnimIndex() == Anim_State::Range_Attack
		&& m_bAnimationFinished == true) {
		m_vShootDuration.x = 0.f;
	}
	return _bool();
}

_bool CCyborgAssassin::attack_normal(_float fTimeDelta)
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

		// 각도 안맞아도 쏘긴 쏜다.
		shoot_Player(fTimeDelta);
	}

	// 플레이어가 움직여 턴 해야할때
	else {
		if(fDegree != 0.f && !isnan(fDegree))
		{
			_vector newVec = XMVectorLerp(vLook, vDir, m_pTransformCom->GetRotationPerSec() * fTimeDelta / fDegree);
			m_pTransformCom->SetLookToRightUp_ground(newVec);
		}

		// 플레이어가 사정거리 밖으로 나가거나 벽 뒤로 갈때
		// 바로 턴으로 돌리지 말고 공격 끝났다가 해라
		if (XMVectorGetX(XMVector3Length(vDir)) >= m_fDealDistance  || TileBetweenMeAndPlayer()) {
			// 새로 길찾기
			
			find_path();
			m_iActionState ^= STATE_ATTACK;
		}
		else {
			shoot_Player(fTimeDelta);
		}
	}

	return _bool();
}

void CCyborgAssassin::find_path()
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

_bool CCyborgAssassin::move_By_AStar(_float fTimeDelta)
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
		
		if(fDegree != 0.f && !isnan(fDegree))
		{
			_vector newVec = XMVectorLerp(vLook, vDir, m_pTransformCom->GetRotationPerSec() * fTimeDelta / fDegree);
			
			m_pTransformCom->SetLookToRightUp_ground(newVec);
		}
		
	}

	// 걷기 실행
	// 서로 벽 있는 것도 조건에 추가해야한다.
	if (XMVectorGetX(XMVector3Length(destPos - currentPos)) > m_fDealDistance || TileBetweenMeAndPlayer())
	{
		m_pModelCom->SetUp_Animation(Anim_State::Sprint_Front, true, 1.5f);

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
		m_iActionState ^= STATE_WALK;
		m_iActionState |= STATE_ATTACK;
		
	}

	return _bool();
}

_bool CCyborgAssassin::find_Player(_float fTimeDelta)
{
	return _bool();
}


CCyborgAssassin* CCyborgAssassin::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCyborgAssassin* pInstance = new CCyborgAssassin(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(TEXT("Failed to Created : CCyborgAssassin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCyborgAssassin::Clone(void* pArg)
{
	CCyborgAssassin* pInstance = new CCyborgAssassin(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCyborgAssassin"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCyborgAssassin::Free()
{
	__super::Free();
}
