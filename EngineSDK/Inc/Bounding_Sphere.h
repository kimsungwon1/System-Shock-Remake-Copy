#pragma once
#include "Bounding.h"

BEGIN(Engine)

class CBounding_Sphere : public CBounding
{
public:
	struct BOUNDING_SPHERE_DESC : public CBounding::BOUNDING_DESC
	{
		_float fRadius;
	};
private:
	CBounding_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_Sphere() = default;

public:
	const BoundingSphere* Get_Desc() const {
		return m_pBoundingDesc;
	}
public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc) override;
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch) override;
	
public:
	virtual _bool Intersect(CCollider::TYPE eColliderType, CBounding* pBounding) override;
	virtual const _float3& Get_Center() const override;
private:
	BoundingSphere* m_pBoundingDesc = nullptr;
public:
	static CBounding_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END