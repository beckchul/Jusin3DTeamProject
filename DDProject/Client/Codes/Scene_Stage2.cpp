#include "stdafx.h"
#include "Scene_Stage2.h"
#include "Component_Manager.h"
#include "ClientNetwork.h"
#include "NavMgr.h"
#include "Camera_Manager.h"
#include "EffectMgr.h"
#include "Light_Manager.h"
#include "DataManager.h"
#include "FrustumManager.h"
#include "CollisionManager.h"
#include "EffectMgr.h"
#include "Light_Manager.h"
#include "SoundMgr.h"
#include "InputDev.h"
#include "Stage2_SkyBall.h"
#include "Stage2_SkyBox.h"

// BackGround
#include "BlockGroup.h"
#include "Ribbon.h"

#include "Crystal.h"
#include "Accessory1.h"
#include "Accessory2.h"
#include "TresureBox.h"

// GameObject

// Camera
#include "Goblin_Camera.h"
#include "RecordCamera.h"
#include "DynamicCamera.h"
#include "ActionCamera.h"
#include "FloatCamera.h"
#include "Victory_Camera.h"

// UI
#include "Font_NavMesh.h"
#include "Font_ActivateCrystal.h"
#include "MapTexture.h"
#include "Panel_Login.h"
#include "Player_Left.h"
#include "Font_FPS.h"
#include "Text_Electric.h"
#include "Text_GameStart.h"

// Effect

///////// Effect_Mesh

///// Observer
#include "LastManObserver.h"
#include "InfoSubject.h"

///// CollisionMgr
#include "CollisionManager.h"

// GameObject
#include "Computer_Goblin.h"

#include "Effect_Magnetic_Sphere.h"

