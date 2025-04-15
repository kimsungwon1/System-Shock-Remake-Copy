#include "Bounding_AABB.h"
#include "GameInstance.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CBounding { pDevice, pContext }
{
}

HRESULT CBounding_AABB::Initialize(CBounding::BOUNDING_DESC* pBoundingDesc)
{
    BOUNDING_AABB_DESC* pDesc = static_cast<BOUNDING_AABB_DESC*>(pBoundingDesc);

	m_pOriginalBoundingDesc = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
    m_pBoundingDesc = new BoundingBox(*m_pOriginalBoundingDesc);
	m_vCenter = pDesc->vCenter;
	m_vExtents = pDesc->vExtents;

    return S_OK;
}

void CBounding_AABB::Update(_fmatrix WorldMatrix)
{
	_matrix TransformMatrix = WorldMatrix;

	TransformMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[0]);
	TransformMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(WorldMatrix.r[1]);
	TransformMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(WorldMatrix.r[2]);

	m_pOriginalBoundingDesc->Transform(*m_pBoundingDesc, TransformMatrix);
}

HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
#ifdef _DEBUG
	DX::Draw(pBatch, *m_pBoundingDesc, m_isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));
#endif

	return S_OK;
}

_bool CBounding_AABB::IsPicking(const _float4x4& WorldMatrix, _float3* pOut)
{
	auto* pGameInstance = CGameInstance::Get_Instance();
	pGameInstance->Transform_MouseRay_ToLocalSpace(WorldMatrix);

	_float3 center = m_pBoundingDesc->Center;
	_float3 extents = m_pBoundingDesc->Extents;
	_float fMinX = center.x - extents.x; _float fMaxX = center.x + extents.x;
	_float fMinY = center.y - extents.y; _float fMaxY = center.y + extents.y;
	_float fMinZ = center.z - extents.z; _float fMaxZ = center.z + extents.z;

	//1
	if (pGameInstance->isPicked_InWorldSpace(
		XMVectorSet(fMinX, fMaxY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMaxY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMinZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	if (pGameInstance->isPicked_InWorldSpace(
		XMVectorSet(fMinX, fMaxY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMinZ, 0.f),
		XMVectorSet(fMinX, fMinY, fMinZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	//2
	if (pGameInstance->isPicked_InWorldSpace(
		XMVectorSet(fMinX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMinX, fMaxY, fMinZ, 0.f),
		XMVectorSet(fMinX, fMinY, fMinZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	if (pGameInstance->isPicked_InWorldSpace(
		XMVectorSet(fMinX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMinX, fMinY, fMinZ, 0.f),
		XMVectorSet(fMinX, fMinY, fMaxZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	//3
	if (pGameInstance->isPicked_InWorldSpace(
		XMVectorSet(fMinX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMaxX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMaxX, fMaxY, fMinZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	if (pGameInstance->isPicked_InWorldSpace(
		XMVectorSet(fMinX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMaxX, fMaxY, fMinZ, 0.f),
		XMVectorSet(fMinX, fMaxY, fMinZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	//4
	if (pGameInstance->isPicked_InWorldSpace(
		XMVectorSet(fMinX, fMinY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMaxZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	if (pGameInstance->isPicked_InWorldSpace(
		XMVectorSet(fMinX, fMinY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMaxZ, 0.f),
		XMVectorSet(fMinX, fMinY, fMaxZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	//5
	if (pGameInstance->isPicked_InWorldSpace(
		XMVectorSet(fMaxX, fMaxY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMaxZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	if (pGameInstance->isPicked_InWorldSpace(
		XMVectorSet(fMaxX, fMaxY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMaxZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMinZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	//6
	if (pGameInstance->isPicked_InWorldSpace(
		XMVectorSet(fMaxX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMinX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMinX, fMinY, fMaxZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	if (pGameInstance->isPicked_InWorldSpace(
		XMVectorSet(fMaxX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMinX, fMinY, fMaxZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMaxZ, 0.f),
		pOut
	)) {
		goto Compute_WorldPos;
	}
	return false;
Compute_WorldPos:
	XMStoreFloat3(pOut, XMVector3TransformCoord(XMLoadFloat3(pOut), XMLoadFloat4x4(&WorldMatrix)));
	return true;
}

_bool CBounding_AABB::Intersect(_fvector vPos, _fvector vDir, _float fDistance)
{
	_float3 center = m_pBoundingDesc->Center;
	_float3 extents = m_pBoundingDesc->Extents;
	_float fMinX = center.x - extents.x; _float fMaxX = center.x + extents.y;
	_float fMinY = center.y - extents.y; _float fMaxY = center.y + extents.y;
	_float fMinZ = center.z - extents.z; _float fMaxZ = center.z + extents.z;

	_vector vNorDir = XMVector3Normalize(vDir);
	_float fDis1 = {};
	_float fMinDis = 10000.f;
	_bool bIntersected = false;
	//1
	if (TriangleTests::Intersects(vPos, vNorDir,
		XMVectorSet(fMinX, fMaxY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMaxY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMinZ, 0.f),
		fDis1
	)) {
		if (fMinDis > fDis1) {
			fMinDis = fDis1;
		}
		bIntersected = true;
	}
	if (TriangleTests::Intersects(vPos, vNorDir,
		XMVectorSet(fMinX, fMaxY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMinZ, 0.f),
		XMVectorSet(fMinX, fMinY, fMinZ, 0.f),
		fDis1
	)) {
		if (fMinDis > fDis1) {
			fMinDis = fDis1;
		}
		bIntersected = true;
	}
	//2
	if (TriangleTests::Intersects(vPos, vNorDir,
		XMVectorSet(fMinX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMinX, fMaxY, fMinZ, 0.f),
		XMVectorSet(fMinX, fMinY, fMinZ, 0.f),
		fDis1
	)) {
		if (fMinDis > fDis1) {
			fMinDis = fDis1;
		}
		bIntersected = true;
	}
	if (TriangleTests::Intersects(vPos, vNorDir,
		XMVectorSet(fMinX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMinX, fMinY, fMinZ, 0.f),
		XMVectorSet(fMinX, fMinY, fMaxZ, 0.f),
		fDis1
	)) {
		if (fMinDis > fDis1) {
			fMinDis = fDis1;
		}
		bIntersected = true;
	}
	//3
	if (TriangleTests::Intersects(vPos, vNorDir,
		XMVectorSet(fMinX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMaxX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMaxX, fMaxY, fMinZ, 0.f),
		fDis1
	)) {
		if (fMinDis > fDis1) {
			fMinDis = fDis1;
		}
		bIntersected = true;
	}
	if (TriangleTests::Intersects(vPos, vNorDir,
		XMVectorSet(fMinX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMaxX, fMaxY, fMinZ, 0.f),
		XMVectorSet(fMinX, fMaxY, fMinZ, 0.f),
		fDis1
	)) {
		if (fMinDis > fDis1) {
			fMinDis = fDis1;
		}
		bIntersected = true;
	}
	//4
	if (TriangleTests::Intersects(vPos, vNorDir,
		XMVectorSet(fMinX, fMinY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMaxZ, 0.f),
		fDis1
	)) {
		if (fMinDis > fDis1) {
			fMinDis = fDis1;
		}
		bIntersected = true;
	}
	if (TriangleTests::Intersects(vPos, vNorDir,
		XMVectorSet(fMinX, fMinY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMaxZ, 0.f),
		XMVectorSet(fMinX, fMinY, fMaxZ, 0.f),
		fDis1
	)) {
		if (fMinDis > fDis1) {
			fMinDis = fDis1;
		}
		bIntersected = true;
	}
	//5
	if (TriangleTests::Intersects(vPos, vNorDir,
		XMVectorSet(fMaxX, fMaxY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMaxZ, 0.f),
		fDis1
	)) {
		if (fMinDis > fDis1) {
			fMinDis = fDis1;
		}
		bIntersected = true;
	}
	if (TriangleTests::Intersects(vPos, vNorDir,
		XMVectorSet(fMaxX, fMaxY, fMinZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMaxZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMinZ, 0.f),
		fDis1
	)) {
		if (fMinDis > fDis1) {
			fMinDis = fDis1;
		}
		bIntersected = true;
	}
	//6
	if (TriangleTests::Intersects(vPos, vNorDir,
		XMVectorSet(fMaxX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMinX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMinX, fMinY, fMaxZ, 0.f),
		fDis1
	)) {
		if (fMinDis > fDis1) {
			fMinDis = fDis1;
		}
		bIntersected = true;
	}
	if (TriangleTests::Intersects(vPos, vNorDir,
		XMVectorSet(fMaxX, fMaxY, fMaxZ, 0.f),
		XMVectorSet(fMinX, fMinY, fMaxZ, 0.f),
		XMVectorSet(fMaxX, fMinY, fMaxZ, 0.f),
		fDis1
	)) {
		if (fMinDis > fDis1) {
			fMinDis = fDis1;
		}
		bIntersected = true;
	}
	if (fMinDis < fDistance)
		return true;
	return false;
}

_bool CBounding_AABB::Intersect(CCollider::TYPE eColliderType, CBounding* pBounding)
{
	m_isColl = false;

	switch (eColliderType)
	{
	case CCollider::TYPE_AABB:
		m_isColl = m_pBoundingDesc->Intersects(*(dynamic_cast<CBounding_AABB*>(pBounding)->Get_Desc()));
		break;
	case CCollider::TYPE_OBB:
		m_isColl = m_pBoundingDesc->Intersects(*(dynamic_cast<CBounding_OBB*>(pBounding)->Get_Desc()));
		break;
	case CCollider::TYPE_SPHERE:
		m_isColl = m_pBoundingDesc->Intersects(*(dynamic_cast<CBounding_Sphere*>(pBounding)->Get_Desc()));
		break;
	}
	return m_isColl;
}

const _float3& CBounding_AABB::Get_Center() const
{
	return m_pBoundingDesc->Center;
}

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc)
{
	CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pBoundingDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CBounding_AABB"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_AABB::Free()
{
	__super::Free();
	
	Safe_Delete(m_pBoundingDesc);
	Safe_Delete(m_pOriginalBoundingDesc);
}
