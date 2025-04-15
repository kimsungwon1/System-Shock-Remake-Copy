#include "stdafx.h"
#include "Client_Defines.h"
#include "FPSPlayer.h"
#include "AssaultRifle.h"

#include "Ammunition.h"
#include "Effect_GunFire.h"
#include "SoundMgr.h"

#include "GameInstance.h"

CAssaultRifle::CAssaultRifle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CWeapon(pDevice, pContext)
{
}

CAssaultRifle::CAssaultRifle(const CWeapon& Prototype)
    : CWeapon(Prototype)
{
}

HRESULT CAssaultRifle::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAssaultRifle::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_AssaultRifle"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)))) {
        return E_FAIL;
    }

    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

    aabbDesc.vExtents = { 0.4f, 0.03f, 0.08f };
    aabbDesc.vCenter = { 0.15f, 0.06f, 0.f };
    aabbDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbDesc))) {
        return E_FAIL;
    }

    m_eItemType = TYPE_AssaultRifle;
    m_fAttackDuration = 0.05f;

    m_iMaxLoadNum = 30;

    m_strName = TEXT("AssaultRifle");

    m_vRecoil = { 0.1f, 0.3f };

    m_iSlotSizeX = 5; m_iSlotSizeY = 3;

    return S_OK;
}

void CAssaultRifle::Priority_Update(_float fTimeDelta)
{
}

void CAssaultRifle::Update(_float fTimeDelta)
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

void CAssaultRifle::Late_Update(_float fTimeDelta)
{
    if (m_bOnInven && m_pSocketTransform == nullptr) {
        return;
    }
    m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CAssaultRifle::Render()
{
    return __super::Render();
}

void CAssaultRifle::Set_Offset()
{
    _matrix worldMat = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    _float4x4 resultMat = {};

    worldMat = XMMatrixRotationY(XMConvertToRadians(-73.f));

    worldMat.r[3].m128_f32[0] = -0.15f;
    worldMat.r[3].m128_f32[1] = -0.2f;
    worldMat.r[3].m128_f32[2] = 0.02f;

    XMStoreFloat4x4(&resultMat, worldMat);
    m_pTransformCom->SetWorldMat(resultMat);
}

_float2 CAssaultRifle::Shoot()
{
    CAmmunition::Ammunition_Desc desc = {};
    desc.bShot = true; desc.fRotationPerSec = XMConvertToRadians(90.f); desc.fSpeedPerSec = 70.f;
    desc.iDamage = 2; desc.pShooter = m_pInfluencer;  desc.bTargetSet = false;
    XMStoreFloat3(&desc.vTargetPos, m_pInfluencer->GetTransformCom()->Get_State(CTransform::STATE_LOOK));

    _vector position = XMLoadFloat3((_float3*)(&m_WorldMatrix._41));
    position += XMLoadFloat3((_float3*)(&m_WorldMatrix._11)) * 0.5f;
    position += XMLoadFloat3((_float3*)(&m_WorldMatrix._21)) * 0.05f;
    XMStoreFloat3((_float3*)(&desc.transMat.m[3]), position);

    //sound
    CSoundMgr::Get_Instance()->PlaySound(TEXT("AssaultRifle_Fire_Main"), 1.f);

    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_FlyingBullet"), TEXT("Prototype_GameObject_Bullet"), &desc);

    //Effect ¹ß»ý
    CEffect_GunFire::GUNFIRE_DESC gDesc = {};
    gDesc.pWorldMatrixOrigin = &m_WorldMatrix;
    gDesc.fRotationPerSec = 1.f; gDesc.fSpeedPerSec = 1.f;
    gDesc.fOffsets[0] = 1.f; gDesc.fOffsets[1] = 0.02f; gDesc.fOffsets[2] = -0.035f;
    gDesc.fAngle = XMConvertToRadians(45.f);

    m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_GunFire"), &gDesc);

    return m_vRecoil;
}

CAssaultRifle* CAssaultRifle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CAssaultRifle* pInstance = new CAssaultRifle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Initialize : Create CAssaultRifle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CAssaultRifle::Clone(void* pArg)
{
    CAssaultRifle* pInstance = new CAssaultRifle(*this);

    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX(TEXT("Failed to Initialize : Clone CAssaultRifle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CAssaultRifle::Free()
{
    __super::Free();
}
