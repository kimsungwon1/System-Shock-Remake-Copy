#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "FPSCamera.h"
#include "FPSPlayer.h"

#include "TextObject.h"
#include "TileObject.h"
#include "UnitObject.h"

#include "SoundMgr.h"

#include "Card.h"
#include "Door.h"

#include "Weapon.h"

#include "GameInstance.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Effect()))
		return E_FAIL;
	if (FAILED(Ready_Layer_Monster()))
		return E_FAIL;

	if (FAILED(Ready_LandObjects()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Texts()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player()))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI()))
		return E_FAIL;
	// LEVEL1 : vPlayerPosition = { 3.0f, 0.1f, 1.9f } / { 11.11f, 0.1f, 41.8f };
	// LEVEL2 : vPlayerPosition = { 57.93f, 0.1f, 37.2f };
	// LEVEL3 : vPlayerPosition = { 52.08f, 0.1f, 57.f };
	if (FAILED(LoadData(LEVEL1, { 3.0f, 0.1f, 1.9f })))
		return E_FAIL;

	return S_OK;
}

void CLevel_GamePlay::Update(_float fTimeDelta)
{
	SetWindowText(g_hWnd, to_wstring(1 / fTimeDelta).c_str());
}

HRESULT CLevel_GamePlay::Render()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	/* 게임플레이 레벨에 필요한 광원을 준비한다. */
	LIGHT_DESC		LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);//_float4(0.3f, 0.3f, 0.3f, 1.f);
	LightDesc.vAmbient = LightDesc.vDiffuse;
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc, 0)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera()
{
	CFPSCamera::CAMERA_FPS_DESC		Desc{};

	//Desc.fSensor = 0.2f;
	Desc.vEye = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	Desc.fFovy = XMConvertToRadians(60.0f);
	Desc.fNear = 0.1f;
	Desc.fFar = 1000.f;
	Desc.fSpeedPerSec = 30.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;

	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FPSCamera"), &Desc)))
		return E_FAIL;

	return S_OK;
}


