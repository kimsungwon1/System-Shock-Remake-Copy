#pragma once
#include "AmmunitionPack.h"
class CRifleBulletPack : public CAmmunitionPack
{
private:
	CRifleBulletPack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRifleBulletPack(const CRifleBulletPack& Prototype);
	virtual ~CRifleBulletPack() = default;

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
	static CRifleBulletPack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

