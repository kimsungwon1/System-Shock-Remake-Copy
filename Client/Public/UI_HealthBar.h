#pragma once

#include "UIObject.h"
#include "Client_Defines.h"

BEGIN(Client)
class CFPSPlayer;

class CUI_HealthBar : public CUIObject
{
private:
	CUI_HealthBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_HealthBar(const CUI_HealthBar& Prototype);
	virtual ~CUI_HealthBar() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Ready_Components();
	CFPSPlayer* m_pPlayer = nullptr;

public:
	static CUI_HealthBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END