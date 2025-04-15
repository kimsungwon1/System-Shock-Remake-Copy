#include "stdafx.h"
#include "FPSPlayer.h"
#include "Weapon.h"
#include "Navigation.h"

#include "Physics.h"

#include "FPS_Arm.h"
#include "FPSCamera.h"
#include "Grenade.h"

#include "Inventory.h"

#include "Card.h"

#include "UnitObject.h"

#include "AnimationText.h"

#include "UI_Info.h"

#include "SoundMgr.h"

#include "GameInstance.h"

_float CFPSPlayer::s_fHeight = 1.7f;

CFPSPlayer::CFPSPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CContainObject{ pDevice, pContext }
{
}

CFPSPlayer::CFPSPlayer(const CFPSPlayer& Prototype)
    : CContainObject{ Prototype }
{
}

HRESULT CFPSPlayer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFPSPlayer::Initialize(void* pArg)
{
    CGameObject::GAMEOBJECT_DESC Desc{};

    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(180.f);

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if(FAILED(Ready_Components())){
        return E_FAIL;
    }

    if (FAILED(Ready_PartObjects())) {
        return E_FAIL;
    }
    m_pFPSCamera = dynamic_cast<CFPSCamera*>(m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_Camera")));

    m_pAnimText = dynamic_cast<CAnimationText*>(m_pGameInstance->Get_CloneObject_ByIndex(LEVEL_GAMEPLAY, TEXT("Layer_UI")));

    m_pAnimText->Set_Arm(dynamic_cast<CFPS_Arm*>(m_vecParts[PART_BODY]));
    m_iState = STATE_STAND;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, { 2.6f, 0.1f, 5.05f });

    m_pNavigationComs[m_iCurrentLevel]->SetCurrentIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_pColliderCom);

    m_LanternDesc.eType = LIGHT_DESC::TYPE_POINT; m_LanternDesc.fRange = 5.f;
    m_LanternDesc.vAmbient = { 1.f, 1.f, 1.f, 1.f }; m_LanternDesc.vDiffuse = { 1.f,1.f,1.f,1.f };
    m_LanternDesc.vDirection = { 0.f,0.f,0.f,0.f }; m_LanternDesc.vSpecular = { 1.f,1.f,1.f,1.f };

    m_pInventory = dynamic_cast<CInventory*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Inventory"), nullptr));//CInventory::Create();

    auto mat = m_pTransformCom->GetWorldMat();
    mat._42 += s_fHeight;
    m_pFPSCamera->SetMatrix(&mat);

    m_pFPSCamera->Late_Update(1.f);

    return S_OK;
}

