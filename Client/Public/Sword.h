#pragma once
#include "Weapon.h"
class CSword : public CWeapon
{
private:
	CSword(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSword(const CSword& Prototype);
	virtual ~CSword() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

protected:

public:
	static CSword* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

