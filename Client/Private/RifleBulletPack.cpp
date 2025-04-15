#include "stdafx.h"

#include "RifleBulletPack.h"

#include "GameInstance.h"

CRifleBulletPack::CRifleBulletPack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAmmunitionPack(pDevice, pContext)
{
}

CRifleBulletPack::CRifleBulletPack(const CRifleBulletPack& Prototype)
    : CAmmunitionPack(Prototype)
{
}

HRESULT CRifleBulletPack::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRifleBulletPack::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_eItemType = CItem::ItemType::TYPE_RifleBullets;

    m_strName = TEXT("Rifle Bullets");

    return S_OK;
}

void CRifleBulletPack::Priority_Update(_float fTimeDelta)
{
}

void CRifleBulletPack::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CRifleBulletPack::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);//m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CRifleBulletPack::Render()
{
    return __super::Render();
}

HRESULT CRifleBulletPack::Ready_Component()
{
    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

    aabbDesc.vExtents = { 1.f, 1.f, 1.f };
    aabbDesc.vCenter = { 0.f,0.5f,0.f };
    aabbDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbDesc))) {
        return E_FAIL;
    }

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Crate_Rifle"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)))) {
        return E_FAIL;
    }

    return S_OK;
}

CRifleBulletPack* CRifleBulletPack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRifleBulletPack* pInstance = new CRifleBulletPack(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Initialize : Create CRifleBulletPack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRifleBulletPack::Clone(void* pArg)
{
    CRifleBulletPack* pInstance = new CRifleBulletPack(*this);

    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX(TEXT("Failed to Initialize : Clone CRifleBulletPack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRifleBulletPack::Free()
{
    __super::Free();
}