void CFPSPlayer::Priority_Update(_float fTimeDelta)
{
    if (m_pGameInstance->IsShowing_Cursor() && !m_pInventory->Is_InvenRender()) {
        return;
    }

    // 애니메이션 텍스트에게 컨트롤받도록...
    m_pAnimText->AnimControl();

    if (m_pAnimText->IsControl())
        return;

    if (m_pPhysicsCom->IsCollided() &&
        !m_pNavigationComs[m_iCurrentLevel]->IsFloating(m_pTransformCom->Get_State(CTransform::STATE_POSITION))) {
        // 너무 세게 부딫치면 데미지

        m_pPhysicsCom->PopPower(4);
    }

    // 걷기 컨트롤
    m_bMoving = false;
    _float3 vTrans;
    if ((m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80) && (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)) {
        m_pTransformCom->Walk(4, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &vTrans, m_pColliderCom);
        m_iState |= PLAYER_STATE::STATE_WALK;
        m_bMoving = true;
    }
    else if ((m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80) && (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)) {
        m_pTransformCom->Walk(5, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &vTrans, m_pColliderCom);
        m_iState |= PLAYER_STATE::STATE_WALK;
        m_bMoving = true;
    }
    else if ((m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80) && (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)) {
        m_pTransformCom->Walk(6, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &vTrans, m_pColliderCom);
        m_iState |= PLAYER_STATE::STATE_WALK;
        m_bMoving = true;
    }
    else if ((m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80) && (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)) {
        m_pTransformCom->Walk(7, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &vTrans, m_pColliderCom);
        m_iState |= PLAYER_STATE::STATE_WALK;
        m_bMoving = true;
    }
    else if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
    {
        m_pTransformCom->Walk(0, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &vTrans, m_pColliderCom);
        m_iState |= PLAYER_STATE::STATE_WALK;
        m_bMoving = true;
    }
    else if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
    {
        m_pTransformCom->Walk(1, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &vTrans, m_pColliderCom);
        m_iState |= PLAYER_STATE::STATE_WALK;
        m_bMoving = true;
    }
    else if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
    {
        m_pTransformCom->Walk(2, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &vTrans, m_pColliderCom);
        m_iState |= PLAYER_STATE::STATE_WALK;
        m_bMoving = true;
    }
    else if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
    {
        m_pTransformCom->Walk(3, fTimeDelta, m_pNavigationComs[m_iCurrentLevel], &vTrans, m_pColliderCom);
        m_iState |= PLAYER_STATE::STATE_WALK;
        m_bMoving = true;
    }
    if (m_pGameInstance->Get_RealDIKeyStateDown(DIK_SPACE) &&
        !m_pNavigationComs[m_iCurrentLevel]->IsFloating(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
    {
        CSoundMgr::Get_Instance()->PlaySound(TEXT("Player_Jump"), 1.f);

        m_pPhysicsCom->ReceivePowerVector(_vector{ 0.f,m_fJumpPerSec,0.f });
        m_bMoving = true;
    }

    // 수류탄 투척
    if (m_bGrenadeThrowing) {
        m_vGrenadeCoolTime.x += fTimeDelta;
        if (m_vGrenadeCoolTime.x >= m_vGrenadeCoolTime.y)
        {
            m_bGrenadeThrowing = false;
            m_vGrenadeCoolTime.x = 0.f;
        }
    }

    if (m_pGameInstance->Get_RealDIKeyStateDown(DIK_G) && !m_bGrenadeThrowing) {
        m_bGrenadeThrowing = true;
        m_vecParts[PART_BODY]->Set_Order(Throw);

        CGrenade::Grenade_DESC desc = {};
        desc.bShot = true; desc.fSpeedPerSec = 15.f; desc.fRotationPerSec = 1.f; desc.pShooter = this; desc.iCurrentLevel = m_iCurrentLevel;
        XMStoreFloat4x4(&desc.transMat, XMMatrixIdentity());
        _vector position = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        position += m_pTransformCom->Get_State(CTransform::STATE_UP);

        XMStoreFloat3((_float3*)(&desc.transMat.m[3]), position); desc.bTargetSet = false;
        XMStoreFloat3(&desc.vTargetPos, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

        XMStoreFloat3(&desc.vThrowdir, XMVector3Normalize(XMVector3TransformNormal(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), -XMConvertToRadians(30.f)))) * desc.fSpeedPerSec);

        if (FAILED(m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FlyingBullet"), TEXT("Prototype_GameObject_Grenade"), &desc))) {
            int a = 10;
        }
    }

    // 피직스 바인드
    m_pTransformCom->Bind_Physics(fTimeDelta, m_pPhysicsCom, m_pNavigationComs[m_iCurrentLevel],
        m_pColliderCom);


    if (!m_bMoving) {
        if (m_iState & STATE_WALK)
            m_iState ^= STATE_WALK;
        m_iState |= STATE_STAND;
    }
    else {
        if (m_iState & STATE_STAND)
            m_iState ^= STATE_STAND;
        m_iState |= STATE_WALK;
    }

    //무기 바꾸기 1~9번
    for (size_t i = DIKEYBOARD_1; i <= DIKEYBOARD_9; i++) {
        if (m_pGameInstance->Get_RealDIKeyStateDown(i)) {
            switch_Weapon((_int)i - DIKEYBOARD_1);
        }
    }
    auto* pWeapon = dynamic_cast<CWeapon*>(m_vecParts[PART_WEAPON]);

    

    // 랜턴 켜기 끄기
    if (m_pGameInstance->Get_RealDIKeyStateDown(DIK_F)) {
        if (!m_bLightOnOff) {
            XMStoreFloat4(&m_LanternDesc.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.5f, 0.f, 0.f));
            m_pGameInstance->Add_Light(m_LanternDesc, -1);
            m_bLightOnOff = true;
        }
        else {
            m_pGameInstance->Delete_Light(-1);
            m_bLightOnOff = false;
        }
    }

    // 인벤 켜기 끄기
    if (m_pGameInstance->Get_RealDIKeyStateDown(DIK_I)) {
        m_pInventory->Set_InvenRender(!m_pInventory->Is_InvenRender());
    }
    
    if (!m_pGameInstance->IsTimeSlowing()) {
        m_pGameInstance->SetBlur(false);
    }
    if (m_pGameInstance->Get_RealDIKeyStateDown(DIK_T) && !m_pGameInstance->IsTimeSlowing()) {
        m_pGameInstance->Reserve_Timealter(0.1f, 0.5f, 5.f, TEXT("Layer_Player"));
        m_pGameInstance->SetBlur(true);
    }

    int arr[2] = { 1, 0 };

    _long		MouseMove = { 0 };

    if(!m_pInventory->Is_InvenRender())
    {
        // 마우스 턴
        if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM_X))
        {
            m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.2f);
        }

        if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMM_Y))
        {
            m_fPitchY += m_pTransformCom->GetRotationPerSec() * (fTimeDelta * MouseMove * 0.2f);
            _float fRadian89 = XMConvertToRadians(89.f);
            _float fRadianM89 = XMConvertToRadians(-89.f);
            _float fSet = fTimeDelta * MouseMove * 0.2f;
            if (m_fPitchY > fRadian89) {
                _float fCha = m_fPitchY - fRadian89;
                fSet = (fSet * m_pTransformCom->GetRotationPerSec() - fCha) / m_pTransformCom->GetRotationPerSec();
                m_fPitchY = fRadian89;
            }
            if (m_fPitchY < fRadianM89) {
                _float fCha = m_fPitchY - fRadianM89;
                fSet = (fSet * m_pTransformCom->GetRotationPerSec() - fCha) / m_pTransformCom->GetRotationPerSec();
                m_fPitchY = fRadianM89;
            }

            m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fSet);
        }
    }

    if(pWeapon != nullptr && !m_pInventory->Is_InvenRender())
    {
        auto t = pWeapon->GetItemType();
        switch (t)
        {
        case CWeapon::TYPE_AssaultRifle:
            control_With_AssaultRifle(pWeapon, fTimeDelta);
            break;

        case CWeapon::TYPE_Pistol:
            control_With_Pistol(pWeapon, fTimeDelta);
            break;

        case CWeapon::TYPE_RocketLauncher:
            control_With_RocketLauncher(pWeapon, fTimeDelta);
            break;
        }
    }
    else if(!m_pInventory->Is_InvenRender()){
        control_With_Fist(fTimeDelta);
    }

    for (auto& pPartObject : m_vecParts)
    {
        if (pPartObject == nullptr)
            continue;
        pPartObject->Priority_Update(fTimeDelta);
    }

    m_pInventory->Priority_Update(fTimeDelta);
}

