#include "stdafx.h"
#include "Scene_Logo.h"

#include "Component_Manager.h"
#include "Light_Manager.h"

#include "Back_Logo.h"
#include "Select_Logo.h"
#include "Loading.h"
#include "Scene_Transition.h"
#include "Management.h"
#include "InputDev.h"
#include "SoundMgr.h"

#include "SceneObserver.h"
#include "InfoSubject.h"
#include "DataManager.h"

USING(Engine) 

CScene_Logo::CScene_Logo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
	, m_pSceneObserver(nullptr)
{

}

CScene_Logo::~CScene_Logo()
{
}

HRESULT CScene_Logo::Ready_Scene(void)
{
	CDataManager::GetInstance()->Set_FadeColor(_vec4(1.f, 1.f, 1.f, 1.f));


	//CSoundMgr::GetInstance()->Play_Sound(L"Game of Thrones.mp3", Engine::CHANNEL_LOGO);

	g_eCurrentScene = SCENE_LOGO;
	CDataManager::GetInstance()->Set_Scene_ID(0);

	if (FAILED(Ready_LayerForBackGround()))
		return E_FAIL;

	m_pSceneObserver = CSceneObserver::Create();
	Engine::CInfoSubject::GetInstance()->Subscribe_Observer(m_pSceneObserver);

	return NOERROR;
}

int CScene_Logo::Update_Scene(const float & fTimeDelta)
{
	Engine::CScene::Update_Scene(fTimeDelta);

	// 시연회 때는 그냥 바로 이동
	if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_0))
	{
		Engine::CScene*		pNewScene = CScene_Transition::Create(m_pGraphicDev, CLoading::LOADING_STAGE1);
		if (nullptr == pNewScene)
			return -1;
		if(FAILED(Engine::CManagement::GetInstance()->SetUp_CurrentScene(pNewScene)))
			return -1;

		return 0;
	}
	else if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_9)) // Menu
	{
		Engine::CScene*		pNewScene = CScene_Transition::Create(m_pGraphicDev, CLoading::LOADING_MENU);
		if (nullptr == pNewScene)
			return -1;
		if (FAILED(Engine::CManagement::GetInstance()->SetUp_CurrentScene(pNewScene)))
			return -1;

		return 0;
	}

	else if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_8)) // Stage2
	{
		Engine::CScene*		pNewScene = CScene_Transition::Create(m_pGraphicDev, CLoading::LOADING_STAGE2);
		if (nullptr == pNewScene)
			return -1;
		if (FAILED(Engine::CManagement::GetInstance()->SetUp_CurrentScene(pNewScene)))
			return -1;

		return 0;
	}

	else if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_7)) // Lobby
	{
		Engine::CScene*		pNewScene = CScene_Transition::Create(m_pGraphicDev, CLoading::LOADING_LOBBY);
		if (nullptr == pNewScene)
			return -1;
		if (FAILED(Engine::CManagement::GetInstance()->SetUp_CurrentScene(pNewScene)))
			return -1;

		return 0;
	}

	Goto_TestScene();

	return 0;
}

void CScene_Logo::Render_Scene(void)
{

}

HRESULT CScene_Logo::Ready_LayerForBackGround(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	// Back_Logo
	pGameObject = CBack_Logo::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_LOGO, L"Layer_BackGround", pGameObject)))
		return E_FAIL;

	pGameObject = CSelect_Logo::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_LOGO, L"Layer_BackGround", pGameObject)))
		return E_FAIL;

	return NOERROR;
}

void CScene_Logo::Goto_TestScene(void)
{
	SCENEID eId = m_pSceneObserver->Get_SceneChange();
	if (SCENE_END != eId)
	{
		CLoading::LOADINGTYPE eType = CLoading::LOADING_END;
		switch (eId)
		{
		case TESTSCENE_SUENGHO:
			eType = CLoading::LOADING_TESTSCENE_SEUNGHO;
			break;
		case TESTSCENE_SEOCHAN:
			eType = CLoading::LOADING_TESTSCENE_SEOCHAN;
			break;
		case TESTSCENE_BACKCHUL:
			eType = CLoading::LOADING_TESTSCENE_BACKCHUL;
			break;
		case TESTSCENE_MINKYEONG:
			eType = CLoading::LOADING_TESTSCENE_MINKYEONG;
			break;

		case TESTSCENE_SUENGHO2:
			eType = CLoading::LOADING_TESTSCENE_SEUNGHO2;
			break;
		case TESTSCENE_SEOCHAN2:
			eType = CLoading::LOADING_TESTSCENE_SEOCHAN2;
			break;
		case TESTSCENE_BACKCHUL2:
			eType = CLoading::LOADING_TESTSCENE_BACKCHUL2;
			break;
		case TESTSCENE_MINKYEONG2:
			eType = CLoading::LOADING_TESTSCENE_MINKYEONG2;
			break;
		}

		Engine::CScene*		pNewScene = CScene_Transition::Create(m_pGraphicDev, eType);
		Engine::CManagement::GetInstance()->SetUp_CurrentScene(pNewScene);
	}
}

CScene_Logo * CScene_Logo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CScene_Logo* pInstance = new CScene_Logo(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CScene_Logo Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}


void CScene_Logo::Free(void)
{
	m_pObjectMgr->Release_GameObject(SCENE_LOGO);
	m_pComponentMgr->Release_Component(SCENE_LOGO);

	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pSceneObserver);
	DWORD dwRef = Engine::Safe_Release(m_pSceneObserver);

	Engine::CScene::Free();
}
