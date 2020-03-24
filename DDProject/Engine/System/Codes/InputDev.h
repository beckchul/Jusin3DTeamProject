#ifndef Input_h__
#define Input_h__

#include "Engine_Defines.h"

BEGIN(Engine)
class ENGINE_DLL CInputDev : public CBase
{
	DECLARE_SINGLETON(CInputDev)
public:
	enum MOUSECLICK { DIM_LBUTTON, DIM_RBUTTON, DIM_MBUTTON, };
	enum MOUSEMOVE	{ DIM_X, DIM_Y, DIM_Z, };
private:
	explicit CInputDev(void);
	virtual ~CInputDev(void);	

public: // Getter
	LPDIRECTINPUT8 Get_InputDev() { return m_pInput; }

public: // Setter
	void Set_InputState(void);
	void Set_KeyEnabled(void) { m_bKeyDisabled = false; } // 키 가능하게
	void Set_KeyDisEnabled(void) { m_bKeyDisabled = true; } // 키 불능

public:
	HRESULT Ready_InputDevice(HINSTANCE hInst, HWND hWnd);

public:
	BYTE GetDIKeyState(BYTE KeyFlag);
	BYTE GetDIMouseState(MOUSECLICK KeyFlag);
	long GetDIMouseMove(MOUSEMOVE KeyFlag);

	bool OnceKeyDown(BYTE KeyFlag, bool KeyEnable = false); // KeyEnable을 true로 하면 어떤 상황에서도 키가 눌림
	bool StayKeyDown(BYTE KeyFlag, bool KeyEnable = false);
	bool OnceMouseClick(MOUSECLICK KeyFlag);
	bool StayMouseClick(MOUSECLICK KeyFlag);

private:
	HRESULT Ready_KeyBoard(HWND hWnd);
	HRESULT Ready_Mouse(HWND hWnd);

private:
	LPDIRECTINPUT8			m_pInput = nullptr;
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	BYTE			m_byKeyState[256];
	DIMOUSESTATE	m_MouseState;

	bool			m_bKeyDown[256];
	bool			m_bMouseDown[3];

	bool			m_bKeyDisabled = false; // 키 불능상태
private:
	virtual void Free(void) override;
};

END

#endif