void CFPSPlayer::Update(_float fTimeDelta)
{
    // 카메라 셋
    auto mat = m_pTransformCom->GetWorldMat();
    mat._42 += s_fHeight;
    m_pFPSCamera->SetMatrix(&mat);

    for (auto& pPartObject : m_vecParts)
    {
        if (pPartObject == nullptr)
            continue;
        pPartObject->Update(fTimeDelta);
    }
    m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix_Ptr());

    m_pInventory->Update(fTimeDelta);

    //아이템 줍기
    auto& refWeaponList = *m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Item"));
    for (auto* pIter : refWeaponList) {
        auto* pItem = dynamic_cast<CItem*>(pIter);
        if (pItem->Is_Possible_Looted(this)) {

            if (m_pGameInstance->Get_RealDIKeyStateDown(DIK_E)) {

                if (pItem == nullptr)
                    continue;
                if (pItem->Is_OnInven()) {
                    continue;
                }
                pItem->DeleteInfoUI();

                _bool bPushSuccess = false;
                if (pItem->GetItemType() != CItem::TYPE_KeyCard) {
                    bPushSuccess = m_pInventory->PushItem(pItem);
                }

                if (!bPushSuccess && pItem->GetItemType() != CItem::TYPE_KeyCard)
                    break;

                pItem->Set_OnInven(true);
                pItem->Set_Influencer(this, nullptr, nullptr);
                pItem->Set_Offset();

                if (pItem->GetItemType() < 10 && m_Weapons[pItem->GetItemType()] == nullptr) {
                    auto* pWeapon = dynamic_cast<CWeapon*>(pItem);
                    m_Weapons[pItem->GetItemType()] = pWeapon;
                    Safe_AddRef(pWeapon);
                }
                if (pItem->GetItemType() == CItem::TYPE_KeyCard) {
                    auto* pCard = dynamic_cast<CCard*>(pItem);
                    _int iCardData = pCard->GetInfo(&iCardData);
                    _bool bPass = true;
                    for (auto pC : m_vecCards) {
                        _int i = pC->GetInfo(&i);
                        if (i == iCardData) {
                            bPass = false;
                        }
                    }
                    if (bPass == true) {
                        m_vecCards.push_back(pCard);
                    }
                }
                break;
            }
        }
    }
}

