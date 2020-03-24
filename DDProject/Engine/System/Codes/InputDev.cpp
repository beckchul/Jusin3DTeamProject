#include "InputDev.h"

USING(Engine)

IMPLEMENT_SINGLETON(CInputDev)

CInputDev::CInputDev()
	: m_pInput(nullptr)
	, m_pKeyBoard(nullptr)
	, m_pMouse(nullptr)
	, m_bKeyDisabled(false)
{
	ZeroMemory(m_byKeyState, sizeof(BYTE) * 256);
	ZeroMemory(&m_MouseState, sizeof(DIMOUSESTATE));
}


CInputDev::~CInputDev()
{
}


void CInputDev::Set_InputState(void)
{
	HRESULT hr;

	hr = m_pKeyBoard->GetDeviceState(256, m_byKeyState);

	if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		m_pKeyBoard->Acquire();

	hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);

	if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		m_pMouse->Acquire();

	//m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	//m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);
}

HRESULT CInputDev::Ready_InputDevice(HINSTANCE hInst, HWND hWnd)
{
	HRESULT		hr = 0;
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, nullptr)))
		MSG_BOX(L"DirectInput8 Created Failed");

	hr = Ready_KeyBoard(hWnd);

	hr = Ready_Mouse(hWnd);

	return S_OK;
}

BYTE Engine::CInputDev::GetDIKeyState(BYTE KeyFlag)
{
	return m_byKeyState[KeyFlag];
}

BYTE Engine::CInputDev::GetDIMouseState(MOUSECLICK KeyFlag)
{
	return m_MouseState.rgbButtons[KeyFlag];
}

long Engine::CInputDev::GetDIMouseMove(MOUSEMOVE KeyFlag)
{
	return *(((long*)&m_MouseState) + KeyFlag);
}

bool Engine::CInputDev::OnceKeyDown(BYTE KeyFlag, bool KeyEnable)
{
	if (TRUE == m_bKeyDisabled && FALSE == KeyEnable) // 키 불능 상태
		return false;

	if (m_byKeyState[KeyFlag])
	{
		if (m_bKeyDown[KeyFlag] == false)
		{
			m_bKeyDown[KeyFlag] = true;
			return true;
		}
	}
	else
		m_bKeyDown[KeyFlag] = false;

	return false;
}

bool Engine::CInputDev::StayKeyDown(BYTE KeyFlag, bool KeyEnable)
{
	if (TRUE == m_bKeyDisabled && FALSE == KeyEnable) // 키 불능 상태
		return false;

	if (m_byKeyState[KeyFlag])
		return true;

	return false;
}

bool Engine::CInputDev::OnceMouseClick(MOUSECLICK KeyFlag)
{
	if (m_MouseState.rgbButtons[KeyFlag])
	{
		if (m_bMouseDown[KeyFlag] == false)
		{
			m_bMouseDown[KeyFlag] = true;
			return true;
		}
	}
	else
		m_bMouseDown[KeyFlag] = false;

	return false;
}

bool CInputDev::StayMouseClick(MOUSECLICK KeyFlag)
{
	if (m_MouseState.rgbButtons[KeyFlag])
		return true;
	
	return false;
}

HRESULT CInputDev::Ready_KeyBoard(HWND hWnd)
{
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		MSG_BOX(L"KeyBoard Created Failed");

	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyBoard->Acquire();
	return S_OK;
}

HRESULT CInputDev::Ready_Mouse(HWND hWnd)
{	
	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		MSG_BOX(L"Mouse Created Failed");

	m_pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->Acquire();
	return S_OK;
}

void CInputDev::Free(void)
{
	DWORD			dwRefCnt = 0;

	if (dwRefCnt = Engine::Safe_Release(m_pKeyBoard))
		MSG_BOX(L"m_pKeyBoard Release Failed");

	if (dwRefCnt = Engine::Safe_Release(m_pMouse))
		MSG_BOX(L"m_pMouse Release Failed");
}
