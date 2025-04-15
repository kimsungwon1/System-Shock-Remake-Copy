#include "stdafx.h"
#include "UnitObject.h"

#include "Shader.h"
#include "Model.h"
#include "Navigation.h"
#include "Collider.h"
#include "Physics.h"

#include "Effect_BloodHit.h"

#include "FPSPlayer.h"
#include "TileObject.h"
#include "GameInstance.h"

bool compare_collider_under(const CCollider* first, const CCollider* second)
{
	return first->Get_Center()->x < second->Get_Center()->x;
}

bool compare_collider_up(const CCollider* first, const CCollider* second)
{
	return first->Get_Center()->x >= second->Get_Center()->x;
}

CUnitObject::CUnitObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CContainObject(pDevice, pContext)
{
}

CUnitObject::CUnitObject(const CUnitObject& Prototype)
	: CContainObject(Prototype)
{
}

HRESULT CUnitObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CUnitObject::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg))) {
		return E_FAIL;
	}
	auto* pDesc = static_cast<CGameObject::GAMEOBJECT_DESC*>(pArg);

	CNavigation::NAVIGATION_DESC			NaviDesc{};

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation1"),
		TEXT("Com_Navigation1"), reinterpret_cast<CComponent**>(&m_pNavigationComs[LEVEL1]), &NaviDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation2"),
		TEXT("Com_Navigation2"), reinterpret_cast<CComponent**>(&m_pNavigationComs[LEVEL2]), &NaviDesc)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation3"),
		TEXT("Com_Navigation3"), reinterpret_cast<CComponent**>(&m_pNavigationComs[LEVEL_BOSS]), &NaviDesc)))
		return E_FAIL;
	

	m_iCurrentLevel = pDesc->iCurrentLevel;

	m_pNavigationComs[m_iCurrentLevel]->SetCurrentIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pColliderCom);

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physics"),
		TEXT("Com_Physics"), reinterpret_cast<CComponent**>(&m_pPhysicsCom)))) {
		return E_FAIL;
	}

	m_fFrustumRadius = 3.f;

	return S_OK;
}

void CUnitObject::Priority_Update(_float fTimeDelta)
{
	if (m_pPhysicsCom->IsCollided()) {
		// 너무 세게 부딫치면 데미지

		m_pPhysicsCom->PopPower(4);
	}
	//__super::Update(fTimeDelta);
	m_pTransformCom->Bind_Physics(fTimeDelta, m_pPhysicsCom, m_pNavigationComs[m_iCurrentLevel],
		nullptr);
	if (m_iHP <= 0) {
		m_bDead = true;
	}
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

	if (m_pHeadColliderCom != nullptr) {
		auto headBoneMat = XMLoadFloat4x4(m_pModelCom->Get_BoneCombinedTransformMatrix_Ptr("head"));

		_float4x4 headBone4x4 = {};

		for (size_t i = 0; i < 3; i++) {
			headBoneMat.r[i] = XMVector3Normalize(headBoneMat.r[i]);
		}

		XMStoreFloat4x4(&headBone4x4, headBoneMat * m_pTransformCom->GetWorldMat_XMMat());
		m_pHeadColliderCom->Update(&headBone4x4);
	}
}

void CUnitObject::Update(_float fTimeDelta)
{
	if (m_bDead) {
		m_bDead = setDead(fTimeDelta);
		return;
	}
	//애니메이션이 끝나면 항상 Idle 애니메이션을 건다.
	if (m_bAnimationFinished) {
		setIdle(fTimeDelta);
	}

	if (m_bFinding_Player) {
		m_bFinding_Player = find_Player(fTimeDelta);
	}
	if (m_bTurning_To_Player) {
		m_bTurning_To_Player = turn_To_Player(fTimeDelta);
	}
	if (m_bMoving_By_AStar) {
		m_bMoving_By_AStar = move_By_AStar(fTimeDelta);
	}
	// 콜라이더 업데이트
	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

	_float4x4 rootTransMat = {};
	// 모델 애니메이션 실행, 끝 여부를 판단
	m_bAnimationFinished = m_pModelCom->Play_Animation(fTimeDelta, &rootTransMat, true);
	_matrix rootTransReal = XMLoadFloat4x4(&rootTransMat);

	if (m_bAnimationFinished) {
		auto scaled = m_pTransformCom->Get_Scaled();
		rootTransReal.r[0] = XMVector3Normalize(rootTransReal.r[0]) * scaled.x;
		rootTransReal.r[1] = XMVector3Normalize(rootTransReal.r[1]) * scaled.y;
		rootTransReal.r[2] = XMVector3Normalize(rootTransReal.r[2]) * scaled.z;

		rootTransReal = rootTransReal * m_pTransformCom->GetWorldMat_XMMat();
		//m_pTransformCom->SetLookToRightUp(rootTransReal.r[CTransform::STATE_LOOK]);
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, rootTransReal.r[3]);

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, rootTransReal.r[3]);
	}

}