void CFPSPlayer::Late_Update(_float fTimeDelta)
{
    for (auto& pPartObject : m_vecParts)
    {
        if (pPartObject == nullptr)
            continue;
        pPartObject->Late_Update(fTimeDelta);
    }
    XMStoreFloat4(&m_LanternDesc.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 0.5f, 0.f, 0.f));
    m_pGameInstance->Change_Light(m_LanternDesc, -1);

    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);


    if (m_iCurrentWeapon == CWeapon::TYPE_RocketLauncher) {
        m_vecParts[PART_BODY]->SetRenderOnOff(false);
    }
    else {
        m_vecParts[PART_BODY]->SetRenderOnOff(true);
    }

    if (m_bTurnedInven) {
        m_pInventory->Late_Update(fTimeDelta);
    }
}

HRESULT CFPSPlayer::Render()
{
#ifdef _DEBUG
    m_pColliderCom->Render();
    m_pNavigationComs[m_iCurrentLevel]->Render();
#endif
    _wstring str = to_wstring(m_iCurrentLoad);

    //m_pGameInstance->Render_Text(TEXT("font1"), str.c_str(), XMVectorSet(_float(500), _float(500), 1.f, 1.f));

    return S_OK;
}

void CFPSPlayer::ReceivePhysicsPower(_fvector vPower)
{
    m_pPhysicsCom->ReceivePowerVector(vPower);
}

void CFPSPlayer::SetPositionNavi(_fvector vPosition)
{
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    m_pNavigationComs[m_iCurrentLevel]->SetCurrentIndex(vPosition, m_pColliderCom);
}

void CFPSPlayer::control_With_AssaultRifle(class CWeapon* pWeapon, _float fTimeDelta)
{
    m_iCurrentLoad = m_pInventory->GetBulletNum(CItem::TYPE_RifleBullets);
    if (m_fAttackedTime >= 0.f) {
        m_fAttackedTime += fTimeDelta;
    }
    
    //Animation Control
    
    if (m_iState & STATE_WALK && !(m_iState & STATE_ATTACK) && !(m_iState & STATE_RELOAD)) {
        m_pArm->SetUp_Animation(ANIM_STATE::ASSAULTRIFLE_IDLE_MOVING, true);
    }
    if (m_iState & STATE_STAND && !(m_iState & STATE_ATTACK) && !(m_iState & STATE_RELOAD)) {
        m_pArm->SetUp_Animation(ANIM_STATE::ASSAULTRIFLE_IDLE, true);
    }
    if (!(m_iState & STATE_RELOAD) && m_pGameInstance->Get_DIMouseState(MOUSEKEYSTATE::DIMK_LBUTTON)) {
        if ((m_fAttackedTime == -1.f || m_fAttackedTime > pWeapon->GetAttackDuration()) && m_iCurrentLoad > 0) {
            _float2 recoiled = pWeapon->Shoot();
            m_pInventory->PopBullet(CItem::TYPE_RifleBullets);

            m_fAttackedTime = 0.f;
            m_iState |= STATE_ATTACK;
            m_pArm->SetUp_Animation_Immediately(ANIM_STATE::ASSAULTRIFLE_SHOOT, false);
            beRecoiled(recoiled);
        }
    }
    // 공격 애니메이션 끝남
    if (m_iState & STATE_ATTACK && m_fAttackedTime > pWeapon->GetAttackDuration()) {
        m_fAttackedTime = -1.f;
        m_iState ^= STATE_ATTACK;
    }
}

