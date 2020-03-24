#ifndef Mouse_Manager_h__
#define Mouse_Manager_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class CGameObject;
class ENGINE_DLL CMouse_Manager : public CBase
{
	DECLARE_SINGLETON(CMouse_Manager)
public:
	enum eMousePointer { POINTER_AIM, POINTER_BUILD, POINTER_REPAIR, POINTER_ARROW, POINTER_END };

private:
	explicit CMouse_Manager(void);
	virtual ~CMouse_Manager(void);

public:
	_vec3			GetMousePos(void); // 마우스 위치
	POINT			Get_MousePoint(void);
	_vec3			GetMouseRelativeGap(void);
	CGameObject*	Get_MousePointer(void) { return m_pMousePointer; }
public:
	void Initialize(HWND hWnd, int iWinCX, int iWinCY);
	void Set_MousePointer(CGameObject* pMousePointer); 
	void Reset(void);

private:
	HWND		m_hWnd;
	_vec3 m_vecInitialPos;
private:
	CGameObject*			m_pMousePointer = nullptr;

private:
	virtual void Free(void);
};

END

#endif // Mouse_Manager_h__