CScene_Stage2::CScene_Stage2(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
	, m_pClinetNetwork(CClientNetwork::GetInstance())
	, m_pNavMgr(CNavMgr::GetInstance())
	, m_pCameraMgr(CCamera_Manager::GetInstance())
	, m_pEffectMgr(CEffectMgr::GetInstance())
	, m_pInfoSubject(Engine::CInfoSubject::GetInstance())
	, m_eSceneId(SCENE_STAGE2)
	, m_fRoundTime(0.f)
	, m_vecFadeColor(1.f, 1.f, 1.f, 1.f)
{
	m_pClinetNetwork->Add_Ref();
	m_pNavMgr->Add_Ref();
	m_pCameraMgr->Add_Ref();
	m_pEffectMgr->Add_Ref();
	m_pInfoSubject->Add_Ref();
}

CScene_Stage2::~CScene_Stage2()
{
}

HRESULT CScene_Stage2::Ready_Scene(void)
{
	CDataManager::GetInstance()->Set_FadeColor(_vec4(1.f, 1.f, 1.f, 1.f));
	CDataManager::GetInstance()->Set_BackColor(_vec4(0.f, 0.f, 0.f, 0.f));

	CSoundMgr::GetInstance()->StopSound(CHANNEL_MENU);
	CSoundMgr::GetInstance()->StopSound(CHANNEL_STAGE);

	g_eCurrentScene = SCENE_STAGE2;
	CDataManager::GetInstance()->Set_Scene_ID(4);

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
	if (FAILED(Ready_RandomData())) 
		return E_FAIL;
	if (FAILED(Ready_Light())) // MapObject 아래
		return E_FAIL;

	CCollisionManager::GetInstance()->Initialize_CollisionMgr(); // NavMesh 정보 저장

	 // 서버 <-> Scene 세팅 (플레이어, 채팅 등)
	m_pClinetNetwork->SetCurrentScene(SCENE_STAGE2);

	m_pInfoSubject->Add_Data(MESSAGE_SCENECHANGE, &m_eSceneId);
	
	m_pInfoSubject->Add_Data(MESSAGE_LASTMAN_COMPUTER_ADD, &m_eSceneId);
	m_pInfoSubject->Add_Data(MESSAGE_LASTMAN_COMPUTER_DEAD, &m_eSceneId);
	m_pInfoSubject->Add_Data(MESSAGE_LASTMAN_PLAYER_ADD, &m_eSceneId);
	m_pInfoSubject->Add_Data(MESSAGE_LASTMAN_PLAYER_DEAD, &m_eSceneId);
	
	return NOERROR;
}

int CScene_Stage2::Update_Scene(const float & fTimeDelta)
{
	CDataManager::GetInstance()->Set_BackColor(_vec4(0.f, 0.f, 0.f, 0.f));

	if (FALSE == CSoundMgr::GetInstance()->IsPlaying(Engine::CHANNEL_TOWER) &&
		FALSE == CSoundMgr::GetInstance()->IsPlaying(Engine::CHANNEL_TOWER_2) &&
		FALSE == CSoundMgr::GetInstance()->IsPlaying(Engine::CHANNEL_TOWER_3) &&
		FALSE == CSoundMgr::GetInstance()->IsPlaying(Engine::CHANNEL_TOWER_ATTACK))
	{
		switch (m_iTrack)
		{
		case 0:
			CSoundMgr::GetInstance()->Play_Sound(L"IDIOTAPE.mp3", Engine::CHANNEL_TOWER, 0.2f);
			++m_iTrack;
			break;
		case 1:
			CSoundMgr::GetInstance()->Play_Sound(L"Stage2.mp3", Engine::CHANNEL_TOWER_2, 0.3f);
			++m_iTrack;
			break; 
		case 2:
			CSoundMgr::GetInstance()->Play_Sound(L"TheFatRat.mp3", Engine::CHANNEL_TOWER_3, 0.2f);
			++m_iTrack;
			break;
		case 3:
			CSoundMgr::GetInstance()->Play_Sound(L"skrillex.mp3", Engine::CHANNEL_TOWER_ATTACK, 0.25f);
			++m_iTrack;
			break;
		}
		
		if (m_iTrack == 4)
			m_iTrack = 0;
	}

	m_pClinetNetwork->Check_Player_Create2();
	m_pClinetNetwork->Check_Player_Delete2();

	m_pCameraMgr->Update_Camara(fTimeDelta); // 카메라

	Engine::CScene::Update_Scene(fTimeDelta);

#ifndef Demonstartion
	m_pNavMgr->Update_NavMesh(fTimeDelta); // 내비메쉬(폰트) -> 시연회땐 주석
#endif
	m_pEffectMgr->Update_Effect(fTimeDelta); // 이펙트

	Lastman_Progress();
	Lastman_Timer(fTimeDelta);

	return 0;
}

void CScene_Stage2::Render_Scene(void)
{
	m_pNavMgr->Render_NavMesh(); // 내비메쉬(Cell)
	CCollisionManager::GetInstance()->Render_CollisionSphere();
}

void CScene_Stage2::FirstRender_Finish(void)
{
	m_pCameraMgr->Change_Camera(CAMERA_ACTION, CUTSCENE_STAGE_2);
}

HRESULT CScene_Stage2::Ready_Effect(void)
{
	m_pMagneticSphere = CEffect_Magnetic_Sphere::Create(m_pGraphicDev);
	if (nullptr == m_pMagneticSphere)
		return E_FAIL;
	m_pMagneticSphere->Set_Information(Engine::CTransform::INFO_POSITION, &_vec3(0.f, 0.f, 0.f));
	m_pMagneticSphere->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(15.f, 15.f, 15.f));
	dynamic_cast<CEffect_Mesh*>(m_pMagneticSphere)->Set_UV(_vec2(10.f, 10.f));
	m_pMagneticSphere->Set_RenderState(FALSE);
	m_pEffectMgr->Add_Effect(m_pMagneticSphere);

	return S_OK;
}

HRESULT CScene_Stage2::Ready_LayerForBackGround(void)
{
	Engine::CGameObject*      pGameObject = nullptr;

	// Map

	//SkyBoxBall
	pGameObject = CStage2_SkyBall::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE2, L"Layer_BackGround", pGameObject)))
		return E_FAIL;

	//SkyBox
	pGameObject = CStage2_SkyBox::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE2, L"Layer_BackGround", pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage2::Ready_LayerForGameObject(void)
{
	Engine::CGameObject*      pGameObject = nullptr;

	return NOERROR;
}

