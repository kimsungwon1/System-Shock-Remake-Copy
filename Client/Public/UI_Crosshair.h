#pragma once
#include "UIObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CUI_Crosshair : public CUIObject
{
private:
	CUI_Crosshair(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Crosshair(const CUI_Crosshair& Prototype);
	virtual ~CUI_Crosshair() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Ready_Components();
public:
	static CUI_Crosshair* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END