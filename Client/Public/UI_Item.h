#pragma once
#include "UIObject.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)
class CItem;

class CUI_Item :
    public CUIObject
{
public:
	struct ITEMSLOT : CUIObject::UI_DESC {
		_int iTextureIndex = 0;
		CItem* pItem = nullptr;
	};
private:
	CUI_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI_Item(const CUI_Item& Prototype);
	virtual ~CUI_Item() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;
public:
	void Set_CurTextureIndex(_int i) { m_iCurTextureIndex = i; }
	_bool IsSameItem(CItem* pItem);
	pair<int, int> GetSlotSize() const;
	void Set_PosInInven(_int x, _int y);
	CItem* GetItem() { return m_pOwnerItem; }

protected:
	HRESULT Ready_Components();
	CTexture* m_pTextureComArr[15] = {nullptr,};
	_int m_iCurTextureIndex = 0;
	CItem* m_pOwnerItem = nullptr;
	//CFPSPlayer* m_pPlayer = nullptr;

public:
	static CUI_Item* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END