#include <iostream>
#include "Mouse_Manager.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(Engine::CMouse_Manager)

Engine::CMouse_Manager::CMouse_Manager(void)
{

}

Engine::CMouse_Manager::~CMouse_Manager(void)
{
}

Engine::_vec3 Engine::CMouse_Manager::GetMousePos(void)
{
	static POINT ptMouse;
	// 현재 마우스 커서의 좌표를 얻어오는 함수.
	GetCursorPos(&ptMouse);

	// 현재 전체 창에 대한 마우스의 좌표를 해당 윈도우 창 영역 안의 좌표로 변환해주는 함수.
	ScreenToClient(m_hWnd, &ptMouse);

	return _vec3((float)ptMouse.x, (float)ptMouse.y, 0.f);
}

POINT Engine::CMouse_Manager::Get_MousePoint(void)
{
	static POINT ptMouse;
	// 현재 마우스 커서의 좌표를 얻어오는 함수.
	GetCursorPos(&ptMouse);

	// 현재 전체 창에 대한 마우스의 좌표를 해당 윈도우 창 영역 안의 좌표로 변환해주는 함수.
	ScreenToClient(m_hWnd, &ptMouse);

	return ptMouse;
}

Engine::_vec3 Engine::CMouse_Manager::GetMouseRelativeGap(void)
{
	return GetMousePos() - m_vecInitialPos;
}

void Engine::CMouse_Manager::Initialize(HWND hWnd, int iWinCX, int iWinCY)
{
	m_hWnd = hWnd;

	m_vecInitialPos = _vec3(iWinCX / 2.f, iWinCY / 2.f, 0.f);
}

void Engine::CMouse_Manager::Set_MousePointer(CGameObject * pMousePointer)
{
	m_pMousePointer = pMousePointer;
	m_pMousePointer->Add_Ref();
}

void Engine::CMouse_Manager::Reset(void)
{
	Safe_Release(m_pMousePointer);
	m_pMousePointer = NULL;
}

void Engine::CMouse_Manager::Free(void)
{
	Safe_Release(m_pMousePointer);
}