void CFPSPlayer::control_With_Pistol(CWeapon* pWeapon, _float fTimeDelta)
{
    m_iCurrentLoad = m_pInventory->GetBulletNum(CItem::TYPE_PistolBullets);
    if (m_fAttackedTime >= 0.f) {
        m_fAttackedTime += fTimeDelta;
    }

    //Animation Control
    if (m_iState & STATE_WALK && !(m_iState & STATE_ATTACK) && !(m_iState & STATE_RELOAD)) {
        m_pArm->SetUp_Animation(ANIM_STATE::P96_Moving, true);
    }
    if (m_iState & STATE_STAND && !(m_iState & STATE_ATTACK) && !(m_iState & STATE_RELOAD)) {
        m_pArm->SetUp_Animation(ANIM_STATE::P96_Idle, true);
    }
    if (m_pGameInstance->Get_RealDIMouseStateDown(MOUSEKEYSTATE::DIMK_LBUTTON) && !(m_iState & STATE_RELOAD)) {
        if ((m_fAttackedTime == -1.f || m_fAttackedTime > pWeapon->GetAttackDuration()) && m_iCurrentLoad > 0) {
            m_fAttackedTime = 0.f;
            m_iState |= STATE_ATTACK;
            m_pArm->SetUp_Animation_Immediately(ANIM_STATE::P96_Shoot, false);

            _float2 recoiled = pWeapon->Shoot();            
            beRecoiled(recoiled);
            m_pInventory->PopBullet(CItem::TYPE_PistolBullets);
        }
    }
    // 공격 애니메이션 끝남
    if (m_iState & STATE_ATTACK && m_fAttackedTime > pWeapon->GetAttackDuration()) {
        m_fAttackedTime = -1.f;
        m_iState ^= STATE_ATTACK;
    }
}

void CFPSPlayer::control_With_RocketLauncher(CWeapon* pWeapon, _float fTimeDelta)
{
    m_iCurrentLoad = m_pInventory->GetBulletNum(CItem::TYPE_Rockets);
    if (m_fAttackedTime >= 0.f) {
        m_fAttackedTime += fTimeDelta;
    }

    // Animation Control
    if (m_iState & STATE_WALK && !(m_iState & STATE_ATTACK)) {
        m_pArm->SetUp_Animation(ANIM_STATE::ASSAULTRIFLE_IDLE_MOVING, true);
    }
    if (m_iState & STATE_STAND && !(m_iState & STATE_ATTACK)) {
        m_pArm->SetUp_Animation(ANIM_STATE::ASSAULTRIFLE_IDLE, true);
    }
    if (m_pGameInstance->Get_RealDIMouseStateDown(MOUSEKEYSTATE::DIMK_LBUTTON)) {
        if ((m_fAttackedTime == -1.f || m_fAttackedTime > pWeapon->GetAttackDuration()) && m_iCurrentLoad > 0) {
            m_fAttackedTime = 0.f;
            m_iState |= STATE_ATTACK;
            m_pArm->SetUp_Animation_Immediately(ANIM_STATE::ASSAULTRIFLE_SHOOT, false);
            _float2 recoiled = pWeapon->Shoot();
            beRecoiled(recoiled);

            m_pInventory->PopBullet(CItem::TYPE_Rockets);
        }
    }
    // 공격 애니메이션 끝남
    if (m_iState & STATE_ATTACK && m_fAttackedTime > pWeapon->GetAttackDuration()) {
        m_fAttackedTime = -1.f;
        m_iState ^= STATE_ATTACK;
    }
}

void CFPSPlayer::control_With_Fist(_float fTimeDelta)
{
    if (m_fAttackedTime >= 0.f) {
        m_fAttackedTime += fTimeDelta;
    }

    if (m_pGameInstance->Get_RealDIMouseStateDown(MOUSEKEYSTATE::DIMK_LBUTTON) && !(m_iState & STATE_ATTACK)) {
        if (m_fAttackedTime == -1.f) {
            m_fAttackedTime = 0.f;
            m_iState |= STATE_ATTACK;
            _int rand = (_int)m_pGameInstance->Get_Random(0.f, 2.f);
            _int nextAnim = ANIM_STATE::Punch_Left;
            if (rand != 0) {
                nextAnim = ANIM_STATE::Punch_Right;
            }
            m_pArm->SetUp_Animation(nextAnim, false);

            // 데미지 입히기
            auto& monsterList = *m_pGameInstance->Get_RefGameObjects(LEVEL_GAMEPLAY, TEXT("Layer_Enemy"));
            for (auto* pMon : monsterList) {
                _float3 vf = {};
                _float fDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pMon->GetTransformCom()->Get_State(CTransform::STATE_POSITION)));
                if (fDistance < 1.5f && pMon->IsPicking(&vf)) {
                    auto* dynamy = dynamic_cast<CUnitObject*>(pMon);
                    dynamy->Set_Damage(1);
                    break;
                }
            }
        }
    }
    // 공격 애니메이션 끝남
    if (m_iState & STATE_ATTACK && m_pArm->IsAnimFinished()) {
        m_iState ^= STATE_ATTACK;
        m_fAttackedTime = -1.f;
    }
}

