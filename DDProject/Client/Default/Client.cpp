// Client.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"

#include "Client.h"

#include "../Codes/MainApp.h"

#include "Timer_Manager.h"
#include "Frame_Manager.h"


#define MAX_LOADSTRING 100

// ���� ����:

WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
HWND		 g_hWnd;
HINSTANCE	 g_hInst;                           // ���� �ν��Ͻ��Դϴ�.
bool		 g_bTextShow;

#ifdef Server__
int			 g_iClientIndex = -1;
#else
int			 g_iClientIndex = 1;
#endif

SCENEID		g_eCurrentScene = SCENE_END;
LASTMAN_PROGRESS g_eLastMan_Progress = LASTMAN_WAIT;
_int g_iRound = 0;

// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
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

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.

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

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
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

	// Timer_Default : �����ӽð��� �����ϱ����� Ÿ�̸�
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

    // �⺻ �޽��� �����Դϴ�.
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

			/*1�ʿ� 60���� Ʈ�縦 ����*/
			float fTimeDelta_Default = pTimer_Manager->Get_TimeDelta(L"Timer_Default");

			// FPS ���
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
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
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
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	g_hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

	RECT		rcWindow = { 0, 0, WINCX, WINCY };

	AdjustWindowRect(&rcWindow, WS_OVERLAPPEDWINDOW, true);

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top, nullptr, nullptr, hInstance, nullptr);

	//MoveWindow(hWnd, 0, 0, WINCX, WINCY, TRUE); // ���콺 ��ŷ ��ġ�� Ʋ��������

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
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �޴� ������ ���� �м��մϴ�.
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
            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
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

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
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