void CUnitObject::Late_Update(_float fTimeDelta)
{
	if (m_pGameInstance->IsIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fFrustumRadius))
	{
		m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
	}
}

HRESULT CUnitObject::Render()
{
#ifdef _DEBUG
	m_pColliderCom->Render();

	if (m_pHeadColliderCom != nullptr) {
		m_pHeadColliderCom->Render();
	}
#endif

	if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldMatrix_Ptr())))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (m_pModelCom->GetType() == CModel::TYPE_ANIM)
		{
			m_pModelCom->Bind_MeshBoneMatrices(m_pShaderCom, "g_BoneMatrices", i);
		}
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_DiffuseTexture", aiTextureType_DIFFUSE, i)))
			return E_FAIL;
		_int iShader = 1;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_NormalTexture", aiTextureType_NORMALS, i)))
			iShader = 0;
		if (m_bDead) {
			if(FAILED(m_pShaderCom->Bind_RawValue("g_fDissolveThreshold", &m_fDeadTime, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;
		}
		else {
			if (FAILED(m_pShaderCom->Begin(iShader)))
				return E_FAIL;
		}
		if (FAILED(m_pModelCom->Render(i)))
			return E_FAIL;
	}
	return S_OK;
}

_bool CUnitObject::IsPicking(_float3* pOut)
{
	return m_pColliderCom->IsPicking(m_pTransformCom->GetWorldMat(), pOut);
}

_bool CUnitObject::TileIntersectsWithTile(_fvector pos, _fvector dir, _float fDistance)
{
	auto* pList = CGameInstance::Get_Instance()->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Tile"));

	list<CGameObject*> tList = CGameInstance::Get_Instance()->GetList_AtOctree_ByRadius(LEVEL_GAMEPLAY, TEXT("Layer_Tile"), pos, fDistance);
	_float fDistanceWithThis = 0.f;
	_bool bIntersects = false;
	
	for (auto* pIter : tList) {
		auto* pTile = dynamic_cast<const CTileObject*>(pIter);
		bIntersects = pTile->LineIntersects(pos, dir, &fDistanceWithThis);
		if (bIntersects && fDistanceWithThis <= fDistance) {
			return true;
		}
		else if(bIntersects && fDistanceWithThis > fDistance){
			bIntersects = false;
		}
	}

	return false;
}

void CUnitObject::SetPositionNavi()
{
	m_pNavigationComs[m_iCurrentLevel]->SetCurrentIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pColliderCom);
}

_bool CUnitObject::TileBetweenMeAndPlayer()
{
	auto* pPlayer = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	_vector vDir = pPlayer->GetTransformCom()->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.1f, 0.f, 0.f) - (m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
	return TileIntersectsWithTile(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.1f, 0.f, 0.f),
		vDir, XMVectorGetX(XMVector3Length(vDir)));
}

_bool CUnitObject::move_By_AStar(_float fTimeDelta)
{
	_vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector destPos = XMLoadFloat3(&m_vDestPos);
	
	if (auto* pTarget = dynamic_cast<CFPSPlayer*>(m_pTargetObject)) {
		if (pTarget->IsMoving()) {
			XMStoreFloat3(&m_vDestPos, m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION));
			m_pNavigationComs[m_iCurrentLevel]->Set_FinalDest(m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION));
			m_pNavigationComs[m_iCurrentLevel]->AStar();
		}
	}

	if (m_pNavigationComs[m_iCurrentLevel]->IsReservedEmpty()) {
		m_pTransformCom->LookAt_ground(destPos);
		//look_across_collider(destPos);
	}
	else {
		//나중에 좀더 자연스럽게 턴 하도록 바꿔볼 것.
		if (IsDestinationDirectly()) {
			m_pTransformCom->LookAt_ground(destPos);
		}
		else {
			m_pTransformCom->LookAt_ground(m_pNavigationComs[m_iCurrentLevel]->Get_NextCellPos());
		}
	}
	if(XMVectorGetX(XMVector3Length(destPos - currentPos)) > m_fDealDistance)
	{
		_float3 fromNextToMine = {};

		_vector vMoved = m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &fromNextToMine, m_pColliderCom);


		if(fromNextToMine.x != 0.f || fromNextToMine.y != 0.f || fromNextToMine.z != 0.f)
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

		return true;
	}
	return false;
}

