#pragma once
#include "UIObject.h"
class CUI_InvenSlot : public CUIObject
{
private:
	CUI_InvenSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_InvenSlot(const CUI_InvenSlot& Prototype);
	virtual ~CUI_InvenSlot() = default;

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
	static CUI_InvenSlot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

