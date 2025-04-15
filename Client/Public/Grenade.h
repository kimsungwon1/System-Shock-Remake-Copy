#pragma once
#include "Ammunition.h"

BEGIN(Engine)

class CPhysics;
class CNavigation;

END

BEGIN(Client)

class CGrenade :
    public CAmmunition
{
public:
	struct Grenade_DESC : Ammunition_Desc {
		_float3 vThrowdir = {};
	};
private:
	CGrenade(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGrenade(const CGrenade& Prototype);
	virtual ~CGrenade() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float m_fBlowDamage = 10;
	_float m_fBlowRange = 5.f;

private:
	void Blow(_float fTimeDelta);

private:
	CPhysics* m_pPhysicsCom = nullptr;
	CNavigation* m_pNavigationComs[GAMEPLAY_LEVELID::L_END] = { nullptr, };

public:
	static CGrenade* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END