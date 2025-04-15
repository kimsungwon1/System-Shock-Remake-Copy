#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
private:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& Prototype);
	virtual ~CCollider() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Update(const _float4x4* pWorldMatrix);

#ifdef _DEBUG
	virtual HRESULT Render() override;
#endif

public:
	_bool IsPicking(const _float4x4& WorldMatrix, _float3* pOut);
	_bool Intersect(CCollider* pTargetCollider, _float3* pIntersectMount = nullptr);
	_bool Intersect(_fvector vPos, _fvector vDir, _float fDistance);

	const _float3* Get_Extents() const;
	const _float3* Get_Center() const;
	_bool IsPlayers() const { return m_bIsPlayers; }
	const class CGameObject* Get_Owner() const { return m_pOwner; }
private:
	class CBounding* m_pBounding = nullptr;
	TYPE			m_eColliderType = {};
	_bool m_bIsPlayers = false;
	const class CGameObject* m_pOwner = nullptr;
#ifdef _DEBUG
private:
	PrimitiveBatch<VertexPositionColor>*	m_pBatch = nullptr;
	BasicEffect*							m_pEffect = nullptr;
	ID3D11InputLayout*						m_pInputLayout = nullptr;

#endif

public:
	static CCollider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eColliderType);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END
