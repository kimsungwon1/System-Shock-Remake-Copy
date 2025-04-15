#pragma once

#include <utility>
#include <vector>
#include <array>
#include <string>
#include <limits>
#include "Base.h"
#include "Client_Defines.h"

using namespace std;

#undef END

struct Object {
	_wstring szModelName;
};

struct ObjectSection {
	AllObjectType type;
	vector<Object*> objects;

};

class CImguiDesign
{
public:
	CImguiDesign(ID3D11Device* m_pDevice, ID3D11DeviceContext* m_pContext);
	~CImguiDesign();

public:
	void Present();
	_float GetDistance() const { return m_fDistance; }
public:
	void InitForGameObject();

private:
	void Present_Transformbuttons();
	void Present_Transform90Degree();
	void Present_PositionTranslate();
	void Present_Animations();
	void Present_EditNavigation();
	void Present_PickedPixel();
public:
	_bool IsLMouseOnGui() const { return m_bMouseOnGui; }
public:
	void SetMainApp(class CMainApp* pMainApp) { m_pMainApp = pMainApp; }

	_float GetHandledScale() const { return m_fHandledScale; }

	_bool IsCellEditing() const { return m_bEditNavi; }

	_bool IsCellPicking() const { return m_bCellPicking; }
	_bool IsCellAdding() const { return m_bCellAdding; }

	_int GetCellVertexIndex() const { return m_iCellVertexIndex; }

	void InitializeCreateCell();

	void AddVertextToMakingCell();
	_float3 GetMakingCell(_uint iIndex) const { return m_CellMaking[iIndex]; }
	void SetMakingCell(_uint iIndex, _float3 vec3) { m_CellMaking[iIndex] = vec3; }

	void SetCellPicking(_bool bCellPicks) { m_bCellPicking = bCellPicks; }

	_int GetSelectedSide() const { return m_iSelectedSide; }
	void InitializeSides() { m_iSelectedSide = -1; }
	_bool IsMakeCellFlag() const { return m_bMakeCellFlag; }
private:
	_bool m_bMouseOnGui = false;

	_bool m_bEditNavi = false;

	_bool m_bCellPicking = false;
	_bool m_bCellAdding = false;

	_int m_iCellVertexIndex = 0;
	_float3 m_CellMaking[3] = { {}, {}, {} };

	_int m_iSelectedSide = -1;

	_bool m_bMakeCellFlag = false;
	_float3 m_fAddCellVertex = {};

	// Light
	_float4 m_vDirection = {};
	_float4 m_vPosition = {};
	_float m_fRange = { 5.f };
	_float4 m_vDiffuse = {};
	_float4 m_vAmbient = {};
	_float4 m_vSpecular = {};

	////////

	_int m_iData = {};
public:
	_int GetObjData() const { return m_iData; }
	void SetObjData(_int iData) { m_iData = iData; }
	//Pixel Picking Pos
public:
	_float3 Get_PixelPos() const { return m_vPixelPos; }
private:
	_float3 m_vPixelPos = {};
private:
	void processMouseScroll();

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	pair<int, int> m_Size = { 400, 600 };
	int m_PerBoxSize = { 50 };
	
	array<ObjectSection, (int)AllObjectType::TYPE_END> m_Sections{
		ObjectSection { AllObjectType::TYPE_TILE, {}},
		ObjectSection { AllObjectType::TYPE_DECO, {}},
		ObjectSection { AllObjectType::TYPE_UNIT, {}}
	};

	AllObjectType m_eCurSection = AllObjectType::TYPE_TILE;
public:
	AllObjectType GetSectionType() const { return m_eCurSection; }
private:
	_float m_fDistance = 10.f;
	_float m_fHandledScale = 1.f;

private:
	class CMainApp* m_pMainApp = nullptr;
};

