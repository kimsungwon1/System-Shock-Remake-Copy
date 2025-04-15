#pragma once
#include "Ammunition.h"
class CFireball :
    public CAmmunition
{
private:
	CFireball(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFireball(const CFireball& Prototype);
	virtual ~CFireball() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	HRESULT Ready_Components();
public:
	static CFireball* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

