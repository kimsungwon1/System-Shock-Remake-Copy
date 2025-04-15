#pragma once
#include "UIObject.h"
#include "Client_Defines.h"

BEGIN(Client)

class CButton :
    public CUIObject
{
public:
	struct BUTTON_DESC : UI_DESC {
		_wstring strQuote = {};
	};

private:
	CButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CButton(const CButton& Prototype);
	virtual ~CButton() = default;

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

	_wstring m_strQuote = {};
public:
	static CButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END