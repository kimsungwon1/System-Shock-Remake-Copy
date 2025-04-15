#pragma once
#include "AmmunitionPack.h"
class CPistolBulletPack : public CAmmunitionPack
{
private:
	CPistolBulletPack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPistolBulletPack(const CPistolBulletPack& Prototype);
	virtual ~CPistolBulletPack() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Ready_Component();

public:
	static CPistolBulletPack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

