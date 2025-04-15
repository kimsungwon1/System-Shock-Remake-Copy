#pragma once
#include "Item.h"

BEGIN(Client)

class CCard : public CItem
{
public:
	struct CARD_DESC : CItem::ITEM_DESC {
		_int iData;
	};
private:
	CCard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCard(const CCard& Prototype);
	virtual ~CCard() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT SaveObject(ofstream& saveStream) const override;
	HRESULT LoadObject(ifstream& loadStream) override;

private:
	_int m_iData = { -1};

	HRESULT Ready_Components();

public:
	_int GetInfo(const _int* arrIndices) const override { return m_iData; }

public:
	static CCard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END