HRESULT CScene_Stage2::Ready_LayerForUI(void)
{
	Engine::CGameObject*      pGameObject = nullptr;

	//// Camera
	AddCamera(CGoblin_Camera, CAMERA_GOBLIN);
	AddCamera(CDynamicCamera, CAMERA_DYNAMIC);
	AddCamera(CRecordCamera, CAMERA_RECORDING);
	AddCamera(CActionCamera, CAMERA_ACTION);
	AddCamera(CFloatCamera, CAMERA_FLOAT);
	AddCamera(CVictory_Camera, CAMERA_VICTORY);

	pGameObject = CPlayer_Left::Create(m_pGraphicDev);
	CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE2, L"Layer_UI", pGameObject);

	pGameObject = CFont_FPS::Create(m_pGraphicDev);
	CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE2, L"Layer_UI", pGameObject);

	return NOERROR;
}

// g_eLastMan_Progress
void CScene_Stage2::Lastman_Progress(void)
{
	// 초반 대기중.. -> 서버에서 시작 message를 받으면 LASTMAN_ROUND1_WAIT으로 가자
	if (LASTMAN_WAIT == g_eLastMan_Progress)
	{
		m_eSCeneState = LASTMAN_WAIT;

		m_fRoundTime = 0.f;
	}

	// Round Wait (10~20초간 탐색전)
	else if (LASTMAN_ROUND_WAIT == g_eLastMan_Progress)
	{
		if (m_eSCeneState != LASTMAN_ROUND_WAIT)
		{
			m_eSCeneState = LASTMAN_ROUND_WAIT;

			//
			m_pCameraMgr->Change_Camera(CAMERA_GOBLIN, 0, 1.f);
			m_fRoundTime = 0.f; // 라운드 시간
		}
		
	}

	// Round start (본격적인 게임 시작)
	else if (LASTMAN_ROUND_START == g_eLastMan_Progress)
	{
		if (m_eSCeneState != LASTMAN_ROUND_START)
		{
			m_eSCeneState = LASTMAN_ROUND_START;

			//

		}
		
	}

	// finish (라운드 종료, 승자 발표)
	else if (LASTMAN_ROUND_FINISH == g_eLastMan_Progress)
	{
		if (m_eSCeneState != LASTMAN_ROUND_FINISH)
		{
			m_eSCeneState = LASTMAN_ROUND_FINISH;

			//

		}
		
	}

	// 강제 시작 치트키
//#ifdef Emperor
	if (CInputDev::GetInstance()->OnceKeyDown(DIK_O))
	{
		Engine::NETDATA tData;
		ZeroMemory(&tData, sizeof(Engine::NETDATA));
		tData.chKey = 72;
		tData.eMessage = NETMSG_CLINET_ROUND2_STATE;
		tData.wSubMessage = 0;

		CClientNetwork::GetInstance()->SendMsg(tData);

		g_iRound++;

		if(g_iRound == 1)
			CClientNetwork::GetInstance()->Create_Stage2Monster();
		else
			CClientNetwork::GetInstance()->Reset_Round();

		g_eLastMan_Progress = LASTMAN_ROUND_WAIT;
	}
//#endif
}