HRESULT CLevel_GamePlay::Ready_Layer_UI()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_HealthBar"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Bullets"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Crosshair"))))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Effect()
{
	/*if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle_Snow"))))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster()
{	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObjects()
{
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Prototype_GameObject_FPS_Player"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Texts()
{
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_AnimationText"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::LoadGameData()
{
	filesystem::path gameDataPath{ "../../Data/ss1" };

	ifstream gameRead(gameDataPath, std::ios::binary);

	if (!gameRead) {
		MSG_BOX(TEXT("Error opening file for reading!"));
		return E_FAIL;
	}
	_uint objNum = 0;
	_uint objType = 0;

	gameRead.read(reinterpret_cast<char*>(&objNum), sizeof(_uint));

	for (_uint i = 0; i < objNum; i++) {
		gameRead.read(reinterpret_cast<char*>(&objType), sizeof(objType));

		_float4x4 transformMatrix = {};
		_uint modelNameLength = {};
		_tchar modelName[MAX_PATH] = {};

		gameRead.read(reinterpret_cast<char*>(&transformMatrix), sizeof(transformMatrix));

		gameRead.read(reinterpret_cast<char*>(&modelNameLength), sizeof(_uint));
		gameRead.read(reinterpret_cast<char*>(&modelName), sizeof(_tchar) * modelNameLength);

		CTileObject::Tile_Desc desc = {};
		//CUnitObject::
		switch (objType) {
		case (_uint)AllObjectType::TYPE_TILE:
			

			desc.fRotationPerSec = 0.f; desc.fSpeedPerSec = 0.f;
			desc.szModelName = modelName; desc.transMat = transformMatrix;

			if (lstrcmp(modelName, TEXT("Door")) == 0) {
				/*CCard::CARD_DESC cDesc = {};
				cDesc.fRotationPerSec = desc.fRotationPerSec; cDesc.fSpeedPerSec = desc.fSpeedPerSec;
				cDesc.iData*/

				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Tile"),
					TEXT("Prototype_GameObject_Door"), &desc)))
					return E_FAIL;
			}
			if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Tile"),
				TEXT("Prototype_GameObject_TileObject"), &desc)))
				return E_FAIL;
			break;
		case (_uint)AllObjectType::TYPE_UNIT:
			LoadUnit(transformMatrix, modelName, LEVEL1);
			break;
		case (_uint)AllObjectType::TYPE_Interactive:
			LoadInteractive(transformMatrix, modelName);

			break;
		case (_uint)AllObjectType::TYPE_ITEM:
			if (FAILED(LoadItem(transformMatrix, modelName, gameRead)))
				return E_FAIL;

			break;
		case (_uint)AllObjectType::TYPE_Light:
			LoadLight(gameRead, transformMatrix);
			break;
		}
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::LoadData(_int iSubLevelIndex, _fvector vPlayerPosition)
{
	// 옥트리 레디
	m_pGameInstance->Delete_Octree(LEVEL_GAMEPLAY, TEXT("Layer_Tile"));

	_wstring strBGMName = {TEXT("Level1")};

	//기존에 있던 데이터들 죄다 지우기
	auto* GetRefTiles = m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Tile"));
	while (nullptr != GetRefTiles && !GetRefTiles->empty()) {
		auto* pTile = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Tile"));
		m_pGameInstance->Delete_CloneObject_ByPointer(LEVEL_GAMEPLAY, TEXT("Layer_Tile"), pTile);
	}
	auto* GetRefUnits = m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"));
	while (nullptr != GetRefUnits && !GetRefUnits->empty()) {
		auto* pUnit = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"));
		m_pGameInstance->Delete_CloneObject_ByPointer(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"), pUnit);
	}
	auto* GetRefInteractives = m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Interactive"));
	while (nullptr != GetRefInteractives && !GetRefInteractives->empty()) {
		auto* pInter = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Interactive"));
		m_pGameInstance->Delete_CloneObject_ByPointer(LEVEL_GAMEPLAY, TEXT("Layer_Interactive"), pInter);
	}
	auto* GetRefItems = m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Item"));
	if(GetRefItems != nullptr)
	{
		for (auto* pItem : *GetRefItems) {
			CItem* pIt = dynamic_cast<CItem*>(pItem);
			if (pIt->Is_OnInven())
				continue;
			m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_Item"), pItem);
		}
		/*_int iSize = GetRefItems->size();
		for (_int i = 0; i < iSize; i++) {
			auto* pItem = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Item"), iSize - 1 - i);
			CItem* pIt = dynamic_cast<CItem*>(pItem);
			if (pIt->Is_OnInven())
				continue;
			m_pGameInstance->Delete_CloneObject_ByPointer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), pItem);
		}*/
	}
	//나중에 빛도 제거 초기화해야 함.

	// 플레이어에게 네비게이션 레벨 전달
	auto* pPlayer = dynamic_cast<CFPSPlayer*>(m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Player")));
	pPlayer->SetLevel(iSubLevelIndex);


	//게임 데이터
	filesystem::path gameDataPath{};
	LIGHT_DESC		LightDesc{};

	switch ((GAMEPLAY_LEVELID)(iSubLevelIndex)) {
	case GAMEPLAY_LEVELID::LEVEL1:
		gameDataPath = "../../Data/ss1_4";
		//vPlayerPosition Setting
		LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
		LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
		LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vAmbient = LightDesc.vDiffuse;
		LightDesc.vSpecular = LightDesc.vDiffuse;
		strBGMName = TEXT("Level1");
		break;
	case GAMEPLAY_LEVELID::LEVEL2:
		gameDataPath = "../../Data/ss2";
		//vPlayerPosition Setting
		LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
		LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
		LightDesc.vDiffuse = _float4(0.05f, 0.05f, 0.05f, 1.f);
		LightDesc.vAmbient = LightDesc.vDiffuse;
		LightDesc.vSpecular = { 0.f,0.f,0.f,1.f };
		strBGMName = TEXT("Level2");
		break;
	case GAMEPLAY_LEVELID::LEVEL_BOSS:
		gameDataPath = "../../Data/ss3";

		LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
		LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
		LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
		LightDesc.vAmbient = LightDesc.vDiffuse;
		LightDesc.vSpecular = { 1.f,1.f,1.f,1.f };
		strBGMName = TEXT("Boss");
		break;
	default:
		gameDataPath = "../../Data/ss1";
		//vPlayerPosition Setting
		break;
	}

	CSoundMgr::Get_Instance()->PlayBGM(strBGMName.c_str(), 1.f);

	m_pGameInstance->Change_Light(LightDesc, 0);

	//플레이어 위치 Set
	pPlayer->SetPositionNavi(vPlayerPosition);
	//옥트리 - 타일 활성화
	/*if(FAILED(m_pGameInstance->Add_Octree(LEVEL_GAMEPLAY, TEXT("Layer_Tile"), 8.f, 8.f, vPlayerPosition)))
		return E_FAIL;*/

	ifstream gameRead(gameDataPath, std::ios::binary);

	if (!gameRead) {
		MSG_BOX(TEXT("Error opening file for reading!"));
		return E_FAIL;
	}
	_uint objNum = 0;
	_uint objType = 0;

	gameRead.read(reinterpret_cast<char*>(&objNum), sizeof(_uint));

	for (_uint i = 0; i < objNum; i++) {
		gameRead.read(reinterpret_cast<char*>(&objType), sizeof(objType));

		_float4x4 transformMatrix = {};
		_uint modelNameLength = {};
		_tchar modelName[MAX_PATH] = {};

		gameRead.read(reinterpret_cast<char*>(&transformMatrix), sizeof(transformMatrix));

		gameRead.read(reinterpret_cast<char*>(&modelNameLength), sizeof(_uint));
		gameRead.read(reinterpret_cast<char*>(&modelName), sizeof(_tchar) * modelNameLength);
		
		CTileObject::Tile_Desc desc = {};
		CCard::CARD_DESC cDesc = {};
		//CUnitObject::
		switch (objType) {
		case (_uint)AllObjectType::TYPE_TILE:

			desc.fRotationPerSec = 0.f; desc.fSpeedPerSec = 0.f;
			desc.szModelName = modelName; desc.transMat = transformMatrix;

			cDesc.fRotationPerSec = 0.f; cDesc.fSpeedPerSec = 0.f;
			cDesc.transMat = transformMatrix;

			if (lstrcmp(modelName, TEXT("Door")) == 0) {
				_int iData = { -1 };
				gameRead.read(reinterpret_cast<char*>(&iData), sizeof(_int));
				cDesc.iData = iData;
				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Tile"),
					TEXT("Prototype_GameObject_Door"), &cDesc)))
					return E_FAIL;
				/*auto* pObj = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Tile"));
				if(pObj != nullptr)
				{
					m_pGameInstance->Push_Back_ToOctree(LEVEL_GAMEPLAY, TEXT("Layer_Tile"), pObj, pObj->GetTransformCom()->Get_State(CTransform::STATE_POSITION));
				}*/
			}
			else {
				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Tile"),
					TEXT("Prototype_GameObject_TileObject"), &desc)))
					return E_FAIL;
				/*auto* pObj = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Tile"));
				if (pObj != nullptr)
				{
					m_pGameInstance->Push_Back_ToOctree(LEVEL_GAMEPLAY, TEXT("Layer_Tile"), pObj, pObj->GetTransformCom()->Get_State(CTransform::STATE_POSITION));
				}*/
			}
			break;
		case (_uint)AllObjectType::TYPE_UNIT:
			if (FAILED(LoadUnit(transformMatrix, modelName, iSubLevelIndex))) {
				return E_FAIL;
			}
			break;
		case (_uint)AllObjectType::TYPE_Interactive:
			if (FAILED(LoadInteractive(transformMatrix, modelName)))
				return E_FAIL;

			break;
		case (_uint)AllObjectType::TYPE_ITEM:
			if (FAILED(LoadItem(transformMatrix, modelName, gameRead)))
				return E_FAIL;
			break;

		case (_uint)AllObjectType::TYPE_Light:
			LoadLight(gameRead, transformMatrix);
			break;
		}
	}

	m_pGameInstance->Add_Octree(LEVEL_GAMEPLAY, TEXT("Layer_Tile"), 2);

	return S_OK;
}

