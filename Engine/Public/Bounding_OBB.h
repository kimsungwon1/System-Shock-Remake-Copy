#pragma once
#include "Bounding.h"

BEGIN(Engine)

class CBounding_OBB final : public CBounding
{
public:
	struct BOUNDING_OBB_DESC : public CBounding::BOUNDING_DESC
	{
		_float3 vExtents;
		_float3 vAngles;
	};
private:
	CBounding_OBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_OBB() = default;

public:
	const BoundingOrientedBox* Get_Desc() const {
		return m_pBoundingDesc;
	}

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc) override;
	virtual void Update(_fmatrix WorldMatrix) override;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;

public:
	virtual _bool Intersect(CCollider::TYPE eColliderType, CBounding* pBounding) override;
	virtual const _float3& Get_Center() const override;
private:
	BoundingOrientedBox* m_pOriginalBoundingDesc = nullptr;
	BoundingOrientedBox* m_pBoundingDesc = nullptr;

public:
	static CBounding_OBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END