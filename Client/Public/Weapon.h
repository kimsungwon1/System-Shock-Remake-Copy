#pragma once
#include "Item.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CModel;
class CContainObject;
class CCollider;
END

BEGIN(Client)

class CWeapon : public CItem
{
public:
    struct Weapon_DESC : CPartObject::PartObject_DESC {
        const _float4x4* pSocketTransform = nullptr;
    };

	

	enum InfoType {
		INFO_TYPE,
		INFO_LOADNUM,

		INFO_END
	};

protected:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& Prototype);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	
	virtual _float2 Shoot() { return m_vRecoil; }

public:
	_float GetAttackDuration() const { return m_fAttackDuration; }

	virtual _int GetInfo(const _int* arrIndices) const override;
	void Load(_int iBullets);

	_int Get_LoadNum() const { return m_iLoadNum; }
	_int Get_MaxLoadNum() const { return m_iMaxLoadNum; }

protected:
	_float2 m_vRecoil = {};
protected:
	_float m_fAttackDuration = {};
	_int m_iLoadNum = 0;
	_int m_iMaxLoadNum = 0;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END