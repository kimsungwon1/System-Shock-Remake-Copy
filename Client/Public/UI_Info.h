#pragma once
#include "UIObject.h"

BEGIN(Client)

class CUI_Info : public CUIObject
{
public:
	//fLifeTime.x == -1이면 사라지지 않기로
	struct INFO_DESCS : UI_DESC {
		_wstring strQuote = {};
		_float fLifeTime = 5.f;
	};
private:
	CUI_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Info(const CUI_Info& Prototype);
	virtual ~CUI_Info() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

protected:
	HRESULT Ready_Components();
	_wstring m_strText = {};

	_float2 m_fLifeTime = { 0.f, 5.f };
	_float2 m_fFadeTime = { 0.f, 0.5f };
public:
	static CUI_Info* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END