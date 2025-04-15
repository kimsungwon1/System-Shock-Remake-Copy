#pragma once
#include "Base.h"

BEGIN(Engine)

class CGameInstance;

class CRaycaster final : public CBase
{
private:
	CRaycaster();
	virtual ~CRaycaster() = default;

public:
	// 레이캐스팅 한 시작 위치와 방향 벡터를 리턴
	HRESULT Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	void Update();
	void Transform_ToLocalSpace(const _float4x4& WorldMatrix);
	_bool isPicked_InLocalSpace(_vector vPointA, _vector vPointB, _vector vPointC, _float3* pOut);
	_bool isPicked_InWorldSpace(_vector vPointA, _vector vPointB, _vector vPointC, _float3* pOut);
private:
	CGameInstance* m_pGameInstance = nullptr;

	HWND						m_hWnd = {};
	_uint						m_iWinSizeX = {};
	_uint						m_iWinSizeY = {};

	_float3 m_vRayPos;
	_float3 m_vRayDir;
	_float3 m_vRayPos_Local;
	_float3 m_vRayDir_Local;
	_bool m_bPickingToCenter = false;

public:
	void Set_RayCaster_Center() { m_bPickingToCenter = true; }
	void Set_RayCaster_Mouse() { m_bPickingToCenter = false; }

public:
	static CRaycaster* Create(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY);
	virtual void Free() override;
};

END
