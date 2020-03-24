#include "stdafx.h"
#include "MainApp.h"

#include "InputDev.h"
#include "Timer_Manager.h"
#include "Frame_Manager.h"
#include "SoundMgr.h"
#include "Target_Manager.h"
#include "Component_Manager.h"
#include "Object_Manager.h"
#include "InfoSubject.h"
#include "Network.h"
#include "Management.h"
#include "MediaMgr.h"
#include "Mouse_Manager.h"
#include "time.h"
#include "ClientNetwork.h"
#include "Light_Manager.h"
#include "NavMgr.h"
#include "EffectMgr.h"
#include "DataManager.h"
#include "Camera_Manager.h"
#include "CollisionManager.h"
#include "FrustumManager.h"
#include "TextureManager.h"

#include "Scene_Transition.h"
#include "Scene_Logo.h"

CMainApp::CMainApp()
	: m_pGraphicDev(nullptr)
	, m_pInpuDev(nullptr)
	, m_pComponentMgr(nullptr)
	, m_pManagementMgr(nullptr)
	, m_pSoundMgr(nullptr)
{
}

CMainApp::~CMainApp()
{
}

HRESULT CMainApp::Ready_MainApp(void)
{
	Engine::CGraphicDev::WINMODE eMode;

#ifdef Demonstration
	eMode = Engine::CGraphicDev::MODE_WIN;
#else
	eMode = Engine::CGraphicDev::MODE_WIN;
#endif

	if (FAILED(Ready_DefaultSetting(eMode, WINCX, WINCY)))
		return E_FAIL;

	if (FAILED(Ready_StartScene()))
		return E_FAIL;


	return NOERROR;
}

int CMainApp::Update_MainApp(const float & fTimeDelta)
{
	Engine::CInputDev::GetInstance()->Set_InputState();

	m_pSoundMgr->UpdateSound();

	return m_pManagementMgr->Update_Management(fTimeDelta);
}

void CMainApp::Render_MainApp(void)
{
	if (nullptr == m_pGraphicDev)
		return;

	m_pGraphicDev->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 
		D3DCOLOR_XRGB(0, 0, 0), 1.f, 0);

	m_pGraphicDev->BeginScene();

	// 장치에 연결된 타겟에 렌더링한다.
	m_pManagementMgr->Render_Management();

	m_pGraphicDev->EndScene();

	// 0번타겟을 화면에 보여준다.
	m_pGraphicDev->Present(nullptr, nullptr, 0, nullptr);
}

HRESULT CMainApp::Ready_DefaultSetting(Engine::CGraphicDev::WINMODE eMode, const WORD& wSizeX, const WORD& wSizeY)
{
	// 필요한 매니져들의 주소를 받아놓자.
	m_pComponentMgr = Engine::CComponent_Manager::GetInstance();
	m_pComponentMgr->Add_Ref();

	m_pManagementMgr = Engine::CManagement::GetInstance();
	m_pManagementMgr->Add_Ref();

	m_pSoundMgr = Engine::CSoundMgr::GetInstance();
	m_pSoundMgr->Add_Ref();

	// 장치들에 대한 초기화를 수행하자.
	// 1.GraphicDev
	if (FAILED(Engine::CGraphicDev::GetInstance()->Ready_GraphicDev(eMode, g_hWnd, wSizeX, wSizeY)))
		goto error;

	m_pGraphicDev = Engine::CGraphicDev::GetInstance()->Get_GraphicDev();	
	if(nullptr == m_pGraphicDev)
		goto error;

	// 2.InputDev
	if (FAILED(Engine::CInputDev::GetInstance()->Ready_InputDevice(g_hInst, g_hWnd)))
		goto error;

	m_pInpuDev = Engine::CInputDev::GetInstance()->Get_InputDev();
	if (nullptr == m_pInpuDev)
		goto error;

	// 3. Mouse
	Engine::CMouse_Manager::GetInstance()->Initialize(g_hWnd, WINCX, WINCY);

	// 4. Media
	Engine::CMediaMgr::GetInstance()->Ready_MediaMgr(m_pGraphicDev, WINCX, WINCY);

	// 5. Server
#ifdef Server__
	if (S_OK == CClientNetwork::GetInstance()->InitNetwork(m_pGraphicDev))
	{
		std::packaged_task<void()> task(bind(&CClientNetwork::RecvData, CClientNetwork::GetInstance()));
		thread temp(move(task));
		temp.detach();
	}
#endif

	// 6. Sound
	m_pSoundMgr->LoadSoundFile();
	
	// 7. DataManager
	Engine::CDataManager::GetInstance()->Initialize_DataMgr();

	// 8. Frustum
	CFrustumManager::GetInstance()->Initialize_FrsutumMgr();

	// 9. TextureManager
	CTextureManager::GetInstance()->Initialize(m_pGraphicDev); // 필요한 텍스처 불러놓음.

	if (FAILED(m_pComponentMgr->Reserve_ComponentMgr(SCENE_END)))
		return E_FAIL;

	if (FAILED(Engine::CObject_Manager::GetInstance()->Reserve_ObjectMgr(SCENE_END)))
		return E_FAIL;

	// 10. EffectMgr
	Engine::CEffectMgr::GetInstance()->Init_EffectData();

	// 11. NavMgr
	Engine::CNavMgr::GetInstance()->ReserveCellSize(m_pGraphicDev, 1000);

	SetCursor(NULL);
	ShowCursor(FALSE);

	return NOERROR;

error:
	MSG_BOX(L"Default_Setting Failed");
	return E_FAIL;
}

