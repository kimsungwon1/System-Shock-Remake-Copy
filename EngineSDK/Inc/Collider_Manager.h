#pragma once
#include "Base.h"

BEGIN(Engine)

class CCollider_Manager : public CBase
{
private:
	CCollider_Manager() = default;
	~CCollider_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT	Add_Collider(class CCollider* pCollider);
	HRESULT Delete_Collider(class CCollider* pCollider);
	_bool	Intersects(CCollider* pCollider, _float3* pColliderCenterVec);

private:
	list<class CCollider*> m_Colliders = {};

public:
	static CCollider_Manager* Create();
	virtual void Free() override;
};

END