void CScene_Stage2::Lastman_Timer(const float& fTimeDelta)
{
	// 20초 뒤 게임이 시작됩니다.
	if ( (m_fRoundTime == 0.f) && (m_eSCeneState == LASTMAN_ROUND_WAIT) )
	{
		cout << "20초 뒤 게임이 시작됩니다." << endl;

		Engine::CGameObject* pGameObject = CText_GameStart::Create(m_pGraphicDev);
		m_pObjectMgr->Add_GameObject(SCENE_STAGE2, L"Layer_UI", pGameObject);

		m_pMagneticSphere->Set_InitMagnetic(g_iRound);
		m_pMagneticSphere->Set_RenderState(FALSE);
	}

	// Time Flow
	if (LASTMAN_WAIT != g_eLastMan_Progress)
		m_fRoundTime += fTimeDelta;

	if ((m_fRoundTime > 20.f) && (g_eLastMan_Progress == LASTMAN_ROUND_WAIT))
	{
		g_eLastMan_Progress = LASTMAN_ROUND_START;
	}

	// 곧 자기장이 활성화 됩니다.
	if ( (m_fRoundTime > 60.f) && (g_eLastMan_Progress == LASTMAN_ROUND_START) )
	{
		cout << "곧 자기장이 활성화 됩니다." << endl;

		g_eLastMan_Progress = LASTMAN_ROUND_ELECTRIC_READY;

		Engine::CGameObject* pGameObject = CText_Electric::Create(m_pGraphicDev);
		m_pObjectMgr->Add_GameObject(SCENE_STAGE2, L"Layer_UI", pGameObject);

		m_pMagneticSphere->Set_MagneticState(CEffect_Magnetic_Sphere::PREVIEW);
		m_pMagneticSphere->Set_RenderState(TRUE);
	}

	// 자기장 활성화
	if ((m_fRoundTime > 90.f) && (g_eLastMan_Progress == LASTMAN_ROUND_ELECTRIC_READY))
	{
		cout << "자기장 활성화" << endl;

		g_eLastMan_Progress = LASTMAN_ROUND_ELECTRIC_ACTIVATE;

		m_pMagneticSphere->Set_MagneticState(CEffect_Magnetic_Sphere::ACTIVE);
		m_pMagneticSphere->Set_RenderState(TRUE);
	}

	
}

