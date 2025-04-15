#include "..\Public\Object_Manager.h"

#include "Layer.h"
#include "GameObject.h"
#include "Octree.h"

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];
	m_pOctrees = new map<const _wstring, COctree*>[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _wstring & strPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(strPrototypeTag))
	{
		Safe_Release(pPrototype);
		return E_FAIL;
	}

	m_Prototypes.emplace(strPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strPrototypeTag, void * pArg)
{
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	/* 복제해야할 원형을 차즌ㄴ다. */
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	/* 그 원형을 복제하여 사본 객체를 생성한다. */
	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	/* 객체들은 레잉어로 묶어서 관리하고 있었거든 */
	/* 사본을 추가하기위한 레이어를 찾자.*/
	CLayer*		pLayer = Find_Layer(iLevelIndex, strLayerTag);

	/* 야 그 레이어가 없는디? */
	/* 내가 추가하려고했던 레이어가 아직없었다 == 처음 추가하는 객체였다. */
	/* 새롱누 렝이ㅓ를 만들어야지. */
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLevelIndex].emplace(strLayerTag, pLayer);
	}
	else /* 내가 추가하려고 하는 레잉어가 만들어져있었어. */
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

HRESULT CObject_Manager::Delete_CloneObject_ByPointer(_uint iLevelIndex, const _wstring& strLayerTag, const CGameObject* pObject)
{
	if (pObject == nullptr) {
		return E_FAIL;
	}
	if (iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	auto iter = m_pOctrees[iLevelIndex].find(strLayerTag);
	if (iter != m_pOctrees[iLevelIndex].end()) {
		iter->second->Delete_Variable(const_cast<CGameObject*>(pObject), pObject->GetTransformCom()->Get_State(CTransform::STATE_POSITION));
	}

	if (nullptr == pLayer)
	{
		return E_FAIL;
	}
	else {
		return pLayer->Delete_GameObject_ByPointer(pObject);
	}
}

CGameObject* CObject_Manager::Get_CloneObject_ByIndex(_uint iLevelIndex, const _wstring& strLayerTag, _int iIndex)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);
	
	if (nullptr == pLayer)
	{
		return nullptr;
	}
	else {
		return pLayer->Get_CloneObject_ByIndex(iIndex);
	}
}

HRESULT CObject_Manager::Priority_Update(_float fTimeDelta)
{
	// 초기화
	if (m_vTimeAlterLast.y > 0.f) {
		if (m_vTimeAlterLast.x >= m_vTimeAlterLast.y) {
			m_vTimeAlterLast = { 0.f, 0.f };
			m_fTimeAlterRatio = 1.f; m_fTimeAlterRatio_ToException = 1.f;
			m_strExceptionLayerTag = {};
		}
		else {
			m_vTimeAlterLast.x += fTimeDelta;
		}
	}

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		/* LEVEL_STATIC용 레이어들과 현재 할당된 레벨용 레이어들만 유효하게 담겨있는 상황이 될꺼다. */
		for (auto& Pair : m_pLayers[i])
		{
			_float fRatio = 1.f;
			// 예외가 되는 레이어라면
			if(Pair.first.compare(m_strExceptionLayerTag) == 0)
			{
				fRatio = m_fTimeAlterRatio_ToException;
			}
			else {
				fRatio = m_fTimeAlterRatio;
			}

			Pair.second->Priority_Update(fTimeDelta * fRatio);
		}	
	}
	return S_OK;
}

HRESULT CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		/* LEVEL_STATIC용 레이어들과 현재 할당된 레벨용 레이어들만 유효하게 담겨있는 상황이 될꺼다. */
		for (auto& Pair : m_pLayers[i])
		{
			_float fRatio = 1.f;
			// 예외가 되는 레이어라면
			if (Pair.first.compare(m_strExceptionLayerTag) == 0)
			{
				fRatio = m_fTimeAlterRatio_ToException;
			}
			else {
				fRatio = m_fTimeAlterRatio;
			}

			Pair.second->Update(fTimeDelta * fRatio);
		}
	}

	return S_OK;
}

HRESULT CObject_Manager::Late_Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		/* LEVEL_STATIC용 레이어들과 현재 할당된 레벨용 레이어들만 유효하게 담겨있는 상황이 될꺼다. */
		for (auto& Pair : m_pLayers[i])
		{
			_float fRatio = 1.f;
			// 예외가 되는 레이어라면
			if (Pair.first.compare(m_strExceptionLayerTag) == 0)
			{
				fRatio = m_fTimeAlterRatio_ToException;
			}
			else {
				fRatio = m_fTimeAlterRatio;
			}

			Pair.second->Late_Update(fTimeDelta * fRatio);
		}
	}

	// 예약된 삭제 오브젝트들을 삭제한다.
	for (auto& iter = begin(m_ReservedToDeleteObjects); iter != end(m_ReservedToDeleteObjects); iter++)
	{
		Delete_CloneObject_ByPointer(iter->iLevelIndex, iter->szLayerTag, iter->pObj);
	}
	m_ReservedToDeleteObjects.clear();

	return S_OK;
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();
}

