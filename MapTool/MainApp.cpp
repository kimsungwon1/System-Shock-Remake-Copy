
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
//#include "stdafx.h"

#include "MainApp.h"

#include "FreeCamera.h"
#include "Tile.h"
#include "MouseHandle.h"
#include "Terrain.h"

#include "Door.h"
#include "Card.h"

#include "GameInstance.h"
#include "Mesh.h"
#include "Model.h"

#include "Collider.h"

#include "MyNavigation.h"

#include "LightObject.h"

#include "ImguiDesign.h"

#include <cstdlib>

using namespace Engine;

vector<_wstring> ListFilesInDirectory(const _wstring& directory) {
	WIN32_FIND_DATA findFileData;
	vector<_wstring> ret;

	HANDLE hFind = FindFirstFile((directory + L"/*").c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return ret;
	}

	do {
		const _wstring fileOrDirectoryName = findFileData.cFileName;
		if (fileOrDirectoryName == L"." || fileOrDirectoryName == L"..") {
			continue;
		}
		ret.emplace_back(fileOrDirectoryName);
	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);
	return ret;
}

CMainApp::CMainApp()
	: m_pGameInstance { CGameInstance::Get_Instance() }
{
	/* 내가 레퍼런스 카운트를 증가시키는 시점 : */
	/* 원래 보관하기위한 포인터에 저장을 할때는 증가시키지 않는다. */
	/* 원래 보관된 위치에 있던 주소를 다른 포인터형변수에 저장을 하게되면 이때는 증가시킨다. */
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{	
	ENGINE_DESC			EngineDesc{};
	EngineDesc.hWnd = g_hWnd;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;
	EngineDesc.isWindowsed = true;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.DisplaySize = ImVec2(static_cast<float>(EngineDesc.iWinSizeX), static_cast<float>(EngineDesc.iWinSizeY));

	/* 엔진을 사용하기위한 여러 초기화 과정을 수행한다. */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);
	
	if (FAILED(Ready_Prototype_Static()))
		return E_FAIL;

	Save_model_prototypes();

	m_pGui = new CImguiDesign(m_pDevice, m_pContext);
	m_pMouseHandle = CMouseHandle::Create(m_pGui);

	m_pGui->SetMainApp(this);

	if (FAILED(Ready_Object()))
		return E_FAIL;
	if (FAILED(Ready_Light()))
		return E_FAIL;
	if (FAILED(Open_Level(LEVEL_TOOL)))
		return E_FAIL;
	return S_OK;
}

HRESULT CMainApp::Init_RemoveObjects()
{
	m_pMouseHandle->PopObject();

	auto* GetRefTiles = m_pGameInstance->Get_RefGameObjects(LEVEL_STATIC, TEXT("Tile"));
	while (nullptr != GetRefTiles && !GetRefTiles->empty()) {
		auto* pTile = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_STATIC, TEXT("Tile"));
		m_pGameInstance->Delete_CloneObject_ByPointer(LEVEL_STATIC, TEXT("Tile"), pTile);
	}
	auto* GetRefUnits = m_pGameInstance->Get_RefGameObjects(LEVEL_STATIC, TEXT("Unit"));
	while (nullptr != GetRefUnits && !GetRefUnits->empty()) {
		auto* pUnit = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_STATIC, TEXT("Unit"));
		m_pGameInstance->Delete_CloneObject_ByPointer(LEVEL_STATIC, TEXT("Unit"), pUnit);
	}
	auto* GetRefInteractives = m_pGameInstance->Get_RefGameObjects(LEVEL_STATIC, TEXT("Interactive"));
	while (nullptr != GetRefUnits && !GetRefUnits->empty()) {
		auto* pUnit = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_STATIC, TEXT("Interactive"));
		m_pGameInstance->Delete_CloneObject_ByPointer(LEVEL_STATIC, TEXT("Interactive"), pUnit);
	}

	auto* GetRefLights = m_pGameInstance->Get_RefGameObjects(LEVEL_STATIC, TEXT("Light"));
	while (nullptr != GetRefLights && !GetRefLights->empty()) {
		auto* pLight = m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_STATIC, TEXT("Light"));
		m_pGameInstance->Delete_CloneObject_ByPointer(LEVEL_STATIC, TEXT("Light"), pLight);
	}

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	/*if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_F5)) {
		Save();
	}
	else if (m_pGameInstance->Get_DIKeyState(DIKEYBOARD_F8)) {
		Load();
	}*/
	if (m_pGameInstance->Get_RealDIKeyStateDown(DIK_R)) {
		m_pGameInstance->SetDebugRenderOnOff();
	}

	m_pGameInstance->Update_Engine(fTimeDelta);

	m_pMouseHandle->Update(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	/*_bool bDemo = true;
	ImGui::ShowDemoWindow(&bDemo);*/
	m_pGui->Present();

	ImGui::Render();

	m_pGameInstance->Render_Begin();

	m_pGameInstance->Draw_Engine();


	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	m_pGameInstance->Render_End();

	return S_OK;
}


