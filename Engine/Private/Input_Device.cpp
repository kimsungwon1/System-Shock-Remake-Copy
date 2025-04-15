#include "..\Public\Input_Device.h"

Engine::CInput_Device::CInput_Device(void)
{

}

_byte CInput_Device::Get_DIKeyStateDown(_ubyte byKeyID)
{
	_byte ks = (Get_DIKeyState(byKeyID));
	if (!m_byKeyStatePast[byKeyID] && ks) {
		m_byKeyStatePast[byKeyID] = 1;
		return true;
	}
	else if (m_byKeyStatePast[byKeyID] && ks) {
		m_byKeyStatePast[byKeyID] = 1;
		return false;
	}
	else {
		m_byKeyStatePast[byKeyID] = 0;
	}
	return false;
}

_byte CInput_Device::Get_DIMouseStateDown(MOUSEKEYSTATE eMouse)
{
	_byte ms = (Get_DIMouseState(eMouse) & 0x80);
	if (!m_tMouseStatePast.rgbButtons[eMouse] && ms) {
		m_tMouseStatePast.rgbButtons[eMouse] = 1;
		return true;
	}
	else if (m_tMouseStatePast.rgbButtons[eMouse] && ms) {
		m_tMouseStatePast.rgbButtons[eMouse] = 1;
		return false;
	}
	else {
		m_tMouseStatePast.rgbButtons[eMouse] = 0;
	}
	return false;
}

HRESULT Engine::CInput_Device::Initialize(HINSTANCE hInst, HWND hWnd)
{
	// DInput �İ�ü�� �����ϴ� �Լ�
	if (FAILED(DirectInput8Create(hInst,
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&m_pInputSDK,
		nullptr)))
		return E_FAIL;

	// Ű���� ��ü ����
	if (FAILED((m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr))))
		return E_FAIL;

	// ������ Ű���� ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ��ġ�� ���� �������� �������ִ� �Լ�, (Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�)
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pKeyBoard->Acquire();


	// ���콺 ��ü ����
	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	// ������ ���콺 ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pMouse->Acquire();


	return S_OK;
}

void Engine::CInput_Device::Update(void)
{
	/* Ű����� ���콺�� ���¸� ���ͼ� �������ش�. */
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);

	for (int i = 0; i < 256; i++) {
		m_RealKeyStateDown[i] = Get_DIKeyStateDown(i);
	}
	for (int i = 0; i < MOUSEKEYSTATE::DIMK_END; i++) {
		m_RealMouseStateDown[i] = Get_DIMouseStateDown((MOUSEKEYSTATE)i);
	}
}

CInput_Device * CInput_Device::Create(HINSTANCE hInst, HWND hWnd)
{
	CInput_Device*		pInstance = new CInput_Device();

	if (FAILED(pInstance->Initialize(hInst, hWnd)))
	{
		MSG_BOX(TEXT("Failed to Created : CInput_Device"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Engine::CInput_Device::Free(void)
{
	Safe_Release(m_pKeyBoard);
	Safe_Release(m_pMouse);
	Safe_Release(m_pInputSDK);
}