HRESULT CFPSPlayer::switch_Weapon(_int iIndex)
{
    if (m_Weapons[iIndex] == nullptr) {
        return E_FAIL;
    }
    auto* pSocketMat = dynamic_cast<CFPS_Arm*>(m_vecParts[PART_BODY])->Get_BoneMatrix_Ptr("lead_gun"); 

    auto* equipedWeapon = dynamic_cast<CWeapon*>(m_vecParts[PART_WEAPON]);
    if (equipedWeapon != nullptr) {
        equipedWeapon->Set_Influencer(this, nullptr, nullptr);
        Safe_Release(equipedWeapon);
    }
    if (m_iCurrentWeapon == iIndex) {
        m_vecParts[PART_WEAPON] = nullptr;
        m_iCurrentWeapon = -1;
        m_pArm->SetUp_Animation(DO_NOTHING, true);
        m_iState = STATE_STAND;
    }
    else {
        m_iCurrentWeapon = iIndex;
        m_vecParts[PART_WEAPON] = m_Weapons[iIndex];
        m_Weapons[iIndex]->Set_Influencer(this, m_pTransformCom->Get_WorldMatrix_Ptr(), pSocketMat);
        Safe_AddRef(m_Weapons[iIndex]);
    }
    return S_OK;
}

void CFPSPlayer::set_Reload_Anim()
{
    _int info[1] = { 0 };
    switch (m_vecParts[PART_WEAPON]->GetInfo(info)) {
    case CWeapon::TYPE_AssaultRifle:
        m_vecParts[PART_BODY]->Set_Order(ASSAULTRIFLE_RELOAD);
        break;

    case CWeapon::TYPE_Pistol:
        m_vecParts[PART_BODY]->Set_Order(P96_Reload);
        break;
    }
}

void CFPSPlayer::reloadWeapon()
{
    
}

_bool CFPSPlayer::isReloading()
{
    auto* pArm = dynamic_cast<CFPS_Arm*>(m_vecParts[PART_BODY]);

    if (pArm->Get_iCurrentAnimIndex() == ASSAULTRIFLE_RELOAD || pArm->Get_iCurrentAnimIndex() == P96_Reload) {
        return true;
    }

    return false;
}

void CFPSPlayer::beRecoiled(const _float2& vRecoil)
{
    //반동으로 튐.
    _float fRadianRecoilHorizon = XMConvertToRadians(vRecoil.x);
    _float fRadianRecoilVertical = -XMConvertToRadians(vRecoil.y);
    _float fRadian89 = XMConvertToRadians(89.f);
    _float fRadianM89 = XMConvertToRadians(-89.f);
    if (m_pGameInstance->Get_Random(0.f, 1.f) < 0.5f)
        fRadianRecoilHorizon = -fRadianRecoilHorizon;

    m_fPitchY += m_pTransformCom->GetRotationPerSec() * fRadianRecoilVertical;
    if (m_fPitchY > fRadian89) {
        _float fCha = m_fPitchY - fRadian89;
        fRadianRecoilVertical = (fRadianRecoilVertical * m_pTransformCom->GetRotationPerSec() - fCha) / m_pTransformCom->GetRotationPerSec();
        m_fPitchY = fRadian89;
    }
    if (m_fPitchY < fRadianM89) {
        _float fCha = m_fPitchY - fRadianM89;
        fRadianRecoilVertical = (fRadianRecoilVertical * m_pTransformCom->GetRotationPerSec() - fCha) / m_pTransformCom->GetRotationPerSec();
        m_fPitchY = fRadianM89;
    }

    m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), fRadianRecoilVertical);
    m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRadianRecoilHorizon);
}