CComponent * CObject_Manager::Find_Component(_uint iLevelIndex, const _wstring & strLayerTag, const _wstring & strComponentTag, _uint iIndex)
{
	CLayer*	pLayer = Find_Layer(iLevelIndex, strLayerTag);
	if (nullptr == pLayer)
		return nullptr;

	return pLayer->Find_Component(strComponentTag, iIndex);	
}

CGameObject* CObject_Manager::Clone_Object(const _wstring& strPrototypeTag, void* pArg)
{
	CGameObject* pPrototype = Find_Prototype(strPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject) 
		return nullptr;	

	return pGameObject;
}

list<class CGameObject*>* CObject_Manager::GetRefGameObjects(_uint iLevelIndex, const _wstring& strLayerTag)
{
	if (iLevelIndex >= m_iNumLevels)
		return nullptr;

	CLayer* pLayer = Find_Layer(iLevelIndex, strLayerTag);

	if (nullptr == pLayer)
	{
		return nullptr;
	}
	return &pLayer->GetRefGameObjects();
}

HRESULT CObject_Manager::Reserve_ToDelete(_uint iLevelIndex, const _wstring& strLayerTag, const CGameObject* pObject)
{
	m_ReservedToDeleteObjects.push_back(ReservedToDelete{iLevelIndex, strLayerTag, pObject});

	return S_OK;
}

HRESULT CObject_Manager::Add_Octree(_uint iLevelIndex, const _wstring& strLayerTag, _float iMinRectlength)
{
	auto foundOct = m_pOctrees[iLevelIndex].find(strLayerTag);
	if (foundOct != m_pOctrees[iLevelIndex].end())
		return E_FAIL;
	auto foundLayer = m_pLayers[iLevelIndex].find(strLayerTag);
	if (foundLayer == m_pLayers[iLevelIndex].end())
		return E_FAIL;

	auto* layerRefs = this->GetRefGameObjects(iLevelIndex, strLayerTag);

	m_pOctrees[iLevelIndex].emplace(strLayerTag, COctree::Create(*layerRefs, iMinRectlength));

	return S_OK;
}

HRESULT CObject_Manager::Delete_Octree(_uint iLevelIndex, const _wstring& strLayerTag)
{
	auto foundOct = m_pOctrees[iLevelIndex].find(strLayerTag);
	if (foundOct == m_pOctrees[iLevelIndex].end())
		return E_FAIL;
	Safe_Release(foundOct->second);
	m_pOctrees[iLevelIndex].erase(foundOct);

	return S_OK;
}

//void CObject_Manager::Push_Back(_uint iLevelIndex, const _wstring& strLayerTag, const CGameObject* pVar, _fvector vVarCenter)
//{
//	auto iter = m_pOctrees[iLevelIndex].find(strLayerTag);
//	if (iter == m_pOctrees[iLevelIndex].end())
//		return;
//	(*iter).second->Push_Back(pVar, vVarCenter);
//}

list<CGameObject*> CObject_Manager::GetList_ByRadius(_uint iLevelIndex, const _wstring& strLayerTag, _fvector vCenter, _float fRadius)
{
	auto iter = m_pOctrees[iLevelIndex].find(strLayerTag);
	if (iter == m_pOctrees[iLevelIndex].end())
		return list<CGameObject*>();
	return (*iter).second->GetList_ByRadius(vCenter, fRadius);
}

CGameObject * CObject_Manager::Find_Prototype(const _wstring & strPrototypeTag)
{
	auto	iter = m_Prototypes.find(strPrototypeTag);
	if(iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const _wstring & strLayerTag)
{
	auto	iter = m_pLayers[iLevelIndex].find(strLayerTag);
	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

CObject_Manager * CObject_Manager::Create(_uint iNumLevels)
{
	CObject_Manager*		pInstance = new CObject_Manager();

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX(TEXT("Failed to Created : CObject_Manager"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pLayers[i])		
			Safe_Release(Pair.second);
		m_pLayers[i].clear();		
	}
	Safe_Delete_Array(m_pLayers);
	
	for (size_t i = 0; i < m_iNumLevels; i++)
	{
		for (auto& Pair : m_pOctrees[i])
			Safe_Release(Pair.second);
		m_pOctrees[i].clear();
	}
	
	Safe_Delete_Array(m_pOctrees);

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);
	m_Prototypes.clear();
}
