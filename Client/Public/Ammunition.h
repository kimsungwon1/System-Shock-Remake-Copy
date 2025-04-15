#pragma once

#include "Item.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)

class CAmmunition : public CItem
{
public:
	struct Ammunition_Desc : CGameObject::GAMEOBJECT_DESC {
		_bool bShot;
		_bool bTargetSet = true;
		// bTargetSet ÀÌ false¸é vTargetPos ´Â look.
		_float3 vTargetPos;
		_int iDamage = 0;
		const class CGameObject* pShooter = nullptr;
	};
	enum Ammunition_Type {
		TYPE_762mm,
		TYPE_9mm,
		TYPE_ROCKET,
		TYPE_GRENADE,
		TYPE_END
	};
protected:
	CAmmunition(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAmmunition(const CAmmunition& Prototype);
	virtual ~CAmmunition() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	virtual _bool IsCollideAndHurt(_float fTimeDelta);
protected:
	_bool m_bShot = false;
	Ammunition_Type m_eAmmunitionType = {};
	_int m_iDamage = {};
	_float2 m_vBlowTime = { 0.f, 3.f };

	const class CGameObject* m_pShooter = nullptr;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END