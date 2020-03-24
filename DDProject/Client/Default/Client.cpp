// Client.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"

#include "Client.h"

#include "../Codes/MainApp.h"

#include "Timer_Manager.h"
#include "Frame_Manager.h"


#define MAX_LOADSTRING 100

// 전역 변수:

WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND		 g_hWnd;
HINSTANCE	 g_hInst;                           // 현재 인스턴스입니다.
bool		 g_bTextShow;

#ifdef Server__
int			 g_iClientIndex = -1;
#else
int			 g_iClientIndex = 1;
#endif

SCENEID		g_eCurrentScene = SCENE_END;
LASTMAN_PROGRESS g_eLastMan_Progress = LASTMAN_WAIT;
_int g_iRound = 0;

// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	// Alloc Console

#ifndef Demonstration 
#ifndef Lastman_Client
	FILE* fp = nullptr;

	if (AllocConsole())
	{
		freopen_s(&fp, "CONIN$", "rb", stdin);
		freopen_s(&fp, "CONOUT$", "wb", stdout);
		freopen_s(&fp, "CONOUT$", "wb", stderr);
	}
#endif
#endif

	CMainApp*		pMainApp = nullptr;	

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	pMainApp = CMainApp::Create();
	if (nullptr == pMainApp)
		return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));

    MSG msg;

	Engine::CTimer_Manager*		pTimer_Manager = Engine::CTimer_Manager::GetInstance();
	pTimer_Manager->Add_Ref();

	Engine::CFrame_Manager*		pFrame_Manager = Engine::CFrame_Manager::GetInstance();
	pFrame_Manager->Add_Ref();

	Engine::CTimer*		pTimer = nullptr;

	// Timer_Default : 프레임시간을 관리하기위한 타이머
	pTimer = Engine::CTimer::Create();
	if (nullptr == pTimer)
		return FALSE;
	if (FAILED(pTimer_Manager->Ready_Timer(L"Timer_Default", pTimer)))
		return FALSE;

#ifdef LIMIT_FPS
	// For.Timer_FPS:60
	pTimer = Engine::CTimer::Create();
	if (nullptr == pTimer)
		return FALSE;
	if (FAILED(pTimer_Manager->Ready_Timer(L"Timer_60", pTimer)))
		return FALSE;

	Engine::CFrame*		pFrame = nullptr;

	// Frame : FPS_60
	pFrame = Engine::CFrame::Create(DEFAULT_FPS);
	if (nullptr == pFrame)
		return FALSE;
	if (FAILED(pFrame_Manager->Ready_Frame(L"Frame_60", pFrame)))
		return FALSE;
#endif

	TCHAR szFPS[64] = {};
	int iFps = 0;
	float fFpsTime = 0.f;

    // 기본 메시지 루프입니다.
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message)
				break;
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (nullptr == pMainApp)
				return FALSE;

			pTimer_Manager->Compute_TimeDelta(L"Timer_Default");

			/*1초에 60번만 트루를 리턴*/
			float fTimeDelta_Default = pTimer_Manager->Get_TimeDelta(L"Timer_Default");

			// FPS 출력
			fFpsTime += fTimeDelta_Default;

			if (fFpsTime > 1.f)
			{
				wsprintf(szFPS, L"FPS : %d", iFps);

				SetWindowText(g_hWnd, szFPS);

				fFpsTime = 0.f;
				iFps = 0;
			}

#ifdef LIMIT_FPS
			if(pFrame_Manager->Permit_Call(L"Frame_60", fTimeDelta_Default))
			{
				pTimer_Manager->Compute_TimeDelta(L"Timer_60");
				float fTimeDelta_60 = pTimer_Manager->Get_TimeDelta(L"Timer_60");

				int iExitCode = pMainApp->Update_MainApp(fTimeDelta_60);
				if (iExitCode & 0x80000000)
					break;

				pMainApp->Render_MainApp();

				iFps++;
			}
#else
			int iExitCode = pMainApp->Update_MainApp(fTimeDelta_Default);
			if (iExitCode & 0x80000000)
				break;

			pMainApp->Render_MainApp();

			iFps++;
#endif
		}

	}

	Safe_Release(pTimer_Manager);
	Safe_Release(pFrame_Manager);

	unsigned long dwRefCnt = pMainApp->Release();

#ifndef Demonstration 
#ifndef Lastman_Client
	FreeConsole();
#endif
#endif
    
    return (int) msg.wParam;
}

//
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= nullptr;	//MAKEINTRESOURCEW(IDC_CLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	RECT		rcWindow = { 0, 0, WINCX, WINCY };

	AdjustWindowRect(&rcWindow, WS_OVERLAPPEDWINDOW, true);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, nullptr, nullptr, hInstance, nullptr);

	//MoveWindow(hWnd, 0, 0, WINCX, WINCY, TRUE); // 마우스 피킹 위치를 틀어지게함

	if (!hWnd)
	{
		return FALSE;
	}

	g_hWnd = hWnd;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
#ifndef Lastman_Client
			PostQuitMessage(0);
#endif
			break;
		default:
			break;
		}
		break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}