HRESULT CMainApp::Ready_Prototype_Static()
{
	/* For. Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, _wstring(TEXT("Prototype_Component_VIBuffer_Rect")),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, 100, 100))))
		return E_FAIL;

	/* Shader*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("Shader_VtxAnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		CShader::Create(m_pDevice, m_pContext, TEXT("Shader_VtxModel.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements)))) {
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;
	
	// Textures
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Terrain"),
		CTexture::Create(m_pDevice, m_pContext, 
			TEXT("../Client/Bin/Resources/Textures/Terrain/Grass_%d.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Brush"),
		CTexture::Create(m_pDevice, m_pContext,
			TEXT("Bin/Resources/whiteCircle.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Beam"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resources/Effects/Beam.dds"), 1))))
		return E_FAIL;

	// Collider
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FreeCamera"),
		CFreeCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Static_Model()))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Static_Model()
{
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(-0.5f, 0.f, 0.f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_TILE,
			"../../Resources/FBX/Tile_Medical/Tile.fbx",
			TEXT("Prototype_Component_Model_Medical_Tile1"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_TILE,
			"../../Resources/FBX/Tile_Medical/Tile2_normal.fbx",
			TEXT("Prototype_Component_Model_Medical_Tile2"), PreTransformMatrix))) {
			return E_FAIL;;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_TILE,
			"../../Resources/FBX/Tile_Medical/Tile3_normal.fbx",
			TEXT("Prototype_Component_Model_Medical_Tile3"), PreTransformMatrix))) {
			return E_FAIL;;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_TILE,
			"../../Resources/FBX/Tile_Medical/Tile4_normal.fbx",
			TEXT("Prototype_Component_Model_Medical_Tile4"), PreTransformMatrix))) {
			return E_FAIL;;
		}
	}
	{
		_matrix PreTransformMatrix2 = XMMatrixIdentity();

		PreTransformMatrix2 = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_TILE,
			"../../Resources/FBX/Tile_Medical/Tile_Half1.fbx",
			TEXT("Prototype_Component_Model_Medical_Tile_Half1"), PreTransformMatrix2))) {
			return E_FAIL;;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_TILE,
			"../../Resources/FBX/Tile_Medical/wall1_normal.fbx",
			TEXT("Prototype_Component_Model_Medical_wall1"), PreTransformMatrix))) {
			return E_FAIL;;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_TILE,
			"../../Resources/FBX/Tile_Medical/wall2_normal.fbx",
			TEXT("Prototype_Component_Model_Medical_wall2"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_TILE,
			"../../Resources/FBX/Tile_Medical/wall2_corner.fbx",
			TEXT("Prototype_Component_Model_Medical_wall2_corner"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_TILE,
			"../../Resources/FBX/Tile_Medical/wall_half.fbx",
			TEXT("Prototype_Component_Model_Medical_wall_half"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_TILE,
			"../../Resources/FBX/Tile_Medical/wall_tri.fbx",
			TEXT("Prototype_Component_Model_Medical_wall_tri"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_TILE,
			"../../Resources/FBX/Tile_Maintenance/ceiling_light.fbx",
			TEXT("Prototype_Component_Model_Maintenance_ceiling_light"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_TILE,
			"../../Resources/FBX/Tile_Maintenance/ceiling1.fbx",
			TEXT("Prototype_Component_Model_Maintenance_ceiling1"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_TILE,
			"../../Resources/FBX/Tile_Maintenance/ceiling2.fbx",
			TEXT("Prototype_Component_Model_Maintenance_ceiling2"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_TILE,
			"../../Resources/FBX/Tile_Maintenance/ceiling3.fbx",
			TEXT("Prototype_Component_Model_Maintenance_ceiling3"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	// Shelve
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_DECO,
			"../../Resources/FBX/Tile_Medical/shelve.fbx",
			TEXT("Prototype_Component_Model_shelve"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	// Interactives
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_Interactive,
			"../../Resources/FBX/Interactives/AlarmLight/AlarmLight.fbx",
			TEXT("Prototype_Component_Model_AlarmLight"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_Interactive,
			"../../Resources/FBX/Interactives/RespawnChamber/RespawnChamber.fbx",
			TEXT("Prototype_Component_Model_RespawnChamber"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_Interactive,
			"../../Resources/FBX/Interactives/Doors/BridgeDoor.fbx",
			TEXT("Prototype_Component_Model_BridgeDoor"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_Interactive,
			"../../Resources/FBX/Interactives/Elevator/Elevator.fbx",
			TEXT("Prototype_Component_Model_Elevator"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_Interactive,
			"../../Resources/FBX/Interactives/LaserButton/LaserButton.fbx",
			TEXT("Prototype_Component_Model_LaserButton"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	//Unit
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.013f, 0.013f, 0.013f)* XMMatrixRotationX(XMConvertToRadians(0.f)) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(Add_model_prototype(CModel::TYPE_ANIM, AllObjectType::TYPE_UNIT, "../../Resources/FBX/anim_CyborgAssassin/Assassin_rifle.fbx",
			TEXT("Prototype_Component_Model_Assassin"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(Add_model_prototype(CModel::TYPE_ANIM, AllObjectType::TYPE_UNIT,
			"../../Resources/FBX/anim_CortexReaver/CortexReaver.fbx",
			TEXT("Prototype_Component_Model_CortexReaver"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(Add_model_prototype(CModel::TYPE_ANIM, AllObjectType::TYPE_UNIT,
			"../../Resources/FBX/anim_Enforcer/CyborgEnforcer.fbx",
			TEXT("Prototype_Component_Model_Enforcer"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(Add_model_prototype(CModel::TYPE_ANIM, AllObjectType::TYPE_UNIT,
			"../../Resources/FBX/anim_Mutant/mutant2.fbx",
			TEXT("Prototype_Component_Model_DefaultMutant"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.015f, 0.015f, 0.015f) * XMMatrixRotationY(XMConvertToRadians(180.f));
		if (FAILED(Add_model_prototype(CModel::TYPE_ANIM, AllObjectType::TYPE_UNIT,
			"../../Resources/FBX/CyborgElite/CyborgElite.fbx",
			TEXT("Prototype_Component_Model_CyborgElite"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	/*Hand*/
	{
		/*_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_ANIM, AllObjectType::TYPE_UNIT,
			"../../Resources/FBX/Hand/Hand.fbx", TEXT("Prototype_Component_Model_PlayerHand"), PreTransformMatrix))) {
			return E_FAIL;
		}*/
	}
	/*Weapons*/
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_ITEM,
			"../../Resources/FBX/Weapons/assaultRifle.fbx",
			TEXT("Prototype_Component_Model_AssaultRifle"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_ITEM,
			"../../Resources/FBX/Weapons/pistol.fbx",
			TEXT("Prototype_Component_Model_Pistol"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.02f, 0.02f, 0.02f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_ITEM,
			"../../Resources/FBX/Weapons/Launcher.fbx",
			TEXT("Prototype_Component_Model_Launcher"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.05f, 0.05f, 0.05f) * XMMatrixRotationY(XMConvertToRadians(-90.f));
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_ITEM,
			"../../Resources/FBX/Rocket/Rocket.fbx",
			TEXT("Prototype_Component_Model_Rocket"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f) * XMMatrixRotationY(XMConvertToRadians(-90.f)) * XMMatrixTranslation(0.f, 0.1f,0.f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_ITEM,
			"../../Resources/FBX/Weapons/Grenade.fbx",
			TEXT("Prototype_Component_Model_Grenade"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.004f, 0.004f, 0.004f) * XMMatrixRotationY(XMConvertToRadians(-270.f));
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_ITEM,
			"../../Resources/FBX/Bullet/Bullet.fbx",
			TEXT("Prototype_Component_Model_Bullet"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.05f, 1.f, 0.05f) * XMMatrixRotationX(XMConvertToRadians(90.f)) * XMMatrixTranslation(0.f,0.f,32.f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_ITEM,
			"../../Resources/FBX/Thunder/Thunder.fbx",
			TEXT("Prototype_Component_Model_Thunder"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.03f, 0.03f, 0.03f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_ITEM,
			"../../Resources/FBX/Ball/Ball.fbx",
			TEXT("Prototype_Component_Model_Fireball"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	//Item
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_ITEM,
			"../../Resources/FBX/Interactives/Card/Card.fbx",
			TEXT("Prototype_Component_Model_Card"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_ITEM,
			"../../Resources/FBX/Crates/Crate_Pistol.fbx",
			TEXT("Prototype_Component_Model_Crate_Pistol"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	{
		_matrix PreTransformMatrix = XMMatrixIdentity();
		PreTransformMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
		if (FAILED(Add_model_prototype(CModel::TYPE_NONANIM, AllObjectType::TYPE_ITEM,
			"../../Resources/FBX/Crates/Crate_Rifle.fbx",
			TEXT("Prototype_Component_Model_Crate_Rifle"), PreTransformMatrix))) {
			return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CMainApp::Ready_Object()
{
	{
		CFreeCamera::CAMERA_FREE_DESC		Desc{};

		Desc.fSensor = 0.2f;
		Desc.vEye = _float4(0.f, 10.f, -10.f, 1.f);
		Desc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
		Desc.fFovy = XMConvertToRadians(60.0f);
		Desc.fNear = 0.1f;
		Desc.fFar = 1000.f;
		Desc.fSpeedPerSec = 10.f;
		Desc.fRotationPerSec = XMConvertToRadians(90.0f);
		Desc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Camera"), TEXT("Prototype_GameObject_FreeCamera"), &Desc)))
			return E_FAIL;
	}

	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Default_Terrain"),
			CTerrain::Create(m_pDevice, m_pContext)))) {
			return E_FAIL;
		}
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Terrain"),
			TEXT("Prototype_GameObject_Default_Terrain")))) {
			return E_FAIL;
		}
	}

	{
		if (FAILED(Add_Tile_prototype(TEXT("Prototype_GameObject_Default"), TEXT("Handle"), TEXT(""),
			AllObjectType::TYPE_END)))
			return E_FAIL;

		CToolObject::TOOLOBJECT_DESC desc = {};
		desc.fRotationPerSec = 10.f;
		desc.fSpeedPerSec = 10.f;
		desc.eType = AllObjectType::TYPE_UNIT;
		desc.szModelName = TEXT("Prototype_Component_Model_Assassin");
		XMStoreFloat4x4(&desc.transMat, XMMatrixIdentity());

		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Handle"),
			TEXT("Prototype_GameObject_Default"), &desc)))
			return E_FAIL;
	}

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Light"),
		CLightObject::Create(m_pDevice, m_pContext, m_pGui, TEXT("Light")))))
		return E_FAIL;

	auto* tile = dynamic_cast<CToolObject*>(m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_STATIC, TEXT("Handle"), -1));
	if (tile != nullptr) {
		m_pMouseHandle->PushObject(tile);
	}

	// Navigation
	{
		if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Navigation"),
			CMyNavigation::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, TEXT("Layer_Navigation"), TEXT("Prototype_GameObject_Navigation")))) {
			return E_FAIL;
		}
	}
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Door"),
		CDoor::Create(m_pDevice, m_pContext, m_pGui, TEXT("Tile")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Card"),
		CCard::Create(m_pDevice, m_pContext, m_pGui, TEXT("Item")))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Light()
{
	LIGHT_DESC		LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.7f, 0.7f, 0.7f, 1.f);
	LightDesc.vAmbient = LightDesc.vDiffuse;
	LightDesc.vSpecular = LightDesc.vDiffuse;

	if (FAILED(m_pGameInstance->Add_Light(LightDesc, 0)))
		return E_FAIL;
	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eStartLevelID)
{	
	/* 어떤 레벨을 선택하던 로딩 레벨로 선 진입한다.  */
	/* 로딩레벨에서 내가 선택한 레벨에 필요한 자원을 준비한다. */
	/* 로딩 레벨은 다음레벨(내가 선택한 레벨)이 누구인지를 인지해야한다. */
	

	return S_OK;
}

