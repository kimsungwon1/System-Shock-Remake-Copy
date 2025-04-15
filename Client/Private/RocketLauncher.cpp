#include "stdafx.h"
#include "Client_Defines.h"
#include "FPSPlayer.h"
#include "RocketLauncher.h"

#include "Ammunition.h"
#include "Effect_GunFire.h"
#include "SoundMgr.h"

#include "GameInstance.h"

CRocketLauncher::CRocketLauncher(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon(pDevice, pContext)
{
}

CRocketLauncher::CRocketLauncher(const CWeapon& Prototype)
    : CWeapon(Prototype)
{
}

HRESULT CRocketLauncher::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRocketLauncher::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Launcher"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)))) {
        return E_FAIL;
    }

    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

    aabbDesc.vExtents = { 0.3f, 0.05f, 0.6f };
    aabbDesc.vCenter = { 0.0f, 0.05f, 0.15f };
    aabbDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbDesc))) {
        return E_FAIL;
    }

    m_eItemType = TYPE_RocketLauncher;
    m_fAttackDuration = 1.f;

    m_iLoadNum = 1;
    m_iMaxLoadNum = 1;

    m_iSlotSizeX = 5; m_iSlotSizeY = 3;

    m_vRecoil = { 0.3f, 1.f };

    m_strName = TEXT("Rocket Launcher");

    return S_OK;
}

void CRocketLauncher::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CRocketLauncher::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    _matrix worldMatrix = {};
    if (m_pSocketTransform != nullptr && m_pParentTransform != nullptr)
    {
        _matrix socketMatrix = XMLoadFloat4x4(m_pSocketTransform);

        for (size_t i = 0; i < 3; i++) {
            socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
        }

        _matrix parentMatrix = XMLoadFloat4x4(m_pParentTransform);
        parentMatrix.r[3].m128_f32[1] += CFPSPlayer::Get_Height();
        worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * socketMatrix * parentMatrix;
    }
    else {
        worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    }

    XMStoreFloat4x4(&m_WorldMatrix, worldMatrix);
}

void CRocketLauncher::Late_Update(_float fTimeDelta)
{
    if (m_bOnInven && m_pSocketTransform == nullptr) {
        return;
    }
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CRocketLauncher::Render()
{
    return __super::Render();
}

void CRocketLauncher::Set_Offset()
{
    _matrix worldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    _float4x4 resultMat = {};

    worldMat = XMMatrixRotationY(XMConvertToRadians(17.f));

    worldMat.r[3].m128_f32[0] = -0.05f;
    worldMat.r[3].m128_f32[1] = -0.35f;
    worldMat.r[3].m128_f32[2] = -0.1f;

    XMStoreFloat4x4(&resultMat, worldMat);
    m_pTransformCom->SetWorldMat(resultMat);
}

_float2 CRocketLauncher::Shoot()
{
    CAmmunition::Ammunition_Desc desc = {};
    desc.bShot = true; desc.fRotationPerSec = XMConvertToRadians(90.f); desc.fSpeedPerSec = 30.f;
    desc.iDamage = 14; desc.pShooter = m_pInfluencer;  desc.bTargetSet = false;
    XMStoreFloat3(&desc.vTargetPos, m_pInfluencer->GetTransformCom()->Get_State(CTransform::STATE_LOOK));

    _vector position = XMLoadFloat3((_float3*)(&m_WorldMatrix._41));
    
    position += XMLoadFloat3((_float3*)m_WorldMatrix.m[0]) * 0.05f;
    position += XMLoadFloat3((_float3*)(m_WorldMatrix.m[1])) * 0.07f;
    XMStoreFloat3((_float3*)(&desc.transMat.m[3]), position);

    auto* pSound = CSoundMgr::Get_Instance();
    _float fRand = m_pGameInstance->Get_Random(0.f, 3.f);

    if (fRand < 1.f) {
        pSound->PlaySound(TEXT("Weapon_GrenadeLauncher_Fire_Main_01"), 1.f);
    }
    else if (fRand < 2.f) {
        pSound->PlaySound(TEXT("Weapon_GrenadeLauncher_Fire_Main_02"), 1.f);
    }
    else if (fRand < 3.f) {
        pSound->PlaySound(TEXT("Weapon_GrenadeLauncher_Fire_Main_03"), 1.f);
    }

    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FlyingBullet"), TEXT("Prototype_GameObject_Rocket"), &desc);

    //Effect ¹ß»ý


    return m_vRecoil;
}

CRocketLauncher* CRocketLauncher::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRocketLauncher* pInstance = new CRocketLauncher(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Initialize : Create CRocketLauncher"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRocketLauncher::Clone(void* pArg)
{
    CRocketLauncher* pInstance = new CRocketLauncher(*this);

    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX(TEXT("Failed to Initialize : Clone CRocketLauncher"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRocketLauncher::Free()
{
    __super::Free();
}
