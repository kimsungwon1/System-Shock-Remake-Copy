#pragma once
#include "Tile.h"
class CLightObject :
    public CTile
{
public:
	struct LIGHTOBJECT_DESC : CToolObject::TOOLOBJECT_DESC {
		LIGHT_DESC light_description = {};
	};
private:
	CLightObject(ID3D11Device* pDevice,
		ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szLayerTag);
	CLightObject(const CLightObject& Prototype);
	~CLightObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	void Priority_Update(_float fTimeDelta) override;
	void Update(_float fTimeDelta) override;
	void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool IsPicking(_float3* pOut) override;
public:
	HRESULT SaveObject(ofstream& saveStream) const override;
	HRESULT LoadObject(ifstream& loadStream) override;

public:
	HRESULT Set_Desc(LIGHT_DESC& desc);
	const LIGHT_DESC& Get_Desc() const { return m_Light_desc; }
private:
	LIGHT_DESC m_Light_desc = {};
	_int		m_iIndex = {};

	static _int s_iNowIndex;
public:
	static CLightObject* Create(ID3D11Device* pDevice,
		ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szLayerTag);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