_wstring CMainApp::Save_By_Explore()
{
	_wstring openFileName = TEXT("");
	OPENFILENAME ofn;        // 공통 대화 상자 구조체
	TCHAR szFile[260];       // 파일 이름 저장 버퍼
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;    // 소유자 윈도우 핸들 (NULL로 설정)
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile) / sizeof(TCHAR);
	ofn.lpstrFilter = TEXT("All\0*.*\0Text\0*.TXT\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn) == TRUE)
	{
		openFileName = ofn.lpstrFile;
	}

	return openFileName;
}

_wstring CMainApp::Load_By_Explore()
{
	_wstring openFileName = TEXT("");
	OPENFILENAME ofn;        // 공통 대화 상자 구조체
	TCHAR szFile[260];       // 파일 이름 저장 버퍼
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;    // 소유자 윈도우 핸들 (NULL로 설정)
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile) / sizeof(TCHAR);
	ofn.lpstrFilter = TEXT("All\0*.*\0Text\0*.TXT\0");
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		openFileName = ofn.lpstrFile;
	}

	return openFileName;
}

HRESULT CMainApp::Save()
{
	std::filesystem::path p = Save_By_Explore();
	
	ofstream fout(p, std::ios::binary | std::ios::trunc);
	if (!fout) {
		MSG_BOX(TEXT("Error opening file for writing!"));
		return E_FAIL;
	}
	auto* layersTiles = m_pGameInstance->Get_RefGameObjects(LEVEL_STATIC, TEXT("Tile"));
	auto* layersUnits = m_pGameInstance->Get_RefGameObjects(LEVEL_STATIC, TEXT("Unit"));
	auto* layersDecos = m_pGameInstance->Get_RefGameObjects(LEVEL_STATIC, TEXT("Deco"));
	auto* layersItems = m_pGameInstance->Get_RefGameObjects(LEVEL_STATIC, TEXT("Item"));
	auto* layersInter = m_pGameInstance->Get_RefGameObjects(LEVEL_STATIC, TEXT("Interactive"));
	auto* layersLights = m_pGameInstance->Get_RefGameObjects(LEVEL_STATIC, TEXT("Light"));

	_uint objectsSize = 0;//layersTiles->size() + layersUnits->size();

	if (layersTiles != nullptr) {
		objectsSize += layersTiles->size();
	}
	if (layersUnits != nullptr) {
		objectsSize += layersUnits->size();
	}
	if (layersDecos != nullptr) {
		objectsSize += layersDecos->size();
	}
	if (layersItems != nullptr) {
		objectsSize += layersItems->size();
	}
	if (layersInter != nullptr) {
		objectsSize += layersInter->size();
	}
	if (layersLights != nullptr) {
		objectsSize += layersLights->size();
	}

	fout.write(reinterpret_cast<const char*>(&objectsSize), sizeof(_uint));

	if(layersTiles != nullptr)
	{
		for (auto& iter = begin(*layersTiles); iter != end(*layersTiles); iter++)
		{
			(*iter)->SaveObject(fout);
		}
	}
	if (layersUnits != nullptr)
	{
		for (auto& iter = begin(*layersUnits); iter != end(*layersUnits); iter++)
		{
			(*iter)->SaveObject(fout);
		}
	}
	if (layersDecos != nullptr)
	{
		for (auto& iter = begin(*layersDecos); iter != end(*layersDecos); iter++)
		{
			(*iter)->SaveObject(fout);
		}
	}
	if (layersItems != nullptr)
	{
		for (auto& iter = begin(*layersItems); iter != end(*layersItems); iter++)
		{
			(*iter)->SaveObject(fout);
		}
	}
	if (layersInter != nullptr)
	{
		for (auto& iter = begin(*layersInter); iter != end(*layersInter); iter++)
		{
			(*iter)->SaveObject(fout);
		}
	}
	if (layersLights != nullptr)
	{
		for (auto& iter = begin(*layersLights); iter != end(*layersLights); iter++)
		{
			(*iter)->SaveObject(fout);
		}
	}

	fout.close();

	return S_OK;
}

