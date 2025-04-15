#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_AABB final : public CBounding
{
public:
	struct BOUNDING_AABB_DESC : public CBounding::BOUNDING_DESC
	{
		_float3 vExtents;
	};
private:
	CBounding_AABB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_AABB() = default;

public:
	const BoundingBox* Get_Desc() const {
		return m_pBoundingDesc;
	}

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc) override;
	virtual void Update(_fmatrix WorldMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;

public:
	_bool IsPicking(const _float4x4& WorldMatrix, _float3* pOut);
	virtual _bool Intersect(CCollider::TYPE eColliderType, CBounding* pBounding) override;
	virtual _bool Intersect(_fvector vPos, _fvector vDir, _float fDistance);
	virtual const _float3& Get_Center() const override;
private:
	BoundingBox* m_pOriginalBoundingDesc = nullptr;
	BoundingBox* m_pBoundingDesc = nullptr;

public:
	static CBounding_AABB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END