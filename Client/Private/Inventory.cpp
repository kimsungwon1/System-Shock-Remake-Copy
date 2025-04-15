#include "stdafx.h"
#include "Inventory.h"

#include "VIBuffer_Point_InvenSlot.h"

#include "Item.h"
#include "UI_Item.h"

#include "AmmunitionPack.h"

#include "GameInstance.h"

CInventory::CInventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CUIObject(pDevice, pContext)
{
}

CInventory::CInventory(const CInventory& ref)
	: CUIObject(ref)
{
}

HRESULT CInventory::Initialize(void* pArg)
{
	UI_DESC			Desc{};

	Desc.fX = g_iWinSizeX >> 1;
	Desc.fY = g_iWinSizeY >> 1;
	Desc.fSizeX = 1;
	Desc.fSizeY = 1;

	Desc.fSpeedPerSec = 10.f;
	Desc.fRotationPerSec = XMConvertToRadians(90.0f);
	Desc.pParent = nullptr;

	if (FAILED(__super::Initialize(&Desc))) {
		return E_FAIL;
	}

	if (FAILED(Ready_Components())) {
		return E_FAIL;
	}
	if (FAILED(Ready_PartUI())) {
		return E_FAIL;
	}

	for (_int i = 0; i < 5; i++) {
		for (_int j = 0; j < 12; j++) {
			m_arrInvenFullByItem[i][j] = false;
		}
	}

	return S_OK;
}

void CInventory::Priority_Update(_float fTimeDelta)
{
	/*if (m_bInvenRender) {
		m_pGameInstance->Show_Cursor(true);
	}
	else {
		m_pGameInstance->Show_Cursor(false);
	}*/
	if (m_bInvenRender)
	{
		for (auto* iter : m_vecItemOnUI) {
			iter->Priority_Update(fTimeDelta);
		}
	}

	// 아이템 피킹, 옮기기
	POINT ptMouse{};
	GetCursorPos(&ptMouse);

	ScreenToClient(g_hWnd, &ptMouse);

	//이미 들고 있으면 마우스 따라가기
	if (m_MousePointer.pItem != nullptr) {
		m_MousePointer.pItem->Set_UIPos({ (_float)ptMouse.x, (_float)ptMouse.y });
	}

	if (m_bInvenRender && m_pGameInstance->Get_RealDIMouseStateDown(MOUSEKEYSTATE::DIMK_LBUTTON)) {
		if (m_MousePointer.pItem == nullptr)
		{
			for (auto* pItem : m_vecItemOnUI) {
				if (pItem->IsMouseOn()) {
					m_MousePointer.pItem = pItem;
					m_MousePointer.vBeforePos = pItem->Get_UIPos();

					_int x = pItem->GetItem()->GetPosInInven().first; _int y = pItem->GetItem()->GetPosInInven().second;
					_int oriX = x; _int oriY = y;
					for (y = oriY; y < oriY + m_MousePointer.pItem->GetSlotSize().second; y++) {
						for (x = oriX; x < oriX + m_MousePointer.pItem->GetSlotSize().first; x++) {
							m_arrInvenFullByItem[y][x] = false;
						}
					}
					break;
				}
			}
		}
		//만약 이미 있음 배치.
		else {
			_float xx = _float(ptMouse.x) - m_vStartPos.x; _float yy = _float(ptMouse.y) - m_vStartPos.y;
			// 위쪽에 배치됨
			if ((_int)xx / 50 >= 0 && (_int)xx / 50 < 12 && (_int)yy / 50 >= 0 && (_int)yy / 50 < 5) {
				_int y = (_int)yy / 50; _int x = (_int)xx / 50;
				_int oriY = y; _int oriX = x;
				_bool bCont = false;
				for (y = oriY; y < oriY + m_MousePointer.pItem->GetSlotSize().second; y++) {
					for (x = oriX; x < oriX + m_MousePointer.pItem->GetSlotSize().first; x++) {
						if (y >= 5 || x >= 12) {
							bCont = true;
							break;
						}
						if (m_arrInvenFullByItem[y][x] == true) {
							bCont = true;
							break;
						}
					}
					if (bCont)
						break;
				}
				if(!bCont) {
					m_MousePointer.pItem->Set_PosInInven((_int)xx / 50, (_int)yy / 50);

					_float2 vPos = m_vStartPos;
					vPos.x += 50 * ((_int)xx / 50) + m_MousePointer.pItem->GetSlotSize().first * 25.f - 25.f;
					vPos.y += 50 * ((_int)yy / 50) + m_MousePointer.pItem->GetSlotSize().second * 25.f - 25.f;

					_int x = (_int)xx / 50; _int y = (_int)yy / 50;
					_int oriX = x; _int oriY = y;
					for (y = oriY; y < oriY + m_MousePointer.pItem->GetSlotSize().second; y++) {
						for (x = oriX; x < oriX + m_MousePointer.pItem->GetSlotSize().first; x++) {
							m_arrInvenFullByItem[y][x] = true;
						}
					}

					m_MousePointer.pItem->Set_UIPos(vPos);
					m_MousePointer.pItem = nullptr; m_MousePointer.vBeforePos = {};
					
				}
			}
		}
	}
}

