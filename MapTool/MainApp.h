#pragma once

#include "Client_Defines.h"
#include "Base.h"

class CImguiDesign;
class CMouseHandle;

BEGIN(Engine)
class CGameInstance;
class CModel;
END

class CMainApp final : public CBase
{
	friend class CImguiDesign;

	struct ModelAndName {
		_wstring szName = {};
		CModel* pModel = { nullptr };
	};

	struct PrototypeNameAndModelName {
		_wstring szPrototypeName = {};
		_wstring szModelName = {};
		//ALLObjectyType
	};

private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT	Initialize();
	HRESULT Init_RemoveObjects();
	void Update(_float fTimeDelta);
	HRESULT Render();

private:
	CGameInstance*			m_pGameInstance = { nullptr };
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	CImguiDesign*			m_pGui = { nullptr };
	CMouseHandle*			m_pMouseHandle = { nullptr };

private:	
	HRESULT Ready_Prototype_Static();
	HRESULT Ready_Prototype_Static_Model();
	HRESULT Ready_Object();
	HRESULT Ready_Light();
	HRESULT Open_Level(LEVELID eStartLevelID);

public:
	_wstring Save_By_Explore();
	_wstring Load_By_Explore();
public:
	HRESULT Save();
	HRESULT Load();
	HRESULT SaveNavi();
	HRESULT LoadNavi();
private:
	// objectType : Tile(Terrain), Deco, Unit
	HRESULT Add_model_prototype(_uint modelType, AllObjectType objectType, const _char* modelpath,
		const _tchar* Prototype_Component_Name, _matrix preTransformMat = XMMatrixIdentity());
	HRESULT Add_Tile_prototype(const _tchar* Prototype_Name, const _tchar* Layer_Name,
		const _tchar* Model_Name, AllObjectType allObjectType);
	HRESULT Save_model_prototypes();
	HRESULT Load_model_prototypes();

	vector<PrototypeNameAndModelName> vecPrototypeNameAndModelName = {};
public:
	const vector<ModelAndName>& GetRefModelPrototype(AllObjectType type) const { return m_vecPModelPrototypes[(_int)type]; }
	const vector<PrototypeNameAndModelName>& GetPrototypeNameAndModelName() const { return vecPrototypeNameAndModelName; }
private:

	// 0 : Tile(Terrain), 1 : Deco, 2 : Unit
	vector<ModelAndName> m_vecPModelPrototypes[(_int)AllObjectType::TYPE_END] = { vector<ModelAndName>(), vector<ModelAndName>(), vector<ModelAndName>(),  vector<ModelAndName>() };
public:
	static CMainApp* Create();
	virtual void Free() override;
};
