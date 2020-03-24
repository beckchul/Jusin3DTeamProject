#include "stdafx.h"
#include "Scene_Lobby.h"

#include "Component_Manager.h"
#include "Light_Manager.h"
#include "Camera_Manager.h"
#include "EffectMgr.h"
#include "DataManager.h"
#include "CollisionManager.h"
#include "ClientNetwork.h"
#include "NavMgr.h"
#include "Mouse_Manager.h"
#include "MousePointer.h"

#include "Loading.h"
#include "Scene_Transition.h"
#include "Management.h"
#include "InputDev.h"
#include "SoundMgr.h"

#include "Camera_Manager.h"
#include "DynamicCamera.h"
#include "RecordCamera.h"
#include "ActionCamera.h"
#include "StaticCamera.h"
#include "StandCamera.h"

#include "Font_NavMesh.h"
#include "MapTexture.h"

#include "HallOfHeroes.h"
#include "Crystal.h"
#include "Accessory1.h"
#include "Accessory2.h"
#include "UI_MapTitle.h"

USING(Engine)

CScene_Lobby::CScene_Lobby(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
	, m_pCameraMgr(CCamera_Manager::GetInstance())
	, m_pEffectMgr(CEffectMgr::GetInstance())
	, m_pClinetNetwork(CClientNetwork::GetInstance())
	, m_vecFadeColor(1.f, 1.f, 1.f, 1.f)
{
	m_pClinetNetwork->Add_Ref();
	m_pCameraMgr->Add_Ref();
	m_pEffectMgr->Add_Ref();
}

CScene_Lobby::~CScene_Lobby()
{
}

void CScene_Lobby::FirstRender_Finish()
{
	m_pCameraMgr->Change_Camera(CAMERA_ACTION, CUTSCENE_LOBBY);
}

HRESULT CScene_Lobby::Ready_Scene(void)
{
	CDataManager::GetInstance()->Set_FadeColor(_vec4(1.f, 1.f, 1.f, 1.f));
	CDataManager::GetInstance()->Set_BackColor(_vec4(0.f, 0.f, 0.f, 0.f));

	CSoundMgr::GetInstance()->StopSound(CHANNEL_MENU);
	CSoundMgr::GetInstance()->PlayBGM(L"Lobby.mp3", Engine::CHANNEL_MENU, 0.3f);

	g_eCurrentScene = SCENE_LOBBY;
	CDataManager::GetInstance()->Set_Scene_ID(2);

	if (FAILED(Ready_NavMesh()))
		return E_FAIL;
	if (FAILED(Ready_Effect()))
		return E_FAIL;
	if (FAILED(Ready_LayerForGameObject()))
		return E_FAIL;
	if (FAILED(Ready_LayerForUI()))
		return E_FAIL;
	if (FAILED(Ready_LayerForBackGround()))
		return E_FAIL;
	if (FAILED(Ready_MapObject()))
		return E_FAIL;
	if (FAILED(Ready_Light())) // MapObject 아래
		return E_FAIL;

	m_pCameraMgr->Change_Camera(CAMERA_STATIC, 0, 100000.f);

	CCollisionManager::GetInstance()->Initialize_CollisionMgr(); // NavMesh 정보 저장

	// 서버 <-> Scene 세팅 (플레이어, 채팅 등)
	m_pClinetNetwork->SetCurrentScene(SCENE_LOBBY);

	// 광원 - 뷰 행렬 만들기.
	_matrix matLightView, matLightProj;

	_vec3 vEyePt(46.f, 450.f, 0.f);
	_vec3 vLookatPt(0.f, 0.f, 0.f);
	_vec3 vUpVec(0.f, 1.f, 0.f);
	D3DXMatrixLookAtLH(&matLightView, &vEyePt, &vLookatPt, &vUpVec);

	// 광원 - 투영 행렬 만들기.
	D3DXMatrixPerspectiveFovLH(&matLightProj, D3DXToRadian(90.f), 1, 1.f, 2000.f);

	CDataManager::GetInstance()->Set_LightViewMatrix(matLightView);
	CDataManager::GetInstance()->Set_LightProjMatrix(matLightProj);

	return NOERROR;
}

int CScene_Lobby::Update_Scene(const float & fTimeDelta)
{
	// 슬슬어두워졌다가 다시 밝아지는 느낌으로 가자.
	// 2초에 걸쳐 어두워짐.

	if (g_bTextShow)
	{
		// 점점 어두워짐
		if (!m_bColorAdd)
		{
			if (m_vecFadeColor.x >= 0.f)
			{
				m_vecFadeColor -= _vec4(1.f, 1.f, 1.f, 1.f) * fTimeDelta * 1.f;
				CDataManager::GetInstance()->Set_FadeColor(m_vecFadeColor);
			}
			else
				m_bColorAdd = true;
		}

		// 점점 밝아짐.
		if (m_bColorAdd)
		{
			if (m_vecFadeColor.x <= 1.f)
			{
				m_vecFadeColor += _vec4(1.f, 1.f, 1.f, 1.f) * fTimeDelta * 0.5f;
				CDataManager::GetInstance()->Set_FadeColor(m_vecFadeColor);
			}
			else
			{
				// 다 밝아졌으면 다시 g_bTextShow를 false
				g_bTextShow = false;
			}
		}
	}


	m_pClinetNetwork->Check_Player_Create();
	m_pClinetNetwork->Check_Player_Delete();

	Engine::CScene::Update_Scene(fTimeDelta);

	m_pCameraMgr->Update_Camara(fTimeDelta); // 카메라
	m_pEffectMgr->Update_Effect(fTimeDelta);

	CNavMgr::GetInstance()->Update_NavMesh(fTimeDelta); // 내비메쉬(폰트) -> 시연회땐 주석

	return 0;
}

void CScene_Lobby::Render_Scene(void)
{
	if (CInputDev::GetInstance()->OnceKeyDown(DIK_END))
		g_eCurrentScene = SCENE_STAGE;

	if(g_eCurrentScene == SCENE_STAGE2 || g_eCurrentScene == SCENE_STAGE) // Stage1? Stage2?
	{
		CLoading::LOADINGTYPE eType = CLoading::LOADING_END;
		switch (g_eCurrentScene)
		{
		case SCENE_STAGE:
			eType = CLoading::LOADING_STAGE1;
			break;

		case SCENE_STAGE2:
			eType = CLoading::LOADING_STAGE2;
			break;
		}

		Engine::CScene*		pNewScene = CScene_Transition::Create(m_pGraphicDev, eType);
		if (nullptr == pNewScene)
			return;
		if (FAILED(Engine::CManagement::GetInstance()->SetUp_CurrentScene(pNewScene)))
			return;
	}

	//CNavMgr::GetInstance()->Render_NavMesh(); // 내비메쉬(Cell)
}

HRESULT CScene_Lobby::Ready_Effect(void)
{
	return NOERROR;
}

HRESULT CScene_Lobby::Ready_Light(void)
{
	CLight_Manager::GetInstance()->Reset_Light(); // 기존 Light Reset

	D3DLIGHT9		LightInfo;
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

	// Light0(촛대)
	vector<_vec3> vecLightPos0 = Engine::CDataManager::GetInstance()->Get_LightPosition(Engine::OBJECT_POINT_LIGHT0);

	for (size_t i = 0; i < vecLightPos0.size(); ++i)
	{ 
		// 여기서 텍스처 추가하면 돼.민경아~
		Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Lobby_Candle", &vecLightPos0[i]);
	}

	// Directional
	LightInfo.Type = D3DLIGHT_DIRECTIONAL;
	LightInfo.Direction = _vec3(0.f, -0.57735f, 0.f);
	LightInfo.Diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);
	LightInfo.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);
	LightInfo.Specular = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);

	if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Lobby::Ready_NavMesh(void)
{
	DWORD dwByte = 0;

	HANDLE hFile = CreateFile(L"../Resources/Data/NavMesh/Data3.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	while (true)
	{
		Engine::NAVMESH NavMesh;
		ZeroMemory(&NavMesh, sizeof(Engine::NAVMESH));

		ReadFile(hFile, &NavMesh, sizeof(Engine::NAVMESH), &dwByte, NULL);

		if (dwByte == 0)
			break;

		Engine::CGameObject* pFontNumber = CFont_NavMesh::Create(m_pGraphicDev, NavMesh.iIndex);

		CNavMgr::GetInstance()->AddCell(&NavMesh, pFontNumber);
	}
	CloseHandle(hFile);

	CNavMgr::GetInstance()->LinkCell();

	return NOERROR;
}

HRESULT CScene_Lobby::Ready_MapObject(void)
{
	DWORD dwByte = 0;

	HANDLE hFile = CreateFile(L"../Resources/Data/Object/Lobby.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	while (true)
	{
		Engine::OBJECT Object;
		ZeroMemory(&Object, sizeof(Engine::OBJECT));

		ReadFile(hFile, &Object, sizeof(Engine::OBJECT), &dwByte, NULL);

		if (dwByte == 0)
			break;

		// Add Object
		Add_MapObject(Object);

	}
	CloseHandle(hFile);

	return NOERROR;
}

HRESULT CScene_Lobby::Add_MapObject(Engine::OBJECT & tObject)
{
	Engine::CGameObject* pGameObject = nullptr;

	switch (tObject.eId)
	{
	case Engine::OBJECT3_CRYSTAL:
		pGameObject = CCrystal::Create(m_pGraphicDev);
		break;
	case Engine::OBJECT3_ACCESSORY1:
		pGameObject = CAccessory1::Create(m_pGraphicDev);
		break;
	case Engine::OBJECT3_ACCESSORY2:
		pGameObject = CAccessory2::Create(m_pGraphicDev);
		break;

	case Engine::OBJECT3_POINT_LIGHT0: // 둥그런애
	case Engine::OBJECT3_POINT_LIGHT1: // 촛대같은거
	case Engine::OBJECT3_POINT_LIGHT2:
	case Engine::OBJECT3_POINT_LIGHT3:
	case Engine::OBJECT3_POINT_LIGHT4:
		CDataManager::GetInstance()->Add_PointLight(tObject.vecPos, tObject.eId, OBJECT3_POINT_LIGHT0);
		//pGameObject = CMapTexture::Create(m_pGraphicDev, tObject.eId);
		break;

	default:
		pGameObject = CHallOfHeroes::Create(m_pGraphicDev, (_int)tObject.eId);
		break;
	}

	if (nullptr == pGameObject)
		return E_FAIL;

	Engine::CTransform* pTransform = (Engine::CTransform*)pGameObject->Get_Component(Engine::CComponent::COM_DYNAMIC, L"Com_Transform");
	pTransform->Set_Information(CTransform::INFO_POSITION, &tObject.vecPos);
	pTransform->Set_Information(CTransform::INFO_ANGLE, (_vec3*)(tObject.fAngle));
	pTransform->Set_Information(CTransform::INFO_SCALE, &tObject.vecScale);

	m_pObjectMgr->Add_GameObject(SCENE_LOBBY, L"Layer_GameObject", pGameObject);

	return NOERROR;
}

HRESULT CScene_Lobby::Ready_LayerForBackGround(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	// Map

	return NOERROR;
}

HRESULT CScene_Lobby::Ready_LayerForGameObject(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	return NOERROR;
}

HRESULT CScene_Lobby::Ready_LayerForUI(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	//// Camera
	AddCamera(CStaticCamera, CAMERA_STATIC);
	AddCamera(CDynamicCamera, CAMERA_DYNAMIC);
	AddCamera(CActionCamera, CAMERA_ACTION);
	AddCamera(CRecordCamera, CAMERA_RECORDING);
	AddCamera(CStandCamera, CAMERA_STAND);

	pGameObject = CMousePointer::Create(m_pGraphicDev);
	CObject_Manager::GetInstance()->Add_GameObject(SCENE_LOBBY, L"Layer_UI", pGameObject);
	Engine::CMouse_Manager::GetInstance()->Set_MousePointer(pGameObject);

	pGameObject = CUI_MapTitle::Create(m_pGraphicDev, 0);
	pGameObject->Set_Pos(&_vec3(-88.f, 25.f, 0.3f));
	CObject_Manager::GetInstance()->Add_GameObject(SCENE_LOBBY, L"Layer_UI", pGameObject);

	pGameObject = CUI_MapTitle::Create(m_pGraphicDev, 1);
	pGameObject->Set_Pos(&_vec3(94.f, 25.f, -0.15f));
	CObject_Manager::GetInstance()->Add_GameObject(SCENE_LOBBY, L"Layer_UI", pGameObject);

	return NOERROR;
}

CScene_Lobby * CScene_Lobby::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CScene_Lobby* pInstance = new CScene_Lobby(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CScene_Lobby Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CScene_Lobby::Free(void)
{
	m_pEffectMgr->Release_Effect();
	m_pClinetNetwork->Stage_Clear();
	m_pCameraMgr->Reset_Camera();
	CNavMgr::GetInstance()->Reset_Cell();
	CDataManager::GetInstance()->Reset_Data();
	Engine::CMouse_Manager::GetInstance()->Reset();

	Engine::Safe_Release(m_pClinetNetwork);
	Engine::Safe_Release(m_pCameraMgr);
	Engine::Safe_Release(m_pEffectMgr);

	m_pObjectMgr->Release_GameObject(SCENE_LOBBY);
	m_pComponentMgr->Release_Component(SCENE_LOBBY);

	CLight_Manager::GetInstance()->Reset_Light();

	Engine::CScene::Free();
}
