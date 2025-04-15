#pragma once
#include "UIObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CElevatorUI :
    public CUIObject
{
private:
	CElevatorUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CElevatorUI(const CElevatorUI& Prototype);
	virtual ~CElevatorUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:

	virtual HRESULT Ready_PartUI() override;
	HRESULT Ready_Components();

public:
	static CElevatorUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END