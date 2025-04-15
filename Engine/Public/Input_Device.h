#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device : public CBase
{
private:
	CInput_Device(void);
	virtual ~CInput_Device(void) = default;
	
public:
	_byte	Get_DIKeyState(_ubyte byKeyID)			{ 
		return m_byKeyState[byKeyID]; }

	_byte   Get_DIKeyStateDown(_ubyte byKeyID);

	_bool	Get_RealDIKeyStateDown(_ubyte byKeyID) {
		return m_RealKeyStateDown[byKeyID];
	}

	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) 	{ 	
		return m_tMouseState.rgbButtons[eMouse]; 	
	}

	_byte	Get_DIMouseStateDown(MOUSEKEYSTATE eMouse);

	_bool	Get_RealDIMouseStateDown(MOUSEKEYSTATE eMouse) {
		return m_RealMouseStateDown[eMouse];
	}

	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)	
	{			
		return *(((_long*)&m_tMouseState) + eMouseState);	
	}
public:
	void Show_Cursor(_bool bShow) { ShowCursor(bShow); m_bShowingCursor = bShow; }
	_bool IsShowing_Cursor() const { return m_bShowingCursor; }
public:
	HRESULT Initialize(HINSTANCE hInst, HWND hWnd);
	void	Update(void);

private:
	LPDIRECTINPUT8			m_pInputSDK = { nullptr };

private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = { nullptr };
	LPDIRECTINPUTDEVICE8	m_pMouse = { nullptr };
	

private:	
	_byte					m_byKeyState[256] = {};			// 키보드에 있는 모든 키값을 저장하기 위한 변수
	_byte					m_byKeyStatePast[256] = { 0,};

	_bool					m_RealKeyStateDown[256] = { false, };
	_bool					m_RealMouseStateDown[MOUSEKEYSTATE::DIMK_END] = { false, };

	_bool					m_bShowingCursor = false;

	DIMOUSESTATE			m_tMouseState = {};
	DIMOUSESTATE			m_tMouseStatePast = { 0, 0, 0, {0,} };

public:
	static CInput_Device* Create(HINSTANCE hInst, HWND hWnd);
	virtual void	Free(void);

};
END

