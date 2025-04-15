#include "stdafx.h"
#include "..\Public\Loader.h"
#include "Monster.h"
#include "Terrain.h"
#include "FreeCamera.h"
#include "BackGround.h"
#include "GameInstance.h"
#include "Body_Player.h"
#include "Sword.h"
#include "FPS_Arm.h"
#include "FPSPlayer.h"

#include "Physics.h"

#include "VIBuffer_Point_Instance.h"
#include "VIBuffer_Point_Screw.h"
#include "VIBuffer_Point_Screw_Short.h"
#include "VIBuffer_Point_InvenSlot.h"

//Monsters
#include "DefaultMutant.h"
#include "CyborgAssassin.h"
#include "CortexReaver.h"
#include "CyborgEnforcer.h"
#include "CyborgElite.h"

//Tile
#include "TileObject.h"

//Weapon
#include "AssaultRifle.h"
#include "Pipe.h"
#include "Pistol.h"
#include "Grenade.h"
#include "RocketLauncher.h"

//Camera
#include "FPSCamera.h"

#include "TextObject.h"

#include "AnimationText.h"

#include "Player.h"
#include "Navigation.h"
#include "Collider.h"

//Interactives
#include "Elevator.h"
#include "RespawnChamber.h"
#include "Door.h"
#include "Card.h"
//Item
#include "Rocket.h"
#include "Bullet.h"
#include "RocketPack.h"
#include "RifleBulletPack.h"
#include "PistolBulletPack.h"

//UI
#include "ElevatorUI.h"
#include "Button.h"
#include "UI_HealthBar.h"
#include "UI_Bullets.h"
#include "UI_Crosshair.h"
#include "UI_Info.h"

#include "UI_InvenSlot.h"
#include "UI_Item.h"

#include "Inventory.h"
//Effect
#include "Thunder.h"
#include "Effect_GunFire.h"
#include "Effect_BombBlow.h"
#include "Fireball.h"
#include "Effect_BloodHit.h"
#include "Effect_BulletHit.h"
#include "Effect_Blast.h"
#include "Effect_EnemyGunFire.h"
#include "Effect_Muzzle.h"
#include "Effect_Muzzle_Flame.h"
#include "Effect_BulletCrater.h"

#include "Particle_BossScrew.h"
#include "Particle_BossScrew_Short.h"
#include "Particle_Snow.h"

#include "LoadingScreen.h"

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