_bool CFPSPlayer::IsThisDoorOpenable(_int iDoorData)
{
    for (auto iCard : m_vecCards) {
        _int iData = iCard->GetInfo(&iData);
        if (iDoorData == iData) {
            return true;
        }
    }

    return false;
    //return true;
}

void CFPSPlayer::ReceiveDamage(_int iDamage)
{
    _float fRand = m_pGameInstance->Get_Random(0.f, 7.f);

    if(fRand < 1.f)
    {
        CSoundMgr::Get_Instance()->PlaySound(TEXT("Player_Hit"), 1.f);
    }

    m_iHP -= iDamage;
}

HRESULT CFPSPlayer::Ready_Components()
{
    CNavigation::NAVIGATION_DESC			NaviDesc{};

    NaviDesc.iCurrentIndex = 0;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation1"),
        TEXT("Com_Navigation1"), reinterpret_cast<CComponent**>(&m_pNavigationComs[LEVEL1]), &NaviDesc)))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation2"),
        TEXT("Com_Navigation2"), reinterpret_cast<CComponent**>(&m_pNavigationComs[LEVEL2]), &NaviDesc)))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation3"),
        TEXT("Com_Navigation3"), reinterpret_cast<CComponent**>(&m_pNavigationComs[LEVEL_BOSS]), &NaviDesc)))
        return E_FAIL;

    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

    aabbDesc.vExtents = { 0.5f, 1.f, 0.5f };
    aabbDesc.vCenter = { 0.f, aabbDesc.vExtents.y, 0.f };
    aabbDesc.bIsPlayers = true;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"),
        TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbDesc)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Physics"),
        TEXT("Com_Physics"), reinterpret_cast<CComponent**>(&m_pPhysicsCom))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Collider(m_pColliderCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFPSPlayer::Ready_PartObjects()
{
    m_vecParts.resize(PART_END, nullptr);

    CFPS_Arm::Player_Arm_DESC desc = {};
    desc.pParentTransform = m_pTransformCom->Get_WorldMatrix_Ptr();
    desc.iParentState = &m_iState;
    Add_PartObject(PART_BODY, TEXT("Prototype_GameObject_FPS_Arm"), &desc);

    m_pArm = dynamic_cast<CFPS_Arm*>(m_vecParts[PART_BODY]);

    /*CWeapon::Weapon_DESC rifeDesc = {}; 
    auto* pRifle = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_AssaultRifle"), &rifeDesc));
    if (pRifle == nullptr) {
        return E_FAIL;
    }
    m_Weapons[CWeapon::TYPE_AssaultRifle] = pRifle;
    m_Weapons[CWeapon::TYPE_AssaultRifle]->Set_Offset();*/

    CWeapon::Weapon_DESC pistolDesc = {};
    auto* pPistol = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_Pistol"), &pistolDesc));
    if (pPistol == nullptr) {
        return E_FAIL;
    }
    m_Weapons[CWeapon::TYPE_Pistol] = pPistol;

    /*CWeapon::Weapon_DESC launcherDesc = {};
    auto* pLauncher = dynamic_cast<CWeapon*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_RocketLauncher"), &launcherDesc));
    if (pLauncher == nullptr) {
        return E_FAIL;
    }
    m_Weapons[CWeapon::TYPE_RocketLauncher] = pLauncher;
    m_Weapons[CWeapon::TYPE_RocketLauncher]->Set_Offset();*/

    return S_OK;
}

CFPS_Arm* CFPSPlayer::GetArm()
{
    return dynamic_cast<CFPS_Arm*>(m_vecParts[PART_BODY]);
}

CFPSPlayer* CFPSPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFPSPlayer* pInstance = new CFPSPlayer(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Created : CFPSPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}



CGameObject* CFPSPlayer::Clone(void* pArg)
{
    CFPSPlayer* pInstance = new CFPSPlayer(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CFPSPlayer"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFPSPlayer::Free()
{
    __super::Free();
    for (auto* pNavi : m_pNavigationComs) {
        Safe_Release(pNavi);
    }
    for (int i = 0; i < 10; i++) {
        Safe_Release(m_Weapons[i]);
    }
    m_pGameInstance->Delete_Collider(m_pColliderCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pPhysicsCom);
    Safe_Release(m_pInventory);
}
