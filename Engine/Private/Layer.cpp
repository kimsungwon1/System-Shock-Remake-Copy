#include "..\Public\Layer.h"

#include "GameObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject * pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.emplace_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::Delete_GameObject_ByPointer(const CGameObject* pGameObject)
{
	auto iter = find(begin(m_GameObjects), end(m_GameObjects), pGameObject);
	if (iter == end(m_GameObjects)) {
		return E_FAIL;
	}
	else {
		Safe_Release(*iter);
		m_GameObjects.erase(iter);
	}

	return S_OK;
}

CGameObject* CLayer::Get_CloneObject_ByIndex(_int iIndex)
{
	if (m_GameObjects.empty()) {
		return nullptr;
	}
	if (iIndex == -1) {
		return m_GameObjects.back();
	}
	int i = 0;
	for (auto& iter : m_GameObjects) {
		if (i == iIndex) {
			return iter;
		}
		iter++;
		i++;
	}
	return nullptr;
}

HRESULT CLayer::Priority_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject->Priority_Update(fTimeDelta);

	return S_OK;
}

HRESULT CLayer::Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject->Update(fTimeDelta);

	return S_OK;
}

HRESULT CLayer::Late_Update(_float fTimeDelta)
{
	for (auto& pGameObject : m_GameObjects)
		pGameObject->Late_Update(fTimeDelta);

	return S_OK;
}

CComponent * CLayer::Find_Component(const _wstring & strComponentTag, _uint iIndex)
{
	if (iIndex >= m_GameObjects.size())
		return nullptr;

	auto	iter = m_GameObjects.begin();

	for (size_t i = 0; i < iIndex; i++)
		++iter;

	return (*iter)->Find_Component(strComponentTag);	
}

CLayer * CLayer::Create()
{
	return new CLayer();
}

void CLayer::Free()
{
	__super::Free();

	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject);
	m_GameObjects.clear();
}

