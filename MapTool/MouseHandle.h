#pragma once
#include "Base.h"

class CToolObject;
class CImguiDesign;

BEGIN(Engine)
	class CGameInstance;
	class CGameObject;
END

class CMouseHandle : public CBase
{
	friend class CImguiDesign;
private:
	CMouseHandle(CImguiDesign* pGui);
	~CMouseHandle() = default;

public:
	HRESULT PushObject(CToolObject* pObj);	// 핸들 안에 새 오브젝트 추가. 원래 있는건 삭제
	HRESULT PopObject();					// 핸들 안에 오브젝트 삭제
	HRESULT CloneObject();					// 핸들 안에 오브젝트와 같은 클론 오브젝트 만들기.
	void Update(_float fTimeDelta);
private:
	CGameInstance* m_pGameInstance = nullptr;
	CToolObject* m_pObjectOnMouse = nullptr;
	CImguiDesign* m_pGui = nullptr;
	class CMyNavigation* m_pNavigation = nullptr;

	_float3 m_posOffset = {0.f,0.f,0.f};
	_bool m_bCross = false;
	_bool m_bPickToTile = false;

	POINT originalCursorPoint = { 0, 0 };

	class CMyCell_Full* m_pPickedCell = nullptr;

public:
	_vector GetPosOffset() const {
		return XMLoadFloat3(&m_posOffset);
	}
	void SetPosOffset(_vector vOffset) {
		XMStoreFloat3(&m_posOffset, vOffset);
	}

	void SetCross(_bool bCross) { m_bCross = bCross; }
	_bool GetCross() const { return m_bCross; }

	// 반드시 nullptr인 포인터를 pRetObject로 넘겨야 한다!!!!!!
	_bool Picking_Object(CGameObject*& pRetObject, _float3* pos, _wstring layerName, _float& fMinDistance);

	_vector GetCrossPosition(_vector vPosition);

	HRESULT SaveNavi(ofstream& ofs);
	HRESULT LoadNavi(ifstream& ifs);

	HRESULT ClearNavi();
private:
	void Update_HandleNotPlacedObject();
	void Update_HandlePlacedObject();
	void Edit_Navigation();
	void Brush();
public:
	const CToolObject* GetObjectOnMouse() const { return m_pObjectOnMouse; }
	CToolObject* GetObjectOnMouse() { return m_pObjectOnMouse; }
public:
	static CMouseHandle* Create(CImguiDesign* pGui);
	void Free() override;
};