void CInventory::Update(_float fTimeDelta)
{
}

void CInventory::Late_Update(_float fTimeDelta)
{
	//__super::Late_Update(fTimeDelta);
	_int i = 0;
	for (auto* pUI : m_vecPartUIs) {
		if (i >= 10 && !m_bInvenRender) {
			break;
		}
		if (pUI == nullptr) {
			continue;
		}
		pUI->Late_Update(fTimeDelta);
		i++;
	}

	if(m_bInvenRender)
	{
		for (auto* iter : m_vecItemOnUI) {
			iter->Late_Update(fTimeDelta);
		}
	}
}

HRESULT CInventory::Render()
{
	/*if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Bind_ShadeResource(m_pShaderCom, "g_Texture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferPointCom->Bind_Buffers()))
		return E_FAIL;
	if (FAILED(m_pVIBufferPointCom->Render()))
		return E_FAIL;*/

	return S_OK;
}

void CInventory::PushBullet(_int eType)
{
	
}

void CInventory::PopBullet(_int eType)
{
	for (auto iter = m_vecBulletPacks.begin(); iter != m_vecBulletPacks.end();) {
		if ((*iter)->GetItemType() == eType) {
			(*iter)->PopAmmu();

			if ((*iter)->Get_AmmunitionSize() <= 0) {
				auto* pPack = *iter;
				// vector m_vecBulletPacks 안에 제거해야함
				m_vecBulletPacks.erase(iter);
				// 유아이 객체 제거해야함
				for (auto uiIter = m_vecItemOnUI.begin(); uiIter != m_vecItemOnUI.end();) {
					if ((*uiIter)->IsSameItem(pPack)) {
						m_vecItemOnUI.erase(uiIter);
						break;
					}
					else {
						uiIter++;
					}
				}
				// pPack 자체 제거해야함
				m_pGameInstance->Reserve_ToDelete(LEVEL_GAMEPLAY, TEXT("Layer_Item"), pPack);
			}
			
			return;
		}
		else {
			iter++;
		}
	}
}

_int CInventory::GetBulletNum(_int eType)
{
	for (auto iter = m_vecBulletPacks.begin(); iter != m_vecBulletPacks.end();) {
		if ((*iter)->GetItemType() == eType) {
			return (*iter)->Get_AmmunitionSize();
		}
		else {
			iter++;
		}
	}
	return 0;
}

void CInventory::Set_InvenRender(_bool bInven)
{
	m_bInvenRender = bInven;
	if (bInven) {
		m_pGameInstance->Show_Cursor(true);
	}
	else {
		m_pGameInstance->Show_Cursor(false);
	}
}

