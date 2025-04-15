#pragma once
#include "Interactives.h"
class CElevator : public CInteractives
{
private:
	CElevator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElevator(const CElevator& Prototype);
	virtual ~CElevator() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;

private:
	HRESULT Ready_Components();

public:
	static CElevator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

