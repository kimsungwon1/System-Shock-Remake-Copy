#include "stdafx.h"
#include "Card.h"
#include "Client_Defines.h"
#include "GameInstance.h"

CCard::CCard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CItem { pDevice, pContext }
{
}

CCard::CCard(const CCard& Prototype)
	: CItem { Prototype }
{
}

HRESULT CCard::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCard::Initialize(void* pArg)
{
	auto* pDesc = static_cast<CARD_DESC*>(pArg);

	m_iData = pDesc->iData;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_eItemType = TYPE_KeyCard;

	m_strName = TEXT("KeyCard - ") + to_wstring(m_iData);
	return S_OK;
}

void CCard::Priority_Update(_float fTimeDelta)
{
}

void CCard::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CCard::Late_Update(_float fTimeDelta)
{
	if (m_bOnInven) {
		return;
	}
	__super::Late_Update(fTimeDelta);
}

HRESULT CCard::Render()
{
	return __super::Render();
}

HRESULT CCard::SaveObject(ofstream& saveStream) const
{
	return E_NOTIMPL;
}

HRESULT CCard::LoadObject(ifstream& loadStream)
{
	return E_NOTIMPL;
}

HRESULT CCard::Ready_Components()
{
	CBounding_AABB::BOUNDING_AABB_DESC aabbDesc = {};

	aabbDesc.vExtents = { 0.1f, 0.1f, 0.1f };
	aabbDesc.vCenter = { 0.f, aabbDesc.vExtents.y, 0.f };
	aabbDesc.pOwner = this;
	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &aabbDesc))) {
		return E_FAIL;
	}

	if (FAILED(Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Card"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModel)))) {
		return E_FAIL;
	}

	return S_OK;
}

CCard* CCard::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCard* pInstance = new CCard(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype())) {
		MSG_BOX(TEXT("Failed to Created : CCard"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCard::Clone(void* pArg)
{
	CCard* pInstance = new CCard(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CCard"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCard::Free()
{
	__super::Free();
}
