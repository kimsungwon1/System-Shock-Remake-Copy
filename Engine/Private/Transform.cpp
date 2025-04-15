#include "..\Public\Transform.h"
#include "Shader.h"
#include "Navigation.h"
#include "Collider.h"
#include "Physics.h"

#include "GameInstance.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent{ pDevice, pContext }
{
}

_float3 CTransform::Get_Scaled() const
{
	_matrix		WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	return _float3(XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_RIGHT])),
		XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_UP])),
		XMVectorGetX(XMVector3Length(WorldMatrix.r[STATE_LOOK])));
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Initialize(void* pArg)
{
	TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(pArg);

	m_fSpeedPerSec = pDesc->fSpeedPerSec;
	m_fRotationPerSec = pDesc->fRotationPerSec;

	return S_OK;
}

HRESULT CTransform::Bind_ShaderResource(CShader* pShader, const _char* pConstantName)
{
	return pShader->Bind_Matrix(pConstantName, &m_WorldMatrix);

}

_vector CTransform::Bind_Physics(_float fTimeDelta, CPhysics* pPhysics, CNavigation* pNavigation, CCollider* pMyCollider)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vPowerDir = pPhysics->GetPower();

	//vPowerDir = XMVectorSetW(XMVector3Normalize(vPowerDir) * m_fSpeedPerSec, vPowerDir.m128_f32[3]);

	if (vPowerDir.m128_f32[1] > 0 || pNavigation->IsFloating(vPosition)) {
		vPowerDir.m128_f32[1] -= vPowerDir.m128_f32[3] * CPhysics::s_fGravity;
	}

	vPosition += vPowerDir * fTimeDelta;

	_float4x4 postMatrix = m_WorldMatrix;
	postMatrix._41 = vPosition.m128_f32[0];
	postMatrix._42 = vPosition.m128_f32[1];
	postMatrix._43 = vPosition.m128_f32[2];

	if (pMyCollider != nullptr)
		pMyCollider->Update(&postMatrix);

	_float fy = {};
	_float3 vLine = {};
	if (nullptr == pNavigation || pNavigation->isMove(vPosition, Get_State(STATE_POSITION), &fy, pMyCollider, &vLine))
	{
		if (vPosition.m128_f32[1] < fy) {
			vPosition.m128_f32[1] = fy;
			if (vPowerDir.m128_f32[1] < 0) {
				pPhysics->SetCollided(true);
			}
		}
		_float3 centerDistance = {};
		if (nullptr == pMyCollider || !m_pGameInstance->Intersects(pMyCollider, &centerDistance))
		{
			Set_State(STATE_POSITION, vPosition);
		}
		else {
			pMyCollider->Update(&m_WorldMatrix);
			pPhysics->SetCollided(true);
		}
	}
	else if (nullptr != pNavigation) {
		pPhysics->SetCollided(true);
	}
	pPhysics->AddTime(fTimeDelta);

	return vPowerDir;
}

void CTransform::Set_Scaled(_float fX, _float fY, _float fZ)
{
	Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * fX);
	Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * fY);
	Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * fZ);
}

