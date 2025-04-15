#include "stdafx.h"

#include "RocketPack.h"

#include "GameInstance.h"

CRocketPack::CRocketPack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CAmmunitionPack(pDevice, pContext)
{
}

CRocketPack::CRocketPack(const CRocketPack& Prototype)
    : CAmmunitionPack(Prototype)
{
}

HRESULT CRocketPack::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CRocketPack::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_iSlotSizeX = 3; m_iSlotSizeY = 2;

    m_eItemType = ItemType::TYPE_Rockets;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_iAmmunitionSize = 20;

    return S_OK;
}

void CRocketPack::Priority_Update(_float fTimeDelta)
{
}

void CRocketPack::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CRocketPack::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);//m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CRocketPack::Render()
{
    return __super::Render();
}

HRESULT CRocketPack::Ready_Component()
{
    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

    aabbDesc.vExtents = { 0.1f, 0.03f, 0.4f };
    aabbDesc.vCenter = { 0.15f, 0.06f, 0.f };
    aabbDesc.pOwner = this;
    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbDesc))) {
        return E_FAIL;
    }

    if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rocket"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)))) {
        return E_FAIL;
    }

    return S_OK;
}

CRocketPack* CRocketPack::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CRocketPack* pInstance = new CRocketPack(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype())) {
        MSG_BOX(TEXT("Failed to Initialize : Create CRocketPack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CRocketPack::Clone(void* pArg)
{
    CRocketPack* pInstance = new CRocketPack(*this);

    if (FAILED(pInstance->Initialize(pArg))) {
        MSG_BOX(TEXT("Failed to Initialize : Clone CRocketPack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRocketPack::Free()
{
    __super::Free();
}