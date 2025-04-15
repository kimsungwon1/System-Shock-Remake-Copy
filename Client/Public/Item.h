#pragma once
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
class CContainObject;
END

BEGIN(Client)

class CItem : public CPartObject
{
public:
	enum ItemType {
		TYPE_Pistol = 0,
		TYPE_AssaultRifle = 3,
		TYPE_RocketLauncher = 4,
		TYPE_Pipe,

		TYPE_KeyCard = 10,
		TYPE_Grenade = 11,
		TYPE_PistolBullets = 12,
		TYPE_RifleBullets = 13,
		TYPE_Rockets = 14,

		Item_END
	};
	static const _float s_fPickItemDistance;
	struct ITEM_DESC : CGameObject::GAMEOBJECT_DESC
	{
		const class CFPSPlayer* pOwner = nullptr;
	};

protected:
	CItem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CItem(const CItem& Prototype);
	virtual ~CItem() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();
public:
	_bool Is_Possible_Looted(const CContainObject* pInfluencer);
	_bool IsPicking();

	void Set_Influencer(const CContainObject* pInfluencer, const _float4x4* pParentMatrix, const _float4x4* pSocketMatrix);

	virtual void Set_Offset() {};
	ItemType GetItemType() const { return m_eItemType; }

	const _wstring& GetName() const { return m_strName; }
	void DeleteInfoUI();
protected:
	const CContainObject* m_pInfluencer = nullptr;

	const _float4x4* m_pSocketTransform = nullptr;

	CGameObject* m_pPickingUI = nullptr;

	_bool m_bOnInven = false;

	ItemType m_eItemType = ItemType::Item_END;

	_wstring m_strName = {};
public:
	_bool Is_OnInven() const { return m_bOnInven; }
	void Set_OnInven(_bool bOnInven) { m_bOnInven = bOnInven; }
	pair<int, int> GetSlotSize() const { return { m_iSlotSizeX, m_iSlotSizeY }; }
	void Set_PosInInven(_int x, _int y) { m_iPosInInvenX = x; m_iPosInInvenY = y; }
	pair<int, int> GetPosInInven() const { return { m_iPosInInvenX, m_iPosInInvenY }; }
protected:
	_int m_iSlotSizeX = 1; _int m_iSlotSizeY = 1;
	_int m_iPosInInvenX = 0; _int m_iPosInInvenY = 0;

	// 자식 클래스에서 정의
	CCollider* m_pColliderCom = nullptr;
	// 여기 클래스에서 정의
	CShader* m_pShader = nullptr;
	// 자식 클래스에서 정의
	CModel* m_pModel = nullptr;

	const class CFPSPlayer* m_pOwner = nullptr;
public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;

};

END