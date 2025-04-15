#pragma once

#include "Client_Defines.h"


#include "Level.h"

BEGIN(Client)

class CLevel_GamePlay final : public CLevel
{
private:
	CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_GamePlay() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera();	
	HRESULT Ready_Layer_UI();
	HRESULT Ready_Layer_Effect();

	HRESULT Ready_Layer_Monster();

	HRESULT Ready_LandObjects();

	HRESULT Ready_Layer_Player();

	HRESULT Ready_Layer_Texts();

	HRESULT LoadGameData();
	virtual HRESULT LoadData(_int iSubLevelIndex, _fvector vPlayerPosition) override;

	HRESULT LoadUnit(const _float4x4& transformMatrix, const wstring& modelName, _int iNaviLevel);
	HRESULT LoadInteractive(const _float4x4& transformMatrix, const wstring& modelName);
	HRESULT LoadDeco(const _float4x4& transformMatrix, const wstring& modelName);
	HRESULT LoadLight(ifstream& load, const _float4x4& transformMatrix);
	HRESULT LoadItem(const _float4x4& transformMatrix, const wstring& modelName, ifstream& readStream);
public:
	static CLevel_GamePlay* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END