HRESULT CMainApp::Load()
{
	std::filesystem::path p = Load_By_Explore();

	ifstream fin(p, std::ios::binary);
	if (!fin) {
		MSG_BOX(TEXT("Error opening file for reading!"));
		return E_FAIL;
	}
	_uint objNum = 0;
	_uint objType = 0;

	Init_RemoveObjects();

	fin.read(reinterpret_cast<char*>(&objNum), sizeof(_uint));

	for (_uint i = 0; i < objNum; i++) {
		
		fin.read(reinterpret_cast<char*>(&objType), sizeof(objType));

		if (objType == TYPE_Light) {
			CLightObject::LIGHTOBJECT_DESC desc = {};

			_uint modelNameLength = {};
			_tchar modelName[MAX_PATH] = {};

			fin.read(reinterpret_cast<char*>(&desc.transMat), sizeof(desc.transMat));
			fin.read(reinterpret_cast<char*>(&modelNameLength), sizeof(_uint));
			fin.read(reinterpret_cast<char*>(&modelName), sizeof(_tchar) * modelNameLength);
			desc.szModelName = modelName;
			desc.fRotationPerSec = 0.f; desc.fSpeedPerSec = 0.f;
			desc.bPlaced = true; desc.eType = (AllObjectType)objType;
			desc.light_description.eType = LIGHT_DESC::TYPE_POINT;
			desc.light_description.vDirection = {0.f,0.f,0.f,0.f};
			fin.read(reinterpret_cast<char*>(&desc.light_description.vDirection), sizeof(_float4));
			fin.read(reinterpret_cast<char*>(&desc.light_description.vPosition), sizeof(_float4));
			fin.read(reinterpret_cast<char*>(&desc.light_description.fRange), sizeof(_float));
			fin.read(reinterpret_cast<char*>(&desc.light_description.vDiffuse), sizeof(_float4));
			fin.read(reinterpret_cast<char*>(&desc.light_description.vAmbient), sizeof(_float4));
			fin.read(reinterpret_cast<char*>(&desc.light_description.vSpecular), sizeof(_float4));

			m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, CToolObject::TypeToLayername[desc.eType],
				TEXT("Prototype_GameObject_Light"), &desc);
		}
		else {
			CDoor::DATAOBJ_DESC desc = {};

			_uint modelNameLength = {};
			_tchar modelName[MAX_PATH] = {};

			fin.read(reinterpret_cast<char*>(&desc.transMat), sizeof(desc.transMat));

			fin.read(reinterpret_cast<char*>(&modelNameLength), sizeof(_uint));
			fin.read(reinterpret_cast<char*>(&modelName), sizeof(_tchar) * modelNameLength);
			desc.szModelName = modelName;

			desc.fRotationPerSec = 10.f; desc.fSpeedPerSec = 10.f;
			desc.bPlaced = true; desc.eType = (AllObjectType)objType;

			if (lstrcmp(desc.szModelName.c_str(), TEXT("Prototype_Component_Model_Card")) == 0)
			{
				fin.read(reinterpret_cast<char*>(&desc.iData), sizeof(_int));
				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, CToolObject::TypeToLayername[desc.eType],
					TEXT("Prototype_GameObject_Card"), &desc)))
					return E_FAIL;
			}
			else if (lstrcmp(desc.szModelName.c_str(), TEXT("Door")) == 0)
			{
				fin.read(reinterpret_cast<char*>(&desc.iData), sizeof(_int));
				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, CToolObject::TypeToLayername[desc.eType],
					TEXT("Prototype_GameObject_Door"), &desc)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_STATIC, CToolObject::TypeToLayername[desc.eType],
					TEXT("Prototype_GameObject_Default"), &desc)))
					return E_FAIL;
			}
		}
	}

	if (fin.eof()) {
		std::cout << "End of file reached." << std::endl;
	}
	else if (fin.fail()) {
		std::cerr << "Input failure before reaching end of file." << std::endl;
	}

	fin.close();

	return S_OK;
}