HRESULT CLevel_GamePlay::LoadUnit(const _float4x4& transformMatrix, const wstring& modelName, _int iNaviLevel)
{
	CGameObject::GAMEOBJECT_DESC desc = {};
	desc.fRotationPerSec = 50, desc.fSpeedPerSec = 10.f;
	desc.transMat = transformMatrix; desc.iCurrentLevel = iNaviLevel;

	CGameObject::GAMEOBJECT_DESC enforcerDesc = {};
	enforcerDesc.fRotationPerSec = 100.f, enforcerDesc.fSpeedPerSec = 5.f;
	enforcerDesc.transMat = transformMatrix; enforcerDesc.iCurrentLevel = iNaviLevel;

	CGameObject::GAMEOBJECT_DESC ReaverDesc = {};
	ReaverDesc.fRotationPerSec = XMConvertToRadians(180.f), ReaverDesc.fSpeedPerSec = 10.f;
	ReaverDesc.transMat = transformMatrix; ReaverDesc.iCurrentLevel = iNaviLevel;
	
	CGameObject::GAMEOBJECT_DESC EliteDesc = {};
	EliteDesc.fRotationPerSec = XMConvertToRadians(180.f), EliteDesc.fSpeedPerSec = 10.f;
	EliteDesc.transMat = transformMatrix; EliteDesc.iCurrentLevel = iNaviLevel;

	auto type = GetType_Name(modelName);

	// 타입에 따라 유닛 추가. 클론오브젝트
	if (type == UNIT_DefaultMutant)
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"), TEXT("Prototype_GameObject_DefaultMutant"), &desc)))
			return E_FAIL;
	}
	else if (type == UNIT_Assassin)
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"), TEXT("Prototype_GameObject_Assassin"), &desc)))
			return E_FAIL;
	}
	else if (type == UNIT_CortexReaver)
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"), TEXT("Prototype_GameObject_CortexReaver"), &ReaverDesc)))
			return E_FAIL;
	}
	else if (type == UNIT_Enforcer)
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"), TEXT("Prototype_GameObject_Enforcer"), &enforcerDesc)))
			return E_FAIL;
	}
	else if (type == UNIT_Elite)
	{
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"), TEXT("Prototype_GameObject_Elite"), &EliteDesc)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CLevel_GamePlay::LoadInteractive(const _float4x4& transformMatrix, const wstring& modelName)
{
	CGameObject::GAMEOBJECT_DESC desc = {};
	desc.fRotationPerSec = 100.f, desc.fSpeedPerSec = 10.f;
	desc.transMat = transformMatrix;
	auto type = GetTypeInterative_Name(modelName);

	if (type == INTER_AlarmLight) {

	}
	if (type == INTER_Door) {
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Interactive"), TEXT("Prototype_GameObject_Door"), &desc);
	}
	if (type == INTER_Elevator) {
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Interactive"), TEXT("Prototype_GameObject_Elevator"), &desc);
	}
	if (type == INTER_LaserButton) {

	}
	if (type == INTER_RespawnChamber) {
		m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Interactive"), TEXT("Prototype_GameObject_RespawnChamber"), &desc);
	}
	return S_OK;
}

HRESULT CLevel_GamePlay::LoadDeco(const _float4x4& transformMatrix, const wstring& modelName)
{
	return S_OK;
}

HRESULT CLevel_GamePlay::LoadLight(ifstream& load, const _float4x4& transformMatrix)
{
	LIGHT_DESC lDesc = {};
	lDesc.eType = LIGHT_DESC::TYPE_POINT;
	load.read(reinterpret_cast<char*>(&lDesc.vDirection), sizeof(_float4));
	load.read(reinterpret_cast<char*>(&lDesc.vPosition), sizeof(_float4));
	load.read(reinterpret_cast<char*>(&lDesc.fRange), sizeof(_float));
	load.read(reinterpret_cast<char*>(&lDesc.vDiffuse), sizeof(_float4));
	load.read(reinterpret_cast<char*>(&lDesc.vAmbient), sizeof(_float4));
	load.read(reinterpret_cast<char*>(&lDesc.vSpecular), sizeof(_float4));

	m_pGameInstance->Add_Light(lDesc, 0);

	return S_OK;
}

HRESULT CLevel_GamePlay::LoadItem(const _float4x4& transformMatrix, const wstring& modelName, ifstream& readStream)
{
	CWeapon::Weapon_DESC desc = {};
	desc.fRotationPerSec = 100.f, desc.fSpeedPerSec = 10.f;
	desc.transMat = transformMatrix; desc.pParentTransform = nullptr; desc.pSocketTransform = nullptr;

	CCard::CARD_DESC cDesc = {};
	cDesc.fRotationPerSec = 100.f, cDesc.fSpeedPerSec = 10.f;
	cDesc.transMat = transformMatrix; cDesc.pOwner = nullptr;

	if (IsSame(modelName, TEXT("Prototype_Component_Model_AssaultRifle"))) {
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_AssaultRifle"), &desc)))
			return E_FAIL;
	}
	if (IsSame(modelName, TEXT("Prototype_Component_Model_Pistol"))) {
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Pistol"), &desc)))
			return E_FAIL;
	}
	if (IsSame(modelName, TEXT("Prototype_Component_Model_Launcher"))) {
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_RocketLauncher"), &desc)))
			return E_FAIL;
	}
	if (IsSame(modelName, TEXT("Prototype_Component_Model_Card"))) {
		readStream.read(reinterpret_cast<char*>(&cDesc.iData), sizeof(_int));

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_Card"), &cDesc)))
			return E_FAIL;
	}
	if (IsSame(modelName, TEXT("Prototype_Component_Model_Rocket"))) {
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_RocketPack"), &desc)))
			return E_FAIL;
	}
	if (IsSame(modelName, TEXT("Prototype_Component_Model_Crate_Pistol"))) {
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_PistolBulletPack"), &desc)))
			return E_FAIL;
	}
	if (IsSame(modelName, TEXT("Prototype_Component_Model_Crate_Rifle"))) {
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Item"), TEXT("Prototype_GameObject_RifleBulletPack"), &desc)))
			return E_FAIL;
	}

	return S_OK;
}


CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CLevel_GamePlay"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();

}
