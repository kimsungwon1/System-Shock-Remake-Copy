#pragma once
#include "Base.h"

// 1. 맨 밑의 자식 옥트리가 객체를 가짐.
//		맨 밑에놈은 자식이 nullptr.
// 2. 만들어질때 처음엔 조그만 옥트리가 나옴
//		객체 추가될수록 옥트리가 추가됨

BEGIN(Engine)
class CGameObject;
//template<typename T>
class ENGINE_DLL COctree : public CBase
{
	enum OCT_TYPE{ UP_NW, UP_NE, UP_SE, UP_SW, DOWN_NW, DOWN_NE, DOWN_SE, DOWN_SW, OCT_END };
private:
	COctree();
	~COctree() = default;

public:
	static _vector Get_OCT_Dir(OCT_TYPE eDir);

public:
	// 조상 옥트리 만들기 위함. 길이가 안 정해졌으니까 여기서 정하기!
	HRESULT Initialize(const list<CGameObject*>& listVariable, _float fMinRectLength);
	// 자식 옥트리 만들기 위함
	HRESULT Initialize(const list<CGameObject*>& listVariable, _float fMinRectLength, _float fRectLength, _cvector vCenter);

	//HRESULT Initialize(_float iPerRectLen, _float iMinRectlength, _fvector vCenter, COctree* pParent = nullptr);

	//void Push_Back(const CGameObject* pVar, _fvector vVarCenter);
	void Delete_Variable(CGameObject* pVar, _fvector vPosition);

	list<CGameObject*> GetList_ByRadius(_fvector vCenter, _float fRadius);
	
	void Update_Variable(const CGameObject* var);

private:
	//list<const CGameObject*> getList();
private:
	_float m_fRectLength;
	_float m_fMinRectLength;
	COctree* m_pChilds[OCT_TYPE::OCT_END] = { nullptr, };
	const COctree* m_pParent = { nullptr };
	
	list<CGameObject*> m_listVar = {};

	_float3 m_vCenter = {};

	//OCT_TYPE get_InOrOut(_fvector vCenter) const;
public:
	static COctree* Create(const list<CGameObject*>& listVariable, _float fMinRectLength) {
		COctree* pInstance = new COctree();
		pInstance->Initialize(listVariable, fMinRectLength);
		return pInstance;
	}

	static COctree* Create(const list<CGameObject*>& listVariable, _float fMinRectLength, _float fRectLength, _cvector vCenter) {
		COctree* pInstance = new COctree();
		pInstance->Initialize(listVariable, fMinRectLength, fRectLength, vCenter);
		return pInstance;
	}

	/*static COctree* Create(_float iPerRectLen, _float iMinRectlength, _vector vCenter, COctree* pParent = nullptr) {
		COctree* pInstance = new COctree();
		pInstance->Initialize(iPerRectLen, iMinRectlength, vCenter, pParent);
		return pInstance;
	}*/
	virtual void Free() override;
};


END