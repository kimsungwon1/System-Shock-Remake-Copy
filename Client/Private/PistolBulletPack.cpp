#include "stdafx.h"

#include "PistolBulletPack.h"

#include "GameInstance.h"

CPistolBulletPack::CPistolBulletPack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAmmunitionPack(pDevice, pContext)
{
}

CPistolBulletPack::CPistolBulletPack(const CPistolBulletPack& Prototype)
    : CAmmunitionPack(Prototype)
{
}

HRESULT CPistolBulletPack::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPistolBulletPack::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_eItemType = CItem::ItemType::TYPE_PistolBullets;

    m_strName = TEXT("Pistol Bullets");

    return S_OK;
}

void CPistolBulletPack::Priority_Update(_float fTimeDelta)
{
}

void CPistolBulletPack::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CPistolBulletPack::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);//m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CPistolBulletPack::Render()
{
    return __super::Render();
}

HRESULT CPistolBulletPack::Ready_Component()
{
    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

    aabbDesc.vExtents = { 1.f, 1.f, 1.f };
    aabbDesc.vCenter = { 0.f,0.5f,0.f };
    aabbDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbDesc))) {
        return E_FAIL;
    }

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Crate_Pistol"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)))) {
        return E_FAIL;
    }

    return S_OK;
}

CPistolBulletPack* CPistolBulletPack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPistolBulletPack* pInstance = new CPistolBulletPack(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Initialize : Create CPistolBulletPack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPistolBulletPack::Clone(void* pArg)
{
    CPistolBulletPack* pInstance = new CPistolBulletPack(*this);

    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX(TEXT("Failed to Initialize : Clone CPistolBulletPack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPistolBulletPack::Free()
{
    __super::Free();
}