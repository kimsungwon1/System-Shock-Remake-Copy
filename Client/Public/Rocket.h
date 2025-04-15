#pragma once
#include "Ammunition.h"
#include "Client_Defines.h"

class CEffect_Blast;

BEGIN(Client)

class CRocket final : public CAmmunition
{
private:
	CRocket(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRocket(const CRocket& Prototype);
	virtual ~CRocket() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

private:
	_float m_fBlowDamage = 8.f;
	_float m_fBlowRange = 5.f;
	CGameObject* m_pBlastEffect = nullptr;
private:
	void blow(_float fTimeDelta);
	virtual _bool IsCollideAndHurt(_float fTimeDelta) override;
public:
	static CRocket* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END