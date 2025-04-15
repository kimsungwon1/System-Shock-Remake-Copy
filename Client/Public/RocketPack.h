#pragma once
#include "AmmunitionPack.h"
class CRocketPack : public CAmmunitionPack
{
private:
	CRocketPack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRocketPack(const CRocketPack& Prototype);
	virtual ~CRocketPack() = default;

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
	static CRocketPack* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