HRESULT CMainApp::SaveNavi()
{
	auto fileName = Save_By_Explore();

	filesystem::path filePath(fileName);

	ofstream fout(filePath, std::ios::binary | std::ios::trunc);
	if (!fout) {
		MSG_BOX(TEXT("Error opening file for writing!"));
		return E_FAIL;
	}
	m_pMouseHandle->SaveNavi(fout);

	fout.close();

	return S_OK;
}

HRESULT CMainApp::LoadNavi()
{
	auto fileName = Load_By_Explore();

	filesystem::path filePath(fileName);

	ifstream fin(filePath, std::ios::binary);
	if (!fin) {
		MSG_BOX(TEXT("Error opening file for reading!"));
		return E_FAIL;
	}

	m_pMouseHandle->ClearNavi();
	m_pMouseHandle->LoadNavi(fin);

	fin.close();

	return S_OK;
}

HRESULT CMainApp::Add_model_prototype(_uint modelType, AllObjectType objectType, const _char* modelpath, const _tchar* Prototype_Component_Name, _matrix preTransformMat)
{
	auto* model = CModel::Create(m_pDevice, m_pContext, (CModel::TYPE)modelType, modelpath, preTransformMat);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, Prototype_Component_Name, model))) {
		return E_FAIL;
	}
	ModelAndName mn = {};
	mn.pModel = model;
	mn.szName = Prototype_Component_Name;
	m_vecPModelPrototypes[(_int)objectType].emplace_back(mn);
	Safe_AddRef(model);

	return S_OK;
}