_bool CUnitObject::find_Player(_float fTimeDelta)
{
	auto* pPlayer = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Player"));

	_vector playerPosition = pPlayer->GetTransformCom()->Get_State(CTransform::STATE_POSITION);
	_float fDistance = XMVectorGetX(XMVector3Length(playerPosition - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	if (fDistance <= m_fSight && 
		!TileBetweenMeAndPlayer()) {
		m_pTargetObject = pPlayer;
		m_bTurning_To_Player = true;
		return false;
	}
	return true;
}

_bool CUnitObject::turn_To_Player(_float fTimeDelta)
{
	return false;
}

void CUnitObject::find_path()
{
	// 사거리 밖이거나, 사이에 벽이 있다면, AStar로 플레이어 앞으로 이동
	if (XMVectorGetX(XMVector3Length((m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)))) > m_fDealDistance)
	{
		XMStoreFloat3(&m_vDestPos, m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION));
		m_pNavigationComs[m_iCurrentLevel]->Set_FinalDest(m_pTargetObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION));
		m_pNavigationComs[m_iCurrentLevel]->AStar();
		m_bMoving_By_AStar = true;
	}
}

_bool CUnitObject::ThereIsAllyFront(_fvector vDir, _float fDistance)
{
	auto colliders = m_pNavigationComs[m_iCurrentLevel]->Get_ColliderBelonged();

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector v1 = {};
	_vector v2 = {};

	_vector c1 = {};
	_vector c2 = {};
	//좌 아래
	if (vDir.m128_f32[0] < 0 && vDir.m128_f32[2] < 0) {
		v1.m128_f32[0] = vPosition.m128_f32[0] + m_pColliderCom->Get_Extents()->x;
		v1.m128_f32[2] = vPosition.m128_f32[2] - m_pColliderCom->Get_Extents()->z;
		v2.m128_f32[0] = vPosition.m128_f32[0] - m_pColliderCom->Get_Extents()->x;
		v2.m128_f32[2] = vPosition.m128_f32[2] + m_pColliderCom->Get_Extents()->z;
	}
	//좌 위
	if (vDir.m128_f32[0] < 0 && vDir.m128_f32[2] >= 0) {
		v1.m128_f32[0] = vPosition.m128_f32[0] - m_pColliderCom->Get_Extents()->x;
		v1.m128_f32[2] = vPosition.m128_f32[2] - m_pColliderCom->Get_Extents()->z;
		v2.m128_f32[0] = vPosition.m128_f32[0] + m_pColliderCom->Get_Extents()->x;
		v2.m128_f32[2] = vPosition.m128_f32[2] + m_pColliderCom->Get_Extents()->z;
	}
	//우 위
	if (vDir.m128_f32[0] >= 0 && vDir.m128_f32[2] >= 0) {
		v1.m128_f32[0] = vPosition.m128_f32[0] - m_pColliderCom->Get_Extents()->x;
		v1.m128_f32[2] = vPosition.m128_f32[2] + m_pColliderCom->Get_Extents()->z;
		v2.m128_f32[0] = vPosition.m128_f32[0] + m_pColliderCom->Get_Extents()->x;
		v2.m128_f32[2] = vPosition.m128_f32[2] + m_pColliderCom->Get_Extents()->z;
	}
	if (vDir.m128_f32[0] >= 0 && vDir.m128_f32[2] < 0) {
		v1.m128_f32[0] = vPosition.m128_f32[0] + m_pColliderCom->Get_Extents()->x;
		v1.m128_f32[2] = vPosition.m128_f32[2] + m_pColliderCom->Get_Extents()->z;
		v2.m128_f32[0] = vPosition.m128_f32[0] - m_pColliderCom->Get_Extents()->x;
		v2.m128_f32[2] = vPosition.m128_f32[2] - m_pColliderCom->Get_Extents()->z;
	}
	for (auto* pCol : colliders) {
		if (pCol == nullptr || (_uint)pCol == 0xdddddddddddddddd || (_uint)pCol == 0xffffffffffffffff)
			continue;
		_float fDis = XMVectorGetX(XMVector3Length(XMLoadFloat3(pCol->Get_Center()) - vPosition));

		if (isinf(fDis)) {
			continue;
		}

		_float cosBehind = XMVectorGetX(XMVector3Dot(vDir, XMLoadFloat3(pCol->Get_Center()) - vPosition));

		if (pCol->IsPlayers() || cosBehind <= 0 || fDis > fDistance) {
			continue;
		}
		if (vDir.m128_f32[0] < 0 && vDir.m128_f32[2] < 0) {
			c1.m128_f32[0] = pCol->Get_Center()->x - pCol->Get_Extents()->x;
			c1.m128_f32[2] = pCol->Get_Center()->z + pCol->Get_Extents()->z;
			c2.m128_f32[0] = pCol->Get_Center()->x + pCol->Get_Extents()->x;
			c2.m128_f32[2] = pCol->Get_Center()->z - pCol->Get_Extents()->z;

			_float col1 = vDir.m128_f32[2] / vDir.m128_f32[0] * (c1.m128_f32[0] - v1.m128_f32[0]) + v1.m128_f32[2];
			_float col2 = vDir.m128_f32[2] / vDir.m128_f32[0] * (c2.m128_f32[0] - v2.m128_f32[0]) + v2.m128_f32[2];

			if (c1.m128_f32[2] > col1 && c2.m128_f32[2] < col2) {
				return true;
			}
		}
		//좌 위
		if (vDir.m128_f32[0] < 0 && vDir.m128_f32[2] >= 0) {
			c1.m128_f32[0] = pCol->Get_Center()->x + pCol->Get_Extents()->x;
			c1.m128_f32[2] = pCol->Get_Center()->z + pCol->Get_Extents()->z;
			c2.m128_f32[0] = pCol->Get_Center()->x - pCol->Get_Extents()->x;
			c2.m128_f32[2] = pCol->Get_Center()->z - pCol->Get_Extents()->z;

			_float col1 = vDir.m128_f32[2] / vDir.m128_f32[0] * (c1.m128_f32[0] - v1.m128_f32[0]) + v1.m128_f32[2];
			_float col2 = vDir.m128_f32[2] / vDir.m128_f32[0] * (c2.m128_f32[0] - v2.m128_f32[0]) + v2.m128_f32[2];

			if (c1.m128_f32[2] > col1 && c2.m128_f32[2] < col2) {
				return true;
			}
		}
		//우 위
		if (vDir.m128_f32[0] >= 0 && vDir.m128_f32[2] >= 0) {
			c1.m128_f32[0] = pCol->Get_Center()->x + pCol->Get_Extents()->x;
			c1.m128_f32[2] = pCol->Get_Center()->z - pCol->Get_Extents()->z;
			c2.m128_f32[0] = pCol->Get_Center()->x - pCol->Get_Extents()->x;
			c2.m128_f32[2] = pCol->Get_Center()->z + pCol->Get_Extents()->z;

			_float col1 = vDir.m128_f32[2] / vDir.m128_f32[0] * (c1.m128_f32[0] - v1.m128_f32[0]) + v1.m128_f32[2];
			_float col2 = vDir.m128_f32[2] / vDir.m128_f32[0] * (c2.m128_f32[0] - v2.m128_f32[0]) + v2.m128_f32[2];

			if (c1.m128_f32[2] < col1 && c2.m128_f32[2] > col2) {
				return true;
			}
		}
		if (vDir.m128_f32[0] >= 0 && vDir.m128_f32[2] < 0) {
			c1.m128_f32[0] = pCol->Get_Center()->x - pCol->Get_Extents()->x;
			c1.m128_f32[2] = pCol->Get_Center()->z - pCol->Get_Extents()->z;
			c2.m128_f32[0] = pCol->Get_Center()->x + pCol->Get_Extents()->x;
			c2.m128_f32[2] = pCol->Get_Center()->z + pCol->Get_Extents()->z;

			_float col1 = vDir.m128_f32[2] / vDir.m128_f32[0] * (c1.m128_f32[0] - v1.m128_f32[0]) + v1.m128_f32[2];
			_float col2 = vDir.m128_f32[2] / vDir.m128_f32[0] * (c2.m128_f32[0] - v2.m128_f32[0]) + v2.m128_f32[2];

			if (c1.m128_f32[2] < col1 && c2.m128_f32[2] > col2) {
				return true;
			}
		}
	}
	return false;
}

_bool CUnitObject::IsDestinationDirectly()
{
	_vector currentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector destPos = XMLoadFloat3(&m_vDestPos);

	auto nextLine = m_pNavigationComs[m_iCurrentLevel]->Get_NextLine();
	_vector v1 = XMLoadFloat3(&nextLine.first), v2 = XMLoadFloat3(&nextLine.second);
	_vector vLook = {};

	if (v1.m128_f32[0] > v2.m128_f32[0]) {
		auto tmp = v2;
		v2 = v1;
		v1 = tmp;
	}

	return isIntersect({ { v1.m128_f32[0], v1.m128_f32[2] }, {v2.m128_f32[0], v2.m128_f32[2]} },
		{ { currentPos.m128_f32[0], currentPos.m128_f32[2] }, { destPos.m128_f32[0], destPos.m128_f32[2]} });
}

void CUnitObject::Free()
{
	__super::Free();
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
	for (auto* pNavi : m_pNavigationComs) {
		Safe_Release(pNavi);
	}

	m_pGameInstance->Delete_Collider(m_pColliderCom);
	Safe_Release(m_pColliderCom);
	Safe_Release(m_pPhysicsCom);
	Safe_Release(m_pHeadColliderCom);
}
