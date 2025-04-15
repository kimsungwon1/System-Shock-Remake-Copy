#include "stdafx.h"
#include "Fireball.h"

#include "GameInstance.h"

CFireball::CFireball(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAmmunition{ pDevice, pContext }
{
}

CFireball::CFireball(const CFireball& Prototype)
    : CAmmunition{ Prototype }
{
}

HRESULT CFireball::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CFireball::Initialize(void* pArg)
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

void CFireball::Priority_Update(_float fTimeDelta)
{
}

void CFireball::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CFireball::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CFireball::Render()
{
    return __super::Render();
}

HRESULT CFireball::Ready_Components()
{
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Fireball"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)))) {
        return E_FAIL;
    }
    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

    aabbDesc.vExtents = { 0.05f, 0.05f, 0.05f };
    aabbDesc.vCenter = { 0.f, 0.01f, 0.f };
    aabbDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbDesc))) {
        return E_FAIL;
    }

    m_eAmmunitionType = Ammunition_Type::TYPE_762mm;

    return S_OK;
}

CFireball* CFireball::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFireball* pInstance = new CFireball(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Initialize : Create CFireball"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CFireball::Clone(void* pArg)
{
    CFireball* pInstance = new CFireball(*this);

    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX(TEXT("Failed to Initialize : Clone CFireball"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFireball::Free()
{
    __super::Free();
}