#include "stdafx.h"
#include "Scene_Transition.h"
#include "Component_Manager.h"
#include "InputDev.h"

#include "Loading.h"
#include "Management.h"

#include "Scene_Logo.h"
#include "Scene_Menu.h"
#include "Scene_Stage.h"
#include "Scene_Stage2.h"
#include "Scene_Lobby.h"

#include "TestScene_BackChul.h"
#include "TestScene_MinKyeong.h"
#include "TestScene_SeoChan.h"
#include "TestScene_SeungHo.h"

//#include "TestScene_BackChul2.h"
//#include "TestScene_MinKyeong2.h"
#include "TestScene_SeoChan2.h"
//#include "TestScene_SeungHo2.h"

#include "MediaMgr.h"

CScene_Transition::CScene_Transition(LPDIRECT3DDEVICE9 pGraphicDev, CLoading::LOADINGTYPE eType)
	: Engine::CScene(pGraphicDev)
	, m_eLoadingTpye(eType)
	, m_pLoading(nullptr)
	, m_pMediaMgr(Engine::CMediaMgr::GetInstance())
	, m_bShowVideo(false)
{
	m_pMediaMgr->Add_Ref();
}


CScene_Transition::~CScene_Transition()
{
}

HRESULT CScene_Transition::Ready_Scene(void)
{
	if (FAILED(Ready_LayerForBackGround()))
		return E_FAIL;

	if (FAILED(Ready_Loading()))
		return E_FAIL;

	switch (m_eLoadingTpye)
	{
	case CLoading::LOADING_LOGO:
		m_pMediaMgr->VideoPlay(L"../Resources/Movie/test2.avi");
		break;

	default:
		m_pMediaMgr->VideoPlay(L"../Resources/Movie/test2.avi");
		break;
	}

#ifdef Demonstration
	m_bShowVideo = true;
#endif

	return NOERROR;
}

int CScene_Transition::Update_Scene(const float & fTimeDelta)
{
	Engine::CScene::Update_Scene(fTimeDelta);

	if(m_bShowVideo)
		m_pMediaMgr->Progress();

	return 0;
}

void CScene_Transition::Render_Scene(void) // 영상 재생
{
	if (m_bShowVideo == true)
		m_pMediaMgr->Render();

	if (nullptr == m_pLoading)
		return;

	if (true == m_pLoading->Get_Complete()) // 로딩이 끝났으면 이동
	{
		if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_RETURN))
			m_bShowVideo = false;

		// 영상 재생 or not
		if (m_bShowVideo == true)
			return;

#ifdef Server__
		if (-1 == g_iClientIndex) // 서버로부터 인덱스 받고 넘어가자
			return;
#endif

		Engine::CScene*		pNewScene = nullptr;

		switch (m_eLoadingTpye)
		{
		case CLoading::LOADING_LOGO:
			pNewScene = CScene_Logo::Create(m_pGraphicDev);
			break;

		case CLoading::LOADING_MENU:
			pNewScene = CScene_Menu::Create(m_pGraphicDev);
			break;

		case CLoading::LOADING_STAGE1:
			pNewScene = CScene_Stage::Create(m_pGraphicDev);
			break;

		case CLoading::LOADING_STAGE2:
			pNewScene = CScene_Stage2::Create(m_pGraphicDev);
			break; 

		case CLoading::LOADING_LOBBY:
			pNewScene = CScene_Lobby::Create(m_pGraphicDev);
			break; 

			// 테스트씬
		case CLoading::LOADING_TESTSCENE_SEUNGHO:
			pNewScene = CTestScene_SeungHo::Create(m_pGraphicDev);
			break;
		case CLoading::LOADING_TESTSCENE_SEOCHAN:
			pNewScene = CTestScene_SeoChan::Create(m_pGraphicDev);
			break;
		case CLoading::LOADING_TESTSCENE_BACKCHUL:
			pNewScene = CTestScene_BackChul::Create(m_pGraphicDev);
			break;
		case CLoading::LOADING_TESTSCENE_MINKYEONG:
			pNewScene = CTestScene_MinKyeong::Create(m_pGraphicDev);
			break;

		case CLoading::LOADING_TESTSCENE_SEUNGHO2:
			//pNewScene = CTestScene_SeungHo2::Create(m_pGraphicDev);
			break;
		case CLoading::LOADING_TESTSCENE_SEOCHAN2:
			pNewScene = CTestScene_SeoChan2::Create(m_pGraphicDev);
			break;
		case CLoading::LOADING_TESTSCENE_BACKCHUL2:
			//pNewScene = CTestScene_BackChul2::Create(m_pGraphicDev);
			break;
		case CLoading::LOADING_TESTSCENE_MINKYEONG2:
			//pNewScene = CTestScene_MinKyeong2::Create(m_pGraphicDev);
			break;
		}

		if (nullptr == pNewScene)
			return;
		if (FAILED(Engine::CManagement::GetInstance()->SetUp_CurrentScene(pNewScene)))
			return;

		return;
	}
}

HRESULT CScene_Transition::Ready_LayerForBackGround(void) // 영상 준비
{
	Engine::CGameObject*		pGameObject = nullptr;

	return NOERROR;
}

HRESULT CScene_Transition::Ready_Loading(void)
{
	if (nullptr != m_pLoading)
		return E_FAIL;

	m_pLoading = CLoading::Create(m_pGraphicDev, m_eLoadingTpye);
	if (nullptr == m_pLoading)
		return E_FAIL;

	return NOERROR;
}

CScene_Transition * CScene_Transition::Create(LPDIRECT3DDEVICE9 pGraphicDev, CLoading::LOADINGTYPE eType)
{
	CScene_Transition* pInstance = new CScene_Transition(pGraphicDev, eType);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CScene_Transition Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}


void CScene_Transition::Free(void)
{
	Engine::Safe_Release(m_pLoading);

	m_pMediaMgr->StopPlay();
	DWORD dwRefCnt = Engine::Safe_Release(m_pMediaMgr);

	Engine::CScene::Free();
}