void CTransform::LookAt(_fvector vAt)
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = vAt - vPosition;

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Go_Straight(_float fTimeDelta, CNavigation* pNavigation, CCollider* pMyCollider)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	_float fy = {};
	_float3 vLine = {};
	if ((nullptr == pNavigation && nullptr == pMyCollider) || pNavigation->isMove(vPosition, Get_State(STATE_POSITION), &fy, pMyCollider, &vLine))
	{
		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Backward(_float fTimeDelta, CNavigation* pNavigation, CCollider* pMyCollider)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vLook = Get_State(STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * fTimeDelta;

	_float fy = {};
	_float3 vLine = {};
	if ((nullptr == pNavigation && nullptr == pMyCollider) || pNavigation->isMove(vPosition, Get_State(STATE_POSITION), &fy, pMyCollider, &vLine))
	{
		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Left(_float fTimeDelta, CNavigation* pNavigation, CCollider* pMyCollider)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	_float fy = {};
	_float3 vLine = {};
	if ((nullptr == pNavigation && nullptr == pMyCollider) || pNavigation->isMove(vPosition, Get_State(STATE_POSITION), &fy, pMyCollider, &vLine))
	{
		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Right(_float fTimeDelta, CNavigation* pNavigation, CCollider* pMyCollider)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vRight = Get_State(STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * fTimeDelta;

	_float fy = {};
	_float3 vLine = {};
	if ((nullptr == pNavigation && nullptr == pMyCollider) || pNavigation->isMove(vPosition, Get_State(STATE_POSITION), &fy, pMyCollider, &vLine))
	{
		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Up(_float fTimeDelta, CNavigation* pNavigation, CCollider* pMyCollider)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vUp = Get_State(STATE_UP);

	vPosition += XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	_float fy = {};
	_float3 vLine = {};
	if ((nullptr == pNavigation && nullptr == pMyCollider) || pNavigation->isMove(vPosition, Get_State(STATE_POSITION), &fy, pMyCollider, &vLine))
	{
		Set_State(STATE_POSITION, vPosition);
	}
}

void CTransform::Go_Down(_float fTimeDelta, CNavigation* pNavigation, CCollider* pMyCollider)
{
	_vector		vPosition = Get_State(STATE_POSITION);
	_vector		vUp = Get_State(STATE_UP);

	vPosition -= XMVector3Normalize(vUp) * m_fSpeedPerSec * fTimeDelta;

	_float fy = {};
	_float3 vLine = {};
	if ((nullptr == pNavigation && nullptr == pMyCollider) || pNavigation->isMove(vPosition, Get_State(STATE_POSITION), &fy, pMyCollider, &vLine))
	{
		Set_State(STATE_POSITION, vPosition);
	}
}

// iDir - 0 : 앞, 1 : 뒤, 2 : 좌, 3 : 우, 4: 앞-좌, 5: 앞-우, 6: 뒤-우, 7: 뒤-좌
_vector CTransform::Walk(_int iDir, _float fTimeDelta, CNavigation* pNavigation, _float3* vCenterDistance, class CCollider* pMyCollider)
{
	_vector vPosition = Get_State(STATE_POSITION);
	_vector vDir = {};
	auto vScale = Get_Scaled();

	_float len = {};
	switch (iDir) {
	case 0:
		vDir = Get_State(STATE_LOOK);
		len = XMVectorGetX(XMVector3Length(vDir));
		vDir.m128_f32[1] = 0;
		vDir = XMVector3Normalize(vDir) * len;

		break;
	case 1:
		vDir = -Get_State(STATE_LOOK);
		len = XMVectorGetX(XMVector3Length(vDir));
		vDir.m128_f32[1] = 0;
		vDir = XMVector3Normalize(vDir) * len;
		break;
	case 2:
		vDir = -Get_State(STATE_RIGHT);
		break;
	case 3:
		vDir = Get_State(STATE_RIGHT);
		break;
	case 4:
		vDir = Get_State(STATE_LOOK);
		len = XMVectorGetX(XMVector3Length(vDir));
		vDir.m128_f32[1] = 0;
		vDir = XMVector3Normalize(vDir) * len;
		vDir = (vScale.z * vScale.z + vScale.x * vScale.x) * 0.5f * XMVector3Normalize(vDir - Get_State(STATE_RIGHT));
		break;
	case 5:
		vDir = Get_State(STATE_LOOK);
		len = XMVectorGetX(XMVector3Length(vDir));
		vDir.m128_f32[1] = 0;
		vDir = XMVector3Normalize(vDir) * len;
		vDir = (vScale.z * vScale.z + vScale.x * vScale.x) * 0.5f * XMVector3Normalize(vDir + Get_State(STATE_RIGHT));
		break;
	case 6:
		vDir = Get_State(STATE_LOOK);
		len = XMVectorGetX(XMVector3Length(vDir));
		vDir.m128_f32[1] = 0;
		vDir = XMVector3Normalize(vDir) * len;
		vDir = (vScale.z * vScale.z + vScale.x * vScale.x) * 0.5f * XMVector3Normalize(-vDir + Get_State(STATE_RIGHT));
		break;
	case 7 :
		vDir = Get_State(STATE_LOOK);
		len = XMVectorGetX(XMVector3Length(vDir));
		vDir.m128_f32[1] = 0;
		vDir = XMVector3Normalize(vDir) * len;
		vDir = (vScale.z * vScale.z + vScale.x * vScale.x) * 0.5f * XMVector3Normalize(-vDir - Get_State(STATE_RIGHT));
		break;
	default:
		return vDir;
		break;
	}
	_float originalHeight = {};



	if (pNavigation != nullptr)
	{
		originalHeight = pNavigation->Get_Height(vPosition);
	}
	_float widthDividesHeight = {};

	_vector vD = XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;
	vPosition += vD;

	_float4x4 postMatrix = m_WorldMatrix;
	postMatrix._41 = vPosition.m128_f32[0];
	postMatrix._42 = vPosition.m128_f32[1];
	postMatrix._43 = vPosition.m128_f32[2];
	pMyCollider->Update(&postMatrix);

	_float fy = vPosition.m128_f32[1];
	_float3 vLine = {};
	if ((nullptr == pNavigation && nullptr == pMyCollider) || pNavigation->isMove(vPosition, Get_State(STATE_POSITION), &fy, pMyCollider, &vLine))
	{
		if (vPosition.m128_f32[1] < fy) {
			vPosition.m128_f32[1] = fy;
		}
		if (nullptr != pMyCollider && !m_pGameInstance->Intersects(pMyCollider, vCenterDistance))
		{
			widthDividesHeight = (fy - originalHeight) / XMVectorGetX(XMVector3Length(vD));
			Set_State(STATE_POSITION, vPosition);
		}
		else {
			pMyCollider->Update(&m_WorldMatrix);
		}
	}
	else if (nullptr != pNavigation) {
		// 슬라이딩
		vLine.y = 0.f;
		_float moveMount = XMVectorGetX(XMVector3Dot(vD, XMLoadFloat3(&vLine))) / XMVectorGetX(XMVector3Length(XMLoadFloat3(&vLine)));
		_vector vMove = Get_State(STATE_POSITION) + moveMount * XMVector3Normalize(XMLoadFloat3(&vLine));
		if (pNavigation->isMove(vMove, Get_State(STATE_POSITION), &fy, pMyCollider, &vLine)) {
			widthDividesHeight = (fy - originalHeight) / XMVectorGetX(XMVector3Length(vD));
			Set_State(STATE_POSITION, vMove);
		}
		//슬라이딩도 통과 X->안움직임

	}
	////만약 네비 각도가 45도를 넘는다? 미끄러진다.
	//if (pNavigation != nullptr && widthDividesHeight > 1) {
	//	_vector vNormal = XMVector3Normalize(pNavigation->Get_CurrentCell_Normal());
	//	_float dot = XMVectorGetX(XMVector3Dot(vNormal, XMVectorSet(0.f, -CPhysics::s_fGravity, 0.f, 0.f)));
	//	_vector slideToUnder = vNormal * abs(dot) + XMVectorSet(0.f, -CPhysics::s_fGravity, 0.f, 0.f);
	//	Set_State(STATE_POSITION, Get_State(STATE_POSITION) + slideToUnder);
	//}

	return XMVector3Normalize(vDir) * m_fSpeedPerSec * fTimeDelta;
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_fRotationPerSec * fTimeDelta);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Turn(_bool isRotationX, _bool isRotationY, _bool isRotationZ, _float fTimeDelta)
{
	_vector		vRotation = XMQuaternionRotationRollPitchYaw(m_fRotationPerSec * fTimeDelta * isRotationX,
		m_fRotationPerSec * fTimeDelta * isRotationY,
		m_fRotationPerSec * fTimeDelta * isRotationZ);

	_vector		vRight = Get_State(STATE_RIGHT);
	_vector		vUp = Get_State(STATE_UP);
	_vector		vLook = Get_State(STATE_LOOK);

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_float fX, _float fY, _float fZ, _float fRadian)
{
	_vector		vRotation = XMQuaternionRotationRollPitchYaw(fX, fY, fZ);

	_float3		vScaled = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScaled.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScaled.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScaled.z;

	_matrix		RotationMatrix = XMMatrixRotationQuaternion(vRotation);

	Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::SetLookToRightUp_ground(_fvector vLook)
{
	_float3 vScaled = Get_Scaled();

	_vector vNewLook{}, vRight{}, vUp{};

	vNewLook = vLook;
	_float	len = XMVectorGetX(XMVector3Length(vNewLook));
	vNewLook.m128_f32[1] = 0;
	vNewLook = XMVector3Normalize(vNewLook) * len;

	vRight = XMVector3Cross(_vector{ 0.f, 1.f,0.f }, vNewLook);
	vUp = XMVector3Cross(vNewLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vNewLook) * vScaled.z);
}

void CTransform::SetLookToRightUp(_fvector vLook)
{
	_float3 vScaled = Get_Scaled();

	_vector vRight{}, vUp{};

	vRight = XMVector3Cross(_vector{ 0.f, 1.f,0.f }, vLook);
	vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScaled.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScaled.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScaled.z);
}

void CTransform::LookAt_ground(_fvector vAt)
{
	_float3		vScale = Get_Scaled();

	_vector		vPosition = Get_State(STATE_POSITION);

	_vector		vLook = vAt - vPosition;

	_float	len = XMVectorGetX(XMVector3Length(vLook));
	vLook.m128_f32[1] = 0;
	vLook = XMVector3Normalize(vLook) * len;

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
	CTransform* pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}


	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created : CTransform"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
