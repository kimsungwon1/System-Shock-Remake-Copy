#pragma once
#include "Weapon.h"
class CRocketLauncher : public CWeapon
{
private:
	CRocketLauncher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRocketLauncher(const CWeapon& Prototype);
	virtual ~CRocketLauncher() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Set_Offset() override;

	virtual _float2 Shoot() override;
public:
	static CRocketLauncher* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

