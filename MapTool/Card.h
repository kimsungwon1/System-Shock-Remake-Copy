#pragma once
#include "Tile.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
class CCollider;
END

class CCard : public CTile
{
private:
	CCard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szLayerTag);
	CCard(const CCard& Prototype);
	virtual ~CCard() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_Data(_int iData) override {
		m_iData = iData;
	}
	_int Get_Data() const override {
		return m_iData;
	}

	HRESULT SaveObject(ofstream& saveStream) const override;
	HRESULT LoadObject(ifstream& loadStream) override;

private:
	_int m_iData = {};
	
	HRESULT Ready_Components();
	
public:
	static CCard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szLayerTag);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

