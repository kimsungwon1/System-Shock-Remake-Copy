#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CPhysics : public CComponent
{
private:
	CPhysics(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPhysics(const CPhysics& Prototype);
	virtual ~CPhysics() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;

public:
	void Update(_float fTimeDelta);

	_vector GetPower() const { return XMLoadFloat4(&m_vPower); }

	void ReceivePowerVector(_fvector vPower) { XMStoreFloat4(&m_vPower, XMLoadFloat4(&m_vPower) + vPower); m_bCollided = false; }
	void AddTime(_float fTimeDelta) { m_vPower.w += fTimeDelta; }
	void PopPower(_int i);

	void SetCollided(_bool isCollided) { m_bCollided = isCollided; }
	_bool IsCollided() const { return m_bCollided; }
private:
	// 4번째는 합친 시간
	_float4 m_vPower = {0.f, 0.f, 0.f, 0.f};
	_bool m_bCollided = false;
public:
	static const _float s_fGravity;
public:
	static CPhysics* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END