HRESULT CScene_Stage2::Ready_Light(void)
{
	CLight_Manager::GetInstance()->Reset_Light(); // 기존 Light Reset

	D3DLIGHT9      LightInfo;
	// Directional
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

	LightInfo.Type = D3DLIGHT_DIRECTIONAL;
	LightInfo.Direction = _vec3(0.f, -0.57735f, 0.f);
	LightInfo.Diffuse = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.f);
	LightInfo.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	LightInfo.Specular = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.f);

	if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage2::Ready_NavMesh(void)
{
	DWORD dwByte = 0;

	HANDLE hFile = CreateFile(L"../Resources/Data/NavMesh/Data2.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	while (true)
	{
		Engine::NAVMESH NavMesh;
		ZeroMemory(&NavMesh, sizeof(Engine::NAVMESH));

		ReadFile(hFile, &NavMesh, sizeof(Engine::NAVMESH), &dwByte, NULL);

		if (dwByte == 0)
			break;

		Engine::CGameObject* pFontNumber = CFont_NavMesh::Create(m_pGraphicDev, NavMesh.iIndex);

		m_pNavMgr->AddCell(&NavMesh, pFontNumber);
	}
	CloseHandle(hFile);

	m_pNavMgr->LinkCell();

	return NOERROR;
}

HRESULT CScene_Stage2::Ready_MapObject(void)
{
	DWORD dwByte = 0;

	HANDLE hFile = CreateFile(L"../Resources/Data/Object/Stage2.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

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

HRESULT CScene_Stage2::Ready_RandomData(void)
{
	// Stage2
	vector< vector<int> >vecRamdomGroup;
	vector<int> vecTemp;
	DWORD dwByte = 0;
	
	HANDLE hFile = CreateFile(L"../Resources/Data/Wave/Random.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	while (true)
	{
		_int iRandom;

		ReadFile(hFile, &iRandom, sizeof(_int), &dwByte, NULL);

		if (dwByte == 0)
			break;

		if (iRandom == 40)
		{
			vecRamdomGroup.push_back(vecTemp);
			vecTemp.clear();
		}
		else
			vecTemp.push_back(iRandom);
	}
	CloseHandle(hFile);

	CDataManager::GetInstance()->Set_AI_Pattern(vecRamdomGroup);

	vector<_vec3> vecAIPos;
	HANDLE hFile2 = CreateFile(L"../Resources/Data/Wave/Stage2.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	while (true)
	{
		_vec3 vPos;

		ReadFile(hFile2, &vPos, sizeof(_vec3), &dwByte, NULL);

		if (dwByte == 0)
			break;

		vecAIPos.push_back(vPos);
	}
	CloseHandle(hFile2);

	CDataManager::GetInstance()->Set_AI_Position(vecAIPos);

	return NOERROR;
}

HRESULT CScene_Stage2::Add_MapObject(Engine::OBJECT& tObject)
{
	Engine::CGameObject* pGameObject = nullptr;

	switch (tObject.eId)
	{
	case Engine::OBJECT2_TRESUREBOX:
		pGameObject = CTresureBox::Create(m_pGraphicDev);
		break;
	case Engine::OBJECT2_CRYSTAL:
		pGameObject = CCrystal::Create(m_pGraphicDev);
		break;
	case Engine::OBJECT2_ACCESSORY1:
		pGameObject = CAccessory1::Create(m_pGraphicDev);
		break;
	case Engine::OBJECT2_ACCESSORY2:
		pGameObject = CAccessory2::Create(m_pGraphicDev);
		break;

	case Engine::OBJECT2_POINT_LIGHT0:
	case Engine::OBJECT2_POINT_LIGHT1:
	case Engine::OBJECT2_POINT_LIGHT2:
	case Engine::OBJECT2_POINT_LIGHT3:
	case Engine::OBJECT2_POINT_LIGHT4:
		CDataManager::GetInstance()->Add_PointLight(tObject.vecPos, tObject.eId, OBJECT2_POINT_LIGHT0);
		//pGameObject = CMapTexture::Create(m_pGraphicDev, tObject.eId);
		break;

		// 리본
	case Engine::OBJECT2_END + 77:
	case Engine::OBJECT2_END + 78:
	case Engine::OBJECT2_END + 79:
	case Engine::OBJECT2_END + 80:
	case Engine::OBJECT2_END + 81:
	case Engine::OBJECT2_END + 82:
	case Engine::OBJECT2_END + 83:
	case Engine::OBJECT2_END + 84:
		pGameObject = CRibbon::Create(m_pGraphicDev, (_int)tObject.eId);
		break;

	default: 
		pGameObject = CBlockGroup::Create(m_pGraphicDev, (_int)tObject.eId);
		break;
	}

	if (nullptr == pGameObject)
		return E_FAIL;

	Engine::CTransform* pTransform = (Engine::CTransform*)pGameObject->Get_Component(Engine::CComponent::COM_DYNAMIC, L"Com_Transform");
	pTransform->Set_Information(CTransform::INFO_POSITION, &tObject.vecPos);
	pTransform->Set_Information(CTransform::INFO_ANGLE, (_vec3*)(tObject.fAngle));
	pTransform->Set_Information(CTransform::INFO_SCALE, &tObject.vecScale);

	m_pObjectMgr->Add_GameObject(SCENE_STAGE2, L"Layer_GameObject", pGameObject);

	return NOERROR;
}

CScene_Stage2 * CScene_Stage2::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CScene_Stage2* pInstance = new CScene_Stage2(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CScene_Stage2 Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CScene_Stage2::Free(void)
{
	m_pEffectMgr->Release_Effect();
	m_pClinetNetwork->Stage_Clear2();
	m_pCameraMgr->Reset_Camera();
	m_pNavMgr->Reset_Cell();
	CDataManager::GetInstance()->Reset_Data();

	m_pInfoSubject->Remove_Data(MESSAGE_SCENECHANGE, &m_eSceneId);
	m_pInfoSubject->Remove_Data(MESSAGE_LASTMAN_COMPUTER_ADD, &m_eSceneId);
	m_pInfoSubject->Remove_Data(MESSAGE_LASTMAN_COMPUTER_DEAD, &m_eSceneId);
	m_pInfoSubject->Remove_Data(MESSAGE_LASTMAN_PLAYER_ADD, &m_eSceneId);
	m_pInfoSubject->Remove_Data(MESSAGE_LASTMAN_PLAYER_DEAD, &m_eSceneId);
	Engine::Safe_Release(m_pInfoSubject);
	Engine::Safe_Release(m_pClinetNetwork);
	Engine::Safe_Release(m_pNavMgr);
	Engine::Safe_Release(m_pCameraMgr);
	Engine::Safe_Release(m_pEffectMgr);

	Engine::CScene::Free();
}