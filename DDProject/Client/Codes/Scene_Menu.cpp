#include "stdafx.h"
#include "Scene_Menu.h"

#include "Component_Manager.h"
#include "Light_Manager.h"
#include "Camera_Manager.h"
#include "EffectMgr.h"

#include "Loading.h"
#include "Scene_Transition.h"
#include "Management.h"
#include "InputDev.h"
#include "SoundMgr.h"

#include "SceneObserver.h"
#include "InfoSubject.h"
#include "DataManager.h"

#include "MenuMap.h"

#include "Camera_Manager.h"
#include "DynamicCamera.h"
#include "RecordCamera.h"
#include "ActionCamera.h"
#include "StandCamera.h"

#include "Effect_Frame.h"
#include "Effect_Particle.h"
#include "Effect_Single.h"

USING(Engine)

CScene_Menu::CScene_Menu(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
	, m_pCameraMgr(CCamera_Manager::GetInstance())
	, m_pEffectMgr(CEffectMgr::GetInstance())
{
	m_pCameraMgr->Add_Ref();
	m_pEffectMgr->Add_Ref();
}

CScene_Menu::~CScene_Menu()
{
}

void CScene_Menu::FirstRender_Finish()
{
	m_pCameraMgr->Change_Camera(CAMERA_ACTION, CUTSCENE_MENU);
}

HRESULT CScene_Menu::Ready_Scene(void)
{
	CDataManager::GetInstance()->Set_FadeColor(_vec4(1.f, 1.f, 1.f, 1.f));


	CSoundMgr::GetInstance()->PlayBGM(L"Menu.mp3", Engine::CHANNEL_MENU, 0.5f);

	g_eCurrentScene = SCENE_MENU;
	CDataManager::GetInstance()->Set_Scene_ID(1);

	if (FAILED(Ready_Effect()))
		return E_FAIL;
	if (FAILED(Ready_LayerForBackGround()))
		return E_FAIL;
	if (FAILED(Ready_LayerForGameObject()))
		return E_FAIL;
	if (FAILED(Ready_LayerForUI()))
		return E_FAIL;
	if (FAILED(Ready_Light()))
		return E_FAIL;

	m_pCameraMgr->Change_Camera(CAMERA_DYNAMIC);

	m_pSceneObserver = CSceneObserver::Create();
	Engine::CInfoSubject::GetInstance()->Subscribe_Observer(m_pSceneObserver);

	return NOERROR;
}

int CScene_Menu::Update_Scene(const float & fTimeDelta)
{
	Engine::CScene::Update_Scene(fTimeDelta);

	m_pCameraMgr->Update_Camara(fTimeDelta); // 카메라
	m_pEffectMgr->Update_Effect(fTimeDelta);

	return 0;
}

void CScene_Menu::Render_Scene(void)
{
	SCENEID eId = m_pSceneObserver->Get_SceneChange();
	if (eId == SCENE_LOBBY)
	{
		CLoading::LOADINGTYPE eType =  CLoading::LOADING_LOBBY;

		Engine::CScene*		pNewScene = CScene_Transition::Create(m_pGraphicDev, eType);
		if (nullptr == pNewScene)
			return;
		if (FAILED(Engine::CManagement::GetInstance()->SetUp_CurrentScene(pNewScene)))
			return;
	}
}

HRESULT CScene_Menu::Ready_Effect(void)
{
	// Scene_Menu랑 Scene_Stage1 이랑 구분이 안되는것 같다
	// 이펙트 리스트들을 추가해주자.

	return NOERROR;
}

HRESULT CScene_Menu::Ready_Light(void)
{
	DWORD dwByte = 0;

	HANDLE hFile = CreateFile(L"../Resources/Data/Object/Menu.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	while (true)
	{
		Engine::OBJECT Object;
		ZeroMemory(&Object, sizeof(Engine::OBJECT));

		ReadFile(hFile, &Object, sizeof(Engine::OBJECT), &dwByte, NULL);

		if (dwByte == 0)
			break;

		D3DLIGHT9		LightInfo;
		ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

		if (OBJECT_POINT_LIGHT0 == Object.eId) // 빛0
		{
			// 벽 횃불
			LightInfo.Type = D3DLIGHT_POINT;
			LightInfo.Position = Object.vecPos;
			LightInfo.Range = 13.f;
			LightInfo.Diffuse = D3DXCOLOR(1.f, 0.45f, 0.f, 8.f);
			LightInfo.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
			LightInfo.Specular = D3DXCOLOR(1.f, 0.45f, 0.f, 1.f);
			// Effect_Torch
			m_pEffectMgr->Add_Effect(L"Effect_MenuTorch", &_vec3(Object.vecPos.x, Object.vecPos.y, Object.vecPos.z));
			m_pEffectMgr->Add_Effect(L"Effect_MenuTorch_Blue", &_vec3(Object.vecPos.x, Object.vecPos.y -0.4f, Object.vecPos.z));

		}
		else if (OBJECT_POINT_LIGHT1 == Object.eId) // 빛1
		{
			// 난로
			LightInfo.Type = D3DLIGHT_POINT;
			LightInfo.Position = Object.vecPos;
			LightInfo.Range = 15.f;
			LightInfo.Diffuse = D3DXCOLOR(1.f, 0.15f, 0.f, 2.f);
			LightInfo.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
			LightInfo.Specular = D3DXCOLOR(1.f, 0.15f, 0.f, 1.f);
			// Effect_FirePlace
			//m_pEffectMgr->Add_Effect(L"Effect_Brazier_Fire", &_vec3(Object.vecPos.x, Object.vecPos.y, Object.vecPos.z));

			m_pEffectMgr->Add_Effect(L"Effect_Upside_FirePlace", &_vec3(Object.vecPos.x, Object.vecPos.y+0.4f, Object.vecPos.z));
			m_pEffectMgr->Add_Effect(L"Effect_Downside_FirePlace", &_vec3(Object.vecPos.x, Object.vecPos.y - 0.47f , Object.vecPos.z + 0.1f));

		}

		if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
			return E_FAIL;

	}
	CloseHandle(hFile);

	D3DLIGHT9		LightInfo;
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

	LightInfo.Type = D3DLIGHT_DIRECTIONAL;
	LightInfo.Direction = _vec3(0.f, -0.57735f, 0.f);
	LightInfo.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);
	LightInfo.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);
	LightInfo.Specular = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);

	if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Menu::Ready_LayerForBackGround(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	// Map
	AddGameObject(CMenuMap, SCENE_MENU, L"Layer_BackGround");

	return NOERROR;
}

HRESULT CScene_Menu::Ready_LayerForGameObject(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	return NOERROR;
}

HRESULT CScene_Menu::Ready_LayerForUI(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	//// Camera
	AddCamera(CDynamicCamera, CAMERA_DYNAMIC);
	AddCamera(CActionCamera, CAMERA_ACTION);
	AddCamera(CRecordCamera, CAMERA_RECORDING);
	AddCamera(CStandCamera, CAMERA_STAND);
	
	return NOERROR;
}

CScene_Menu * CScene_Menu::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CScene_Menu* pInstance = new CScene_Menu(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CScene_Menu Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CScene_Menu::Free(void)
{
	m_pEffectMgr->Release_Effect();
	m_pCameraMgr->Reset_Camera();

	Engine::Safe_Release(m_pCameraMgr);
	Engine::Safe_Release(m_pEffectMgr);

	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pSceneObserver);
	DWORD dwRef = Engine::Safe_Release(m_pSceneObserver);

	m_pObjectMgr->Release_GameObject(SCENE_MENU);
	m_pComponentMgr->Release_Component(SCENE_MENU);

	CLight_Manager::GetInstance()->Reset_Light();

	Engine::CScene::Free();
}
