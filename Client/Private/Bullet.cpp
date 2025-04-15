#include "stdafx.h"
#include "Bullet.h"

#include "GameInstance.h"

CBullet::CBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAmmunition{pDevice, pContext}
{
}

CBullet::CBullet(const CBullet& Prototype)
    : CAmmunition { Prototype }
{
}

HRESULT CBullet::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBullet::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg))) {
        return E_FAIL;
    }
    if (FAILED(Ready_Components())) {
        return E_FAIL;
    }

    m_vBlowTime.y = 5.f;

    return S_OK;
}

void CBullet::Priority_Update(_float fTimeDelta)
{
}

void CBullet::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CBullet::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CBullet::Render()
{
    return __super::Render();
}

HRESULT CBullet::Ready_Components()
{
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Bullet"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)))) {
        return E_FAIL;
    }
    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

    aabbDesc.vExtents = { 0.02f, 0.01f, 0.02f };
    aabbDesc.vCenter = { 0.f, 0.01f, 0.f };
    aabbDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbDesc))) {
        return E_FAIL;
    }

    m_eAmmunitionType = Ammunition_Type::TYPE_762mm;

    return S_OK;
}

CBullet* CBullet::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CBullet* pInstance = new CBullet(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Initialize : Create CBullet"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBullet::Clone(void* pArg)
{
    CBullet* pInstance = new CBullet(*this);

    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX(TEXT("Failed to Initialize : Clone CBullet"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBullet::Free()
{
    __super::Free();
}
