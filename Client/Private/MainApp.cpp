#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "SoundMgr.h"

#include "LoadingScreen.h"

#include "Level_Loading.h"
#include "GameInstance.h"


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

	/* 엔진을 사용하기위한 여러 초기화 과정을 수행한다. */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	m_pGameInstance->Set_RayCaster_Center();
	m_pGameInstance->Show_Cursor(false);

	if (FAILED(m_pGameInstance->Add_Font(TEXT("font1"), TEXT("../Bin/FontFiles/myfile.spritefont"))))
		int a = 10;

	if (FAILED(Ready_Prototype_Component_Static()))
		return E_FAIL;

	CSoundMgr::Get_Instance()->Initialize();

	if (Ready_Sound())
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_Loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../../Resources/loading.jfif"), 1))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LoadingScreen"),
		CLoadingScreen::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	m_pGameInstance->Update_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Render_Begin();

	m_pGameInstance->Draw_Engine();

	//m_pGameInstance->Render_Text(TEXT("font1"), TEXT("Magic candle"), XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pGameInstance->Render_End();

	return S_OK;
}


HRESULT CMainApp::Ready_Prototype_Component_Static()
{
	

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;	

	///* For.Prototype_Component_Shader_VtxNorTex */
	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
	//	return E_FAIL;

	/* For. Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	return S_OK;
}

HRESULT CMainApp::Open_Level(LEVELID eStartLevelID)
{	
	/* 어떤 레벨을 선택하던 로딩 레벨로 선 진입한다.  */
	/* 로딩레벨에서 내가 선택한 레벨에 필요한 자원을 준비한다. */
	/* 로딩 레벨은 다음레벨(내가 선택한 레벨)이 누구인지를 인지해야한다. */
	if (FAILED(m_pGameInstance->Change_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eStartLevelID))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Sound()
{
	auto* pSoundManager = CSoundMgr::Get_Instance();

	//Music
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Music/Citadel_Synth_Intro_Loop.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Music/Boss.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Music/Level1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Music/Level2.ogg")))
		return E_FAIL;
	//Player
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Player/Player_Hit.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Player/Player_Jump.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Player/Weapon/AssaultRifle_Fire_Main.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Player/Weapon/MiniPistol_Fire_Main_01.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Player/Weapon/MiniPistol_Fire_Main_02.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Player/Weapon/MiniPistol_Fire_Main_03.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Player/Weapon/MiniPistol_Fire_Main_04.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Player/Weapon/Weapon_GrenadeLauncher_Fire_Main_01.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Player/Weapon/Weapon_GrenadeLauncher_Fire_Main_02.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Player/Weapon/Weapon_GrenadeLauncher_Fire_Main_03.ogg")))
		return E_FAIL;

	//Ammunition
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Ammunition/Bomb_Explosion_Main_01.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Ammunition/Bomb_Explosion_Main_02.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Ammunition/Bomb_Explosion_Main_03.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Ammunition/Bomb_Explosion_Main_04.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Ammunition/Bomb_Explosion_Main_05.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Ammunition/Physics_Impact_Bullet_Shared_Flesh_Bullet_01.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Ammunition/Physics_Impact_Bullet_Cortex.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Ammunition/Physics_Impact_Bullet_Tile.ogg")))
		return E_FAIL;

	// Enemies
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Assassin/OnDeath_1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Assassin/OnDeath_2.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Assassin/OnDeath_3.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Assassin/OnDeath_4.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Assassin/OnDeath_5.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Assassin/Fire_Close_1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Assassin/Fire_Close_2.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Assassin/Fire_Close_3.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Assassin/Fire_Close_4.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Mutant/Enemy_HumanoidMutant_Vox_OnMeleeAttack_01.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Mutant/Enemy_HumanoidMutant_Vox_OnMeleeAttack_02.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Mutant/Enemy_HumanoidMutant_Vox_OnMeleeAttack_03.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Mutant/Enemy_HumanoidMutant_Vox_OnMeleeAttack_04.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Mutant/Enemy_HumanoidMutant_Vox_OnMeleeAttack_05.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Mutant/Enemy_HumanoidMutant_Vox_OnMeleeAttack_06.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Mutant/Enemy_HumanoidMutant_Vox_OnMeleeAttack_07.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Mutant/Enemy_HumanoidMutant_Vox_OnMeleeAttack_08.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Mutant/Enemy_HumanoidMutant_Vox_OnMeleeAttack_09.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Mutant/Enemy_HumanoidMutant_Vox_OnMeleeAttack_10.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Mutant/Enemy_HumanoidMutant_Vox_OnDeath_01.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Mutant/Enemy_HumanoidMutant_Vox_OnDeath_02.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Mutant/Enemy_HumanoidMutant_Vox_OnDeath_03.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Mutant/Enemy_HumanoidMutant_Vox_OnDeath_04.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Death_Foley_1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Death_Sweet_1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Death_UI_1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Melee_A_Foley_1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Grenade_1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Grenade_2.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Grenade_3.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Grenade_4.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Shot_1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Shot_2.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Shot_3.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Shot_4.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Shot_6.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Shot_7.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Shot_8.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Shot_9.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Shot_10.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Shot_11.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Rocket_1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Rocket_2.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Rocket_3.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Elec_1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Elec_2.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Elec_3.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/CortexReaver/Enemy_CortexReaver_Attack_Range_Elec_4.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Enforcer/Enemy_CyborgEnforcer_Attack_Range_1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Enforcer/Enemy_CyborgEnforcer_Attack_Range_2.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Enforcer/Enemy_CyborgEnforcer_Vox_State_OnDeath_01.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Enforcer/Enemy_CyborgEnforcer_Vox_State_OnDeath_02.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Enforcer/Enemy_CyborgEnforcer_Vox_State_OnDeath_03.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Enforcer/Enemy_CyborgEnforcer_Vox_State_OnDeath_04.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Enforcer/Enemy_CyborgEnforcer_Vox_State_OnDeath_05.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Enforcer/Enemy_CyborgEnforcer_Vox_State_OnDeath_06.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Vox_State_OnDeath_1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Vox_Appear.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Range_A_Attack_Foley_1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Range_A_Attack_Foley_2.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Range_A_Attack_Foley_3.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Range_A_Attack_Foley_4.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Range_A_Attack_Foley_5.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Range_A_Attack_Foley_6.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Range_GrenadeThrow_Whoosh_1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Range_GrenadeThrow_Whoosh_2.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Range_GrenadeThrow_Whoosh_3.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Range_GrenadeThrow_Whoosh_4.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Range_GrenadeThrow_Whoosh_5.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Servo_1.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Servo_2.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Servo_3.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Servo_4.ogg")))
		return E_FAIL;
	if (FAILED(pSoundManager->LoadSoundFile("../Sound/Enemy/Elite/Enemy_CyborgElite_Servo_5.ogg")))
		return E_FAIL;
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

	CSoundMgr::Destroy_Instance();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	m_pGameInstance->Release_Engine();

	Safe_Release(m_pGameInstance);
	
}

