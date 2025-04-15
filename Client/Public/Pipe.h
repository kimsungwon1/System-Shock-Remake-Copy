#pragma once
#include "Weapon.h"

BEGIN(Engine)
class CShader;
class CModel;
END

class CPipe final : public CWeapon
{
private:
	CPipe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPipe(const CWeapon& Prototype);
	virtual ~CPipe() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	static CPipe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};