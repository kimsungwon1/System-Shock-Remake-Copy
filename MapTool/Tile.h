#pragma once
#include "ToolObject.h"
class CTile : public CToolObject
{
protected:
	CTile(ID3D11Device* pDevice,
		ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szPrototypeName, const _tchar* szLayerTag, const _tchar* ModelCompName, AllObjectType eType);
	CTile(const CTile& Prototype);
	~CTile() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	void Priority_Update(_float fTimeDelta) override;
	void Update(_float fTimeDelta) override;
	//void Late_Update(_float fTimeDelta) override;
	//virtual HRESULT Render() override;

protected:

public:
	static CTile* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CImguiDesign* pGui, const _tchar* szPrototypeName, const _tchar* szLayerTag, const _tchar* ModelCompName, AllObjectType eType);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

