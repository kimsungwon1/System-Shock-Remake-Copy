#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	/* IDirect3DDevice9 == ID3D11Device + ID3D11DeviceContext */
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	virtual HRESULT LoadData(_int iSubLevelIndex, _fvector vPlayerPosition) { return E_FAIL; }
protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

public:
	virtual void Free() override;
};

END