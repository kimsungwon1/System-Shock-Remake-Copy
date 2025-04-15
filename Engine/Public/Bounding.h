#pragma once
#include "Base.h"
#include "DebugDraw.h"
#include "Collider.h"

BEGIN(Engine)

class CBounding abstract : public CBase
{
public:
	struct BOUNDING_DESC 
	{
		_float3 vCenter;
		_bool bIsPlayers = false;
		const class CGameObject* pOwner = nullptr;
	};
protected:
	CBounding(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding() = default;

public:
	virtual HRESULT	Initialize(CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Update(_fmatrix WorldMatrix);
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch);

	virtual _bool IsPicking(const _float4x4& WorldMatrix, _float3* pOut) { return false; }

public:
	virtual _bool Intersect(CCollider::TYPE eColliderType, CBounding* pBounding) = 0;


	virtual _bool Intersect(_fvector vPos, _fvector vDir, _float fDistance) { return false; };
	virtual const _float3& Get_Center() const = 0;
	virtual const _float3& Get_vExtents() const { return m_vExtents; }
protected:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	_float3 m_vCenter = {};
	_float3 m_vExtents = {};
	_bool	m_isColl = false;
public:
	virtual void Free() override;
};

END
