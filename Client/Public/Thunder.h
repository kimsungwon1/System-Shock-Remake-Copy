#pragma once
#include "Ammunition.h"

BEGIN(Client)

class CThunder : public CAmmunition
{
private:
	CThunder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CThunder(const CThunder& Prototype);
	virtual ~CThunder() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Ready_Components();
	// 어떤 주기로 회전하는지

	_bool IsCollideAndHurt(_float fTimeDelta) override;
private:
	_float2 m_vDamageSchedule = { 0.f, 0.2f };
	_float m_fTime = 0.f;
public:
	static CThunder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END