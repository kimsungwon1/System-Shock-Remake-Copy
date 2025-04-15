#include "stdafx.h"
#include "UI_Item.h"

#include "Client_Defines.h"

#include "Item.h"

#include "GameInstance.h"

CUI_Item::CUI_Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CUI_Item::CUI_Item(const CUI_Item& Prototype)
    : CUIObject{ Prototype }
{
}

HRESULT CUI_Item::Initialize_Prototype()
{
    for (_int i = 0; i < 15; i++) {
        m_pTextureComArr[i] = nullptr;
    }
    return S_OK;
}

HRESULT CUI_Item::Initialize(void* pArg)
{
    ITEMSLOT* pDesc = static_cast<ITEMSLOT*>(pArg);
    m_iCurTextureIndex = pDesc->iTextureIndex;
    m_pOwnerItem = pDesc->pItem;

    if (FAILED(__super::Initialize(pArg))) {
        return E_FAIL;
    }

    if (FAILED(Ready_Components())) {
        return E_FAIL;
    }
    
    //m_eType = TYPE_HEALTHBAR;

    return S_OK;
}

void CUI_Item::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);

    POINT p = { };

    GetCursorPos(&p);
    ScreenToClient(g_hWnd, &p);
    RECT rect = { };
    SetRect(&rect, m_fX - m_fSizeX * 0.5f, m_fY - m_fSizeY * 0.5f, m_fX + m_fSizeX * 0.5f, m_fY + m_fSizeY * 0.5f);
    if (PtInRect(&rect, p)) {
        m_bMouseOn = true;
    }
    else {
        m_bMouseOn = false;
    }
}

void CUI_Item::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CUI_Item::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_Item::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;
    if (FAILED(m_pTextureComArr[m_iCurTextureIndex]->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Begin(0)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Buffers()))
        return E_FAIL;
    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

_bool CUI_Item::IsSameItem(CItem* pItem)
{
    if (pItem == m_pOwnerItem) {
        return true;
    }
    return false;
}

pair<int, int> CUI_Item::GetSlotSize() const
{
    return m_pOwnerItem->GetSlotSize();
}

void CUI_Item::Set_PosInInven(_int x, _int y)
{
    m_pOwnerItem->Set_PosInInven(x, y);
}

HRESULT CUI_Item::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    for (_int i = 0; i < 15; i++) {
        m_pTextureComArr[i] = nullptr;
    }
    /* FOR.Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MiniPistol_Slot"),
        TEXT("Com_Texture_0"), reinterpret_cast<CComponent**>(&m_pTextureComArr[CItem::ItemType::TYPE_Pistol]))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_AssaultRifle_Slot"),
        TEXT("Com_Texture_1"), reinterpret_cast<CComponent**>(&m_pTextureComArr[CItem::ItemType::TYPE_AssaultRifle]))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RocketLauncher_Slot"),
        TEXT("Com_Texture_2"), reinterpret_cast<CComponent**>(&m_pTextureComArr[CItem::ItemType::TYPE_RocketLauncher]))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Grenade_Slot"),
        TEXT("Com_Texture_4"), reinterpret_cast<CComponent**>(&m_pTextureComArr[CItem::ItemType::TYPE_Grenade]))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PistolBullets_Slot"),
        TEXT("Com_Texture_5"), reinterpret_cast<CComponent**>(&m_pTextureComArr[CItem::ItemType::TYPE_PistolBullets]))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RifleBullets_Slot"),
        TEXT("Com_Texture_6"), reinterpret_cast<CComponent**>(&m_pTextureComArr[CItem::ItemType::TYPE_RifleBullets]))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Rockets_Slot"),
        TEXT("Com_Texture_7"), reinterpret_cast<CComponent**>(&m_pTextureComArr[CItem::ItemType::TYPE_Rockets]))))
        return E_FAIL;

    /* FOR.Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CUI_Item* CUI_Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Item* pInstance = new CUI_Item(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CUI_Item"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUI_Item::Clone(void* pArg)
{
    CUI_Item* pInstance = new CUI_Item(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CUI_Item"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUI_Item::Free()
{
    __super::Free();

    for (_int i = 0; i < 15; i++) {
        Safe_Release(m_pTextureComArr[i]);
    }
}