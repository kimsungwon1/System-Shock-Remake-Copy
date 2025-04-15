#pragma once
#include "Tile.h"

BEGIN(Engine)
class CTexture;
class CVIBuffer_Rect;
class CCollider;
END

class CDoor : public CTile
{
public:
	struct DATAOBJ_DESC : CToolObject::TOOLOBJECT_DESC {
		_int iData = {};
	};
private:
	CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szLayerTag);
	CDoor(const CDoor& Prototype);
	virtual ~CDoor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Priority_Update(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Late_Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	_bool IsPicking(_float3* pOut) override;
	void Set_Data(_int iData) {
		m_iData = iData;
	}
	_int Get_Data() const override {
		return m_iData;
	}
	HRESULT SaveObject(ofstream& saveStream) const override;
	HRESULT LoadObject(ifstream& loadStream) override;
private:
	HRESULT Ready_Components();

	_float2 m_vTime = {};

	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;

	_float3 m_vPoints[4] = { {-0.5f, 0.5f, 0.f}, {0.5f, 0.5f, 0.f},
							{0.5f, -0.5f, 0.f}, {-0.5f, -0.5f, 0.f} };
	_int m_iData = {};
public:
	static CDoor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szLayerTag);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

