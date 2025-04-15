#pragma once
#include "UIObject.h"

#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_InvenSlot;

END


BEGIN(Client)
class CUI_Item;
class CItem;
class CAmmunitionPack;

class CInventory : public CUIObject
{
	struct InvenMousePointer {
		CUI_Item* pItem = nullptr;
		_float2 vBeforePos;
	};

private:
	CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CInventory(const CInventory& ref);
	virtual ~CInventory() = default;

public:
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	//vector<_int> m_vecBulletNum = {100, 100, 100, 100, 100, 100, 100, 100, 100, 100};
	vector<CAmmunitionPack*> m_vecBulletPacks = {};
public:
	void PushBullet(_int eType);
	void PopBullet(_int eType);
	_int GetBulletNum(_int eType);
	void Set_InvenRender(_bool bInven);
	_bool Is_InvenRender() const { return m_bInvenRender; }

	_bool PushItem(CItem* pItem);
private:
	HRESULT Ready_Components();
	HRESULT Ready_PartUI() override;
private:
	class CShader* m_pShaderCom = { nullptr };
	_float2 m_arrUIPos[5][12] = {};
	_float2 m_arrWeaponUIPos[10] = {};

	_bool m_arrInvenFullByItem[5][12] = { false, };

	_bool m_bInvenRender = false;

	vector<CUI_Item*> m_vecItemOnUI = {};
	//class CTexture* m_pTextureCom = { nullptr };
	//CVIBuffer_Point_InvenSlot* m_pVIBufferPointCom = nullptr;
	InvenMousePointer m_MousePointer = {};
	_float2 m_vWeaponStartPos = {};
	_float2 m_vStartPos = {};
public:
	static CInventory* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject* Clone(void* pArg) override;
	void Free() override;
};

END