CLoader::CLoader(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice { pDevice }
	, m_pContext { pContext }
	, m_pGameInstance { CGameInstance::Get_Instance()}
{
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = static_cast<CLoader*>(pArg);

	if(FAILED(pLoader->Loading()))
		return 1;
		
	return 0;
}


HRESULT CLoader::Initialize(LEVELID eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_LOADING, TEXT("Layer_UI"), TEXT("Prototype_GameObject_LoadingScreen"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading()
{
	EnterCriticalSection(&m_CriticalSection);

	HRESULT			hr = {};

	switch (m_eNextLevelID)
	{
	case LEVEL_LOGO:
		hr = Ready_Resources_For_LogoLevel();
		break;
	case LEVEL_GAMEPLAY:
		hr = Ready_Resources_For_GamePlayLevel();
		break;
	}

	LeaveCriticalSection(&m_CriticalSection);

	return hr;
}

void CLoader::Draw_LoadingText()
{
	SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Ready_Resources_For_LogoLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	/* For. Prototype_Component_Texture_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/poster.jpg"), 1))))
		return E_FAIL;


	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));


	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	/* For. Prototype_GameObject_BackGround */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BackGround"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Ready_Resources_For_GamePlayLevel()
{
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다."));
	
	// For. Button
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Button"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/UI/ButtonElement.png"), 1))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HealthBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/UI/HealthBar.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Crosshair"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/UI/Crosshair2.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GunFire"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Effects/fire/ShotgunConeShape.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BombBlow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Effects/explosion/T_Expl_Air1_color.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BombBlow_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Effects/explosion/T_Expl_Air1_packed.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blood_Hit"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Effects/blood/T_BloodSplatter_%d.dds"), 6))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Blood_Hit_Normal"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Effects/blood/T_BloodSplatter_N_%d.dds"), 6))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Bullet_Hit"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Effects/T_RadialGradient.dds"), 1))))
		return E_FAIL;
	/* For. Prototype_Component_Texture_Particle */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Particle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Beam"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Effects/Beam.dds"), 1))))
		return E_FAIL;
	if(FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Blast"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Effects/fire/BulletHolesDepth_flame.dds"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Muzzle_Flash"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/new_pfx/aa_ak4muzzle_0%d.dds"), 3))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Muzzle_Flame"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/new_pfx/aamuzzle_round_tiled%d.dds"), 2))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BulletCrater"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Effects/crater/bulletCrater.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_TextBox"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/UI/TextBox.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Inven/Slot.png"), 1))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_AssaultRifle_Slot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Inven/AssaultRifle.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grenade_Slot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Inven/Grenade.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MiniPistol_Slot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Inven/MiniPistol.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PistolBullets_Slot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Inven/PistolBullets.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RifleBullets_Slot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Inven/RifleBullets.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RocketLauncher_Slot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Inven/RocketLauncher.png"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Rockets_Slot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/Inven/Rockets.png"), 1))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("모델을(를) 로딩중입니다."));
	/* For. Prototype_Component_VIBuffer_Terrain*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	// For. All Binary Models
	vector<_wstring> vec = ListFilesInDirectory(L"../../Data/ModelData");
	for (auto& path : vec)
	{
		_wstring realPath = L"../../Data/ModelData";
		std::filesystem::path fsPath(path);
		std::wstring fileName = fsPath.stem().wstring();
		realPath += L"/";
		realPath += path;
		ifstream modelPrototypeLoads(realPath, std::ios::binary);

		if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, fileName, CModel::Create(
			m_pDevice, m_pContext, modelPrototypeLoads)))) {
			return E_FAIL;
		}
	}
	CVIBuffer_Instancing::INSTANCE_DESC			ParticleDesc{};
	/* For. Prototype_Component_VIBuffer_Particle_Explosion */
	ZeroMemory(&ParticleDesc, sizeof ParticleDesc);

	ParticleDesc.iNumInstance = 3000;
	ParticleDesc.vCenter = _float3(64.f, 20.f, 64.f);
	ParticleDesc.vRange = _float3(128.f, 1.f, 128.f);
	ParticleDesc.vSize = _float2(0.1f, 0.3f);
	ParticleDesc.vPivot = _float3(0.f, 0.f, 0.f);
	ParticleDesc.vSpeed = _float2(1.f, 3.f);
	ParticleDesc.vLifeTime = _float2(4.f, 8.f);
	ParticleDesc.isLoop = true;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Snow"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;
	ParticleDesc.vSize = _float2(0.05f, 0.05f);
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Screw"),
		CVIBuffer_Point_Screw::Create(m_pDevice, m_pContext, ParticleDesc))))
		return E_FAIL;

	CVIBuffer_Instancing::INSTANCE_DESC			sParticleDesc{};
	/* For. Prototype_Component_VIBuffer_Particle_Explosion */
	ZeroMemory(&sParticleDesc, sizeof sParticleDesc);

	sParticleDesc.iNumInstance = 500;
	sParticleDesc.vCenter = _float3(64.f, 20.f, 64.f);
	sParticleDesc.vRange = _float3(128.f, 1.f, 128.f);
	sParticleDesc.vSize = _float2(0.1f, 0.3f);
	sParticleDesc.vPivot = _float3(0.f, 0.f, 0.f);
	sParticleDesc.vSpeed = _float2(1.f, 3.f);
	sParticleDesc.vLifeTime = _float2(4.f, 8.f);
	sParticleDesc.isLoop = true;
	if(FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Particle_Screw_Short"),
		CVIBuffer_Point_Screw_Short::Create(m_pDevice, m_pContext, sParticleDesc))))
		return E_FAIL;
	
	sParticleDesc.iNumInstance = 60;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_InvenSlot"),
		CVIBuffer_Point_InvenSlot::Create(m_pDevice, m_pContext, sParticleDesc))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("네비게이션을 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation1"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/navi1-2")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation2"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/navi2")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation3"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../../Data/navi3")))))
		return E_FAIL;

	

	lstrcpy(m_szLoadingText, TEXT("셰이더을(를) 로딩중입니다."));
	/* For. Prototype_Component_Shader_VtxNorTex*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosNorTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For. Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/*For. Prototype_Component_Shader_VtxAnimation*/
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTXPOINTINSTANCE::Elements, VTXPOINTINSTANCE::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("물리 컴포넌트를 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physics"),
		CPhysics::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("사운드을(를) 로딩중입니다."));

	lstrcpy(m_szLoadingText, TEXT("UI을(를) 로딩중입니다."));
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Elevator"),
		CElevatorUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Button"),
		CButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_HealthBar"),
		CUI_HealthBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Bullets"),
		CUI_Bullets::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Crosshair"),
		CUI_Crosshair::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_TextBox"),
		CUI_Info::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Inventory"),
		CInventory::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_InvenSlot"),
		CUI_InvenSlot::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Item"),
		CUI_Item::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("객체원형을(를) 로딩중입니다."));
	/* For. Prototype_GameObject_Terrain */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Terrain"),
		CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For. Prototype_GameObject_Monster */
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Monster"),
		CMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Body_Player"),
		CBody_Player::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FPS_Arm"),
		CFPS_Arm::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FPS_Player"),
		CFPSPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Weapon"),
		CSword::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Player"),
		CPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_Sphere"),
		CCollider::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	/* For. Prototype_GameObject_Camera */

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AssaultRifle"),
		CAssaultRifle::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pipe"),
		CPipe::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Pistol"),
		CPistol::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RocketLauncher"),
		CRocketLauncher::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FPSCamera"),
		CFPSCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// TextObject
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Text"),
		CTextObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_AnimationText"),
		CAnimationText::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_TileObject"),
		CTileObject::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Door"),
		CDoor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Monsters
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_DefaultMutant"),
		CDefaultMutant::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Assassin"),
		CCyborgAssassin::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_CortexReaver"),
		CCortexReaver::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Enforcer"),
		CCyborgEnforcer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Elite"),
		CCyborgElite::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Interactives
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Elevator"),
		CElevator::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RespawnChamber"),
		CRespawnChamber::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Ammunition
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rocket"),
		CRocket::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Grenade"),
		CGrenade::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Bullet"),
		CBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Card"),
		CCard::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//Ammunition - loads.
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RocketPack"),
		CRocketPack::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_PistolBulletPack"),
		CPistolBulletPack::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_RifleBulletPack"),
		CRifleBulletPack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	// Effect
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Thunder"),
		CThunder::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_GunFire"), CEffect_GunFire::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BombBlow"), CEffect_BombBlow::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fireball"), CFireball::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BloodHit"), CEffect_BloodHit::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BulletHit"), CEffect_BulletHit::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Blast"), CEffect_Blast::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossScrew"), CParticle_BossScrew::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BossScrew_Short"), CParticle_BossScrew_Short::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Particle_Snow"), CParticle_Snow::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_EnemyGunFire"), CEffect_EnemyGunFire::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Muzzle"), CEffect_Muzzle::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Muzzle_Flame"), CEffect_Muzzle_Flame::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_BulletCrater"), CEffect_BulletCrater::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다."));

	m_isFinished = true;

	return S_OK;
}

CLoader * CLoader::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, LEVELID eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed to Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	__super::Free();

	WaitForSingleObject(m_hThread, INFINITE);

	DeleteObject(m_hThread);

	CloseHandle(m_hThread);

	DeleteCriticalSection(&m_CriticalSection);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