HRESULT CMainApp::Add_Tile_prototype(const _tchar* Prototype_Name, const _tchar* Layer_Name,
	const _tchar* Model_Name, AllObjectType allObjectType)
{
	auto* pTile = CTile::Create(m_pDevice, m_pContext, m_pGui, Prototype_Name, Layer_Name,
		Model_Name, allObjectType);
	if (FAILED(m_pGameInstance->Add_Prototype(Prototype_Name, pTile))) {
		return E_FAIL;
	}
	vecPrototypeNameAndModelName.emplace_back(PrototypeNameAndModelName{ Prototype_Name, Model_Name });

	return S_OK;
}

HRESULT CMainApp::Save_model_prototypes()
{
	for (auto& pMNArr : m_vecPModelPrototypes)
	{
		for(auto& pMN : pMNArr)
		{
			_wstring path = { L"../Data/ModelData/" };
			path += pMN.szName;
			path += L".model";

			ofstream modelPrototypeSaves(path, std::ios::binary | std::ios::trunc);


			if (FAILED(pMN.pModel->SaveModel(modelPrototypeSaves))) {
				return E_FAIL;
			}

			modelPrototypeSaves.close();
		}
	}

	return S_OK;
}

HRESULT CMainApp::Load_model_prototypes()
{
	
	vector<_wstring> vec = ListFilesInDirectory(L"../Data/ModelData");

	for (auto& path : vec)
	{
		_wstring realPath = L"../Data/ModelData";
		std::filesystem::path fsPath(path);
		std::wstring fileName = fsPath.stem().wstring();
		realPath += L"/";
		realPath += path;
		ifstream modelPrototypeLoads(realPath, std::ios::binary);

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, fileName, CModel::Create(
			m_pDevice, m_pContext, modelPrototypeLoads)))) {
			return E_FAIL;
		}
	}

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pMouseHandle);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& pMNArr : m_vecPModelPrototypes)
	{
		for (auto& pMN : pMNArr) {
			Safe_Release(pMN.pModel);
		}
		pMNArr.clear();
	}

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
	Safe_Delete(m_pGui);
}