_bool CInventory::PushItem(CItem* pItem)
{
	_bool bPutThisToVecAmmu = false;
	if (pItem->GetItemType() == CItem::TYPE_PistolBullets || pItem->GetItemType() == CItem::TYPE_RifleBullets || pItem->GetItemType() == CItem::TYPE_Rockets) {
		for (auto* iter : m_vecBulletPacks) {
			if (iter->GetItemType() == pItem->GetItemType()) {
				iter->JoinTogether(dynamic_cast<CAmmunitionPack*>(pItem));
				return false;
			}
		}
		bPutThisToVecAmmu = true;
	}

	for (_int i = 0; i < 5; i++) {
		for (_int j = 0; j < 12; j++) {
			_bool bCont = false;
			if (m_arrInvenFullByItem[i][j] == true) {
				continue;
			}
			else {
				_int y = i; _int x = j;
				for (y = i; y < i + pItem->GetSlotSize().second; y++) {
					for (x = j; x < j + pItem->GetSlotSize().first; x++) {
						if (y >= 5 || x >= 12) {
							bCont = true;
							break;
						}
						if (m_arrInvenFullByItem[y][x] == true) {
							bCont = true;
							break;
						}
					}
					if (bCont)
						break;
				}
				if (bCont)
					continue;
				else {
					pItem->Set_PosInInven(j, i);
					
					CUI_Item::ITEMSLOT desc = {};
					desc.iTextureIndex = pItem->GetItemType();
					desc.fSizeX = 50.f * pItem->GetSlotSize().first; desc.fSizeY = 50.f * pItem->GetSlotSize().second;
					desc.fX = m_arrUIPos[i][j].x + desc.fSizeX * 0.5f - 25.f; desc.fY = m_arrUIPos[i][j].y + desc.fSizeY * 0.5f - 25.f; desc.pItem = pItem;
					//m_pGameInstance->Add_CloneObject_ToLayer(LEVEL_GAMEPLAY, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Item"), &desc);
					CUI_Item* uiItem = dynamic_cast<CUI_Item*>(m_pGameInstance->Clone_Object(TEXT("Prototype_GameObject_UI_Item"), &desc));
					m_vecItemOnUI.push_back(uiItem);
					
					_int y = i; _int x = j;
					
					for (y = i; y < i + pItem->GetSlotSize().second; y++) {
						for (x = j; x < j + pItem->GetSlotSize().first; x++) {
							m_arrInvenFullByItem[y][x] = true;
						}
					}
					if (bPutThisToVecAmmu) {
						m_vecBulletPacks.push_back(dynamic_cast<CAmmunitionPack*>(pItem));
					}
					return true;
				}
			}
		}
	}

	return false;
}

HRESULT CInventory::Ready_Components()
{
	/*CVIBuffer_Point_InvenSlot::INVENSLOTINSTANCE_DESC slotDesc = {};
	slotDesc.vScale = { 50.f,50.f };

	_float2 vStartPos = { (_float)g_iWinSizeX * 0.5f - 12.f * 50.f * 0.5f,
		(_float)g_iWinSizeY * 0.5f - 5.f * 50.f * 0.5f };

	for (_int i = 0; i < 5; i++) {
		for (_int j = 0; j < 12; j++) {
			slotDesc.arrSlotPoses[i][j] = { vStartPos.x + 50.f * j, vStartPos.y + 50.f * i };
		}
	}

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_InvenSlot"),
		TEXT("Com_VIBuffer_InvenSlot"), reinterpret_cast<CComponent**>(&m_pVIBufferPointCom), &slotDesc)))
		return E_FAIL;*/

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_InvenSlot"),
		TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
		return E_FAIL;

	return S_OK;
}

HRESULT CInventory::Ready_PartUI()
{
	m_vecPartUIs.resize(60 + 10);
	UI_DESC desc = {};
	
	m_vWeaponStartPos = { (_float)g_iWinSizeX * 0.5f - 10.f * 50.f * 0.5f,
		(_float)g_iWinSizeY - 60.f };
	desc.fSizeX = desc.fSizeY = 50.f; desc.fY = m_vWeaponStartPos.y;
	for (_int i = 0; i < 10; i++) {
		desc.fX = { m_vWeaponStartPos.x + 50.f * i };
		
		m_arrWeaponUIPos[i] = { desc.fX, desc.fY };

		if (FAILED(Add_PartUI(i, TEXT("Prototype_GameObject_InvenSlot"), &desc)))
			return E_FAIL;
	}

	m_vStartPos = { (_float)g_iWinSizeX * 0.5f - 12.f * 50.f * 0.5f,
		(_float)g_iWinSizeY * 0.5f - 5.f * 50.f * 0.5f + 120};

	for (_int i = 0; i < 5; i++) {
		for (_int j = 0; j < 12; j++) {
			_int iIndex = i * 12 + j + 10;
			desc.fX = { m_vStartPos.x + 50.f * j };
			desc.fY = m_vStartPos.y + 50.f * i;
			
			m_arrUIPos[i][j] = { desc.fX, desc.fY };
			
			if(FAILED(Add_PartUI(iIndex, TEXT("Prototype_GameObject_InvenSlot"), &desc)))
				return E_FAIL;
		}
	}

	return S_OK;
}

CInventory* CInventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CInventory* pInstance = new CInventory(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(TEXT("Failed to Created : CInventory"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CInventory::Clone(void* pArg)
{
	CInventory* pInstance = new CInventory(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CInventory"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInventory::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	for (auto* iter : m_vecItemOnUI) {
		Safe_Release(iter);
	}
}