HRESULT CMainApp::Ready_StartScene(void)
{
	Engine::CScene*	pScene = nullptr;
	
#ifdef Demonstration // 시연회
	pScene = CScene_Transition::Create(m_pGraphicDev, CLoading::LOADING_MENU);
#else
	#ifdef Lastman_Client
		pScene = CScene_Transition::Create(m_pGraphicDev, CLoading::LOADING_STAGE2);
	#else
		pScene = CScene_Transition::Create(m_pGraphicDev, CLoading::LOADING_LOGO);
#endif

#endif

	if (nullptr == pScene)
		return E_FAIL;
	if (FAILED(m_pManagementMgr->SetUp_CurrentScene(pScene)))
		return E_FAIL;

	return NOERROR;
}

CMainApp * CMainApp::Create(void)
{
	CMainApp* pInstance = new CMainApp();

	if (FAILED(pInstance->Ready_MainApp()))
	{
		MessageBox(0, L"CMainApp Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CMainApp::Free(void)
{	
	DWORD dwRefCnt = 0;

	Engine::Safe_Release(m_pComponentMgr);
	Engine::Safe_Release(m_pInpuDev);
	Engine::Safe_Release(m_pGraphicDev);
	Engine::Safe_Release(m_pManagementMgr);
	Engine::Safe_Release(m_pSoundMgr);


	///////////////////////////////////////////////////

	// Player, Monster : NavMgr
	if (dwRefCnt = Engine::CTextureManager::GetInstance()->DestroyInstance())
		MSG_BOX(L"CTextureManager Release Failed - CMainApp::Free()");

	if (dwRefCnt = Engine::CTimer_Manager::GetInstance()->DestroyInstance()) // X
		MSG_BOX(L"CTimer_Manager Release Failed - CMainApp::Free()");

	if (dwRefCnt = Engine::CFrame_Manager::GetInstance()->DestroyInstance()) // X
		MSG_BOX(L"CFrame_Manager Release Failed - CMainApp::Free()");

	if (dwRefCnt = Engine::CSoundMgr::GetInstance()->DestroyInstance()) // X
		MSG_BOX(L"CSoundMgr Release Failed - CMainApp::Free()");

	if (dwRefCnt = Engine::CManagement::GetInstance()->DestroyInstance()) // Renderer
		MSG_BOX(L"CManagement Release Failed - CMainApp::Free()");

	if (dwRefCnt = Engine::CMediaMgr::GetInstance()->DestroyInstance()) // X
		MSG_BOX(L"CMediaMgr Release Failed - CMainApp::Free()");

	if (dwRefCnt = Engine::CLight_Manager::GetInstance()->DestroyInstance()) // X
		MSG_BOX(L"CLight_Manager Release Failed - CMainApp::Free()");

	if (dwRefCnt = Engine::CEffectMgr::GetInstance()->DestroyInstance()) // GameObject
		MSG_BOX(L"CEffectMgr Release Failed - CMainApp::Free()");

	if (dwRefCnt = Engine::CObject_Manager::GetInstance()->DestroyInstance()) // GameObject
		MSG_BOX(L"CObject_Manager Release Failed - CMainApp::Free()");

	if (dwRefCnt = CClientNetwork::GetInstance()->DestroyInstance()) // GameObject
		MSG_BOX(L"CClientNetwork Release Failed - CMainApp::Free()");

	if (dwRefCnt = Engine::CNavMgr::GetInstance()->DestroyInstance()) // X
		MSG_BOX(L"CNavMgr Release Failed - CMainApp::Free()");

	if (dwRefCnt = CCamera_Manager::GetInstance()->DestroyInstance()) // FrustumMgr, CGameObject
		MSG_BOX(L"CCamera_Manager Release Failed - CMainApp::Free()");

	if (dwRefCnt = CCollisionManager::GetInstance()->DestroyInstance()) // CDataManager, CMouse_Manager
		MSG_BOX(L"CCollisionManager Release Failed - CMainApp::Free()");

	if (dwRefCnt = Engine::CMouse_Manager::GetInstance()->DestroyInstance()) // GameObject
		MSG_BOX(L"CMouse_Manager Release Failed - CMainApp::Free()");

	if (dwRefCnt = CFrustumManager::GetInstance()->DestroyInstance()) // X
		MSG_BOX(L"CFrustumManager Release Failed - CMainApp::Free()");

	if (dwRefCnt = Engine::CComponent_Manager::GetInstance()->DestroyInstance()) // GameObject들 아래
		MSG_BOX(L"CComponent_Manager Release Failed - CMainApp::Free()");

	if (dwRefCnt = Engine::CTarget_Manager::GetInstance()->DestroyInstance())
		MSG_BOX(L"CTarget_Manager Release Failed - CMainApp::Free()");

	if (dwRefCnt = Engine::CInputDev::GetInstance()->DestroyInstance())
		MSG_BOX(L"CInputDev Release Failed - CMainApp::Free()");

	if (dwRefCnt = Engine::CDataManager::GetInstance()->DestroyInstance())
		MSG_BOX(L"CDataManager Release Failed - CMainApp::Free()");

	if (dwRefCnt = Engine::CGraphicDev::GetInstance()->DestroyInstance())
		MSG_BOX(L"CGraphicDev Release Failed - CMainApp::Free()");

	// GameObject들이 InfoSubject의 GetInstance를 가지고 있으니 그들 아래에 있어야 한다
	if (dwRefCnt = Engine::CInfoSubject::GetInstance()->DestroyInstance())
		MSG_BOX(L"CInfoSubject Release Failed - CMainApp::Free()");
}
