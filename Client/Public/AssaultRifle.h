#pragma once
#include "Weapon.h"

BEGIN(Engine)
class CShader;
class CModel;
END

class CAssaultRifle final : public CWeapon
{
private:
	CAssaultRifle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAssaultRifle(const CWeapon& Prototype);
	virtual ~CAssaultRifle() = default;

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
	static CAssaultRifle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

