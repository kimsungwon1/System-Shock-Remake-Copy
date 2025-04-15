#include "stdafx.h"
#include "Client_Defines.h"
#include "FPSPlayer.h"
#include "Pistol.h"
#include "Ammunition.h"
#include "Effect_GunFire.h"
#include "SoundMgr.h"

#include "GameInstance.h"

CPistol::CPistol(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon(pDevice, pContext)
{
}

CPistol::CPistol(const CWeapon& Prototype)
    : CWeapon(Prototype)
{
}

HRESULT CPistol::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPistol::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Pistol"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)))) {
        return E_FAIL;
    }

    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};
    aabbDesc.vExtents = { 0.5f, 1.f, 0.5f };
    aabbDesc.vCenter = { 0.f, aabbDesc.vExtents.y, 0.f };
    aabbDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbDesc))) {
        return E_FAIL;
    }

    _matrix worldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    _float4x4 resultMat = {};

    worldMat = XMMatrixRotationY(XMConvertToRadians(-90.f));

    worldMat.r[3].m128_f32[0] = -0.15f;
    worldMat.r[3].m128_f32[1] = -0.21f;
    worldMat.r[3].m128_f32[2] = 0.02f;

    XMStoreFloat4x4(&resultMat, worldMat);
    m_pTransformCom->SetWorldMat(resultMat);

    m_eItemType = TYPE_Pistol;
    m_fAttackDuration = 0.2f;

    m_iMaxLoadNum = 15;

    m_vRecoil = { 0.1f, 0.3f };

    m_strName = TEXT("Mini Pistol");

    return S_OK;
}

void CPistol::Priority_Update(_float fTimeDelta)
{
}

void CPistol::Update(_float fTimeDelta)
{
    _matrix socketMatrix = XMLoadFloat4x4(m_pSocketTransform);

    for (size_t i = 0; i < 3; i++) {
        socketMatrix.r[i] = XMVector3Normalize(socketMatrix.r[i]);
    }

    _matrix parentMatrix = XMLoadFloat4x4(m_pParentTransform);
    parentMatrix.r[3].m128_f32[1] += CFPSPlayer::Get_Height();
    _matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * socketMatrix * parentMatrix;

    XMStoreFloat4x4(&m_WorldMatrix, worldMatrix);
}

void CPistol::Late_Update(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPistol::Render()
{
    return __super::Render();
}

_float2 CPistol::Shoot()
{
    CAmmunition::Ammunition_Desc desc = {};
    desc.bShot = true; desc.fRotationPerSec = XMConvertToRadians(90.f); desc.fSpeedPerSec = 50.f;
    desc.iDamage = 1; desc.pShooter = m_pInfluencer;  desc.bTargetSet = false;
    XMStoreFloat3(&desc.vTargetPos, m_pInfluencer->GetTransformCom()->Get_State(CTransform::STATE_LOOK));

    _vector position = XMLoadFloat3((_float3*)(&m_WorldMatrix._41));
    XMStoreFloat3((_float3*)(&desc.transMat.m[3]), position);

    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FlyingBullet"), TEXT("Prototype_GameObject_Bullet"), &desc);


    CEffect_GunFire::GUNFIRE_DESC shotEffectDesc = {}; shotEffectDesc.pWorldMatrixOrigin = &m_WorldMatrix;
    shotEffectDesc.fRotationPerSec = 1.f; shotEffectDesc.fSpeedPerSec = 1.f;
    shotEffectDesc.fOffsets[0] = 1.f; shotEffectDesc.fOffsets[1] = 0.02f;

    auto* pSound = CSoundMgr::Get_Instance();
    _float fRand = m_pGameInstance->Get_Random(0.f, 4.f);
    
    if (fRand < 1.f) {
        pSound->PlaySound(TEXT("MiniPistol_Fire_Main_01"), 1.f);
    }
    else if (fRand < 2.f) {
        pSound->PlaySound(TEXT("MiniPistol_Fire_Main_02"), 1.f);
    }
    else if (fRand < 3.f) {
        pSound->PlaySound(TEXT("MiniPistol_Fire_Main_03"), 1.f);
    }
    else {
        pSound->PlaySound(TEXT("MiniPistol_Fire_Main_04"), 1.f);
    }

    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_GunFire"), &shotEffectDesc);

    return m_vRecoil;
}

CPistol* CPistol::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPistol* pInstance = new CPistol(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Initialize : Create CPistol"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPistol::Clone(void* pArg)
{
    CPistol* pInstance = new CPistol(*this);

    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX(TEXT("Failed to Initialize : Clone CPistol"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPistol::Free()
{
    __super::Free();
}
