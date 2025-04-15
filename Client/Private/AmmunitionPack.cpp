#include "stdafx.h"

#include "AmmunitionPack.h"

#include "GameInstance.h"

CAmmunitionPack::CAmmunitionPack(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CItem(pDevice, pContext)
{
}

CAmmunitionPack::CAmmunitionPack(const CAmmunitionPack& Prototype)
    : CItem(Prototype)
{
}

HRESULT CAmmunitionPack::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CAmmunitionPack::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    auto* desc = static_cast<AMMUNITIONPACK_DESC*>(pArg);

    m_iAmmunitionSize = 50;// desc->iAmmunitionSize;

    XMStoreFloat4x4(&m_WorldMatrix, m_pTransformCom->GetWorldMat_XMMat());

    return S_OK;
}

void CAmmunitionPack::Priority_Update(_float fTimeDelta)
{
}

void CAmmunitionPack::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CAmmunitionPack::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
    //m_pGameInstance->Add_RenderObject(CRenderer::RG_NONBLEND, this);
}

HRESULT CAmmunitionPack::Render()
{
    return __super::Render();
}

void CAmmunitionPack::JoinTogether(CAmmunitionPack* pOther)
{
    if (GetItemType() != pOther->GetItemType())
        return;
    m_iAmmunitionSize += pOther->m_iAmmunitionSize;
    m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_Item"), pOther);
}

void CAmmunitionPack::Free()
{
    __super::Free();
}