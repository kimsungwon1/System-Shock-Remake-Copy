#pragma once


#include "Base.h"

/* 객체들을 모아서 그룹(Layer)별로 저장한다. */
/* 모아 놓은 객체들의 반복적인 Update를 호출해준다. */
/* 모아 놓은 객체들의 반복적인 Render를 호출해준다.(x) : 객체들을 그리는 순서를 내가 따로 좀 지정을 해야할 필요가 있다. */

BEGIN(Engine)
class CGameObject;

class COctree;


class CObject_Manager final : public CBase
{
private:
	struct ReservedToDelete {
		_uint iLevelIndex;
		_wstring szLayerTag;
		const class CGameObject* pObj = nullptr;
	};

	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	HRESULT Initialize(_uint iNumLevels);
	HRESULT Add_Prototype(const _wstring& strPrototypeTag, class CGameObject* pPrototype);	
	HRESULT Add_CloneObject_ToLayer(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strPrototypeTag, void* pArg);
	HRESULT Delete_CloneObject_ByPointer(_uint iLevelIndex, const _wstring& strLayerTag, const CGameObject* pObject);
	class CGameObject* Get_CloneObject_ByIndex(_uint iLevelIndex, const _wstring& strLayerTag, _int iIndex = -1);

	HRESULT Priority_Update(_float fTimeDelta);
	HRESULT Update(_float fTimeDelta);
	HRESULT Late_Update(_float fTimeDelta);
	void Clear(_uint iLevelIndex);

public:
	class CComponent* Find_Component(_uint iLevelIndex, const _wstring& strLayerTag, const _wstring& strComponentTag, _uint iIndex);
	class CGameObject* Clone_Object(const _wstring& strPrototypeTag, void* pArg);
public:
	list<class CGameObject*>* GetRefGameObjects(_uint iLevelIndex, const _wstring& strLayerTag);
public:
	HRESULT Reserve_ToDelete(_uint iLevelIndex, const _wstring& strLayerTag, const CGameObject* pObject);
public:
	void Reserve_Timealter(_float fTimeAlterRatio, _float fTimeAlterRatio_ToException, _float fTimeAlterLast, const _wstring& strExceptionLayer) {
		m_fTimeAlterRatio = fTimeAlterRatio;
		m_fTimeAlterRatio_ToException = fTimeAlterRatio_ToException;
		m_vTimeAlterLast.y = fTimeAlterLast;
		m_strExceptionLayerTag = strExceptionLayer;
	};
	_bool IsTimeSlowing(){ return m_vTimeAlterLast.y > 0; }

public: // About Octtree
	HRESULT Add_Octree(_uint iLevelIndex, const _wstring& strLayerTag, _float iMinRectlength);
	HRESULT Delete_Octree(_uint iLevelIndex, const _wstring& strLayerTag);
	list<CGameObject*> GetList_ByRadius(_uint iLevelIndex, const _wstring& strLayerTag, _fvector vCenter, _float fRadius);

	//void Push_Back(_uint iLevelIndex, const _wstring& strLayerTag, const CGameObject* pVar, _fvector vVarCenter);
	
private:

	/* 원형객체들을 보관한다. */
	map<const _wstring, class CGameObject*>		m_Prototypes;

	/* 레벨별로, 사본객체들을 그룹별로 모아서 보관한다. */
	_uint										m_iNumLevels = {};
	map<const _wstring, class CLayer*>*			m_pLayers = { nullptr };
	typedef map<const _wstring, class CLayer*>		LAYERS;

	map<const _wstring, COctree*>* m_pOctrees = {  };

	_float m_fTimeAlterRatio = 1.f;
	_float m_fTimeAlterRatio_ToException = 1.f;
	_float2 m_vTimeAlterLast = { 0.f, 0.f };
	_wstring m_strExceptionLayerTag = {};

private:
	class CGameObject* Find_Prototype(const _wstring& strPrototypeTag);
	class CLayer* Find_Layer(_uint iLevelIndex, const _wstring& strLayerTag);

	vector<ReservedToDelete> m_ReservedToDeleteObjects;
public:
	static CObject_Manager* Create(_uint iNumLevels);
	virtual void Free() override;
};

END