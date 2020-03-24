#include "stdafx.h"
#include "ActionCamera.h"
#include "InputDev.h"
#include "Camera_Manager.h"
#include "Text_Title.h"
#include "Object_Manager.h"
#include "InfoSubject.h"
#include "Scene.h"
#include "Scene_Transition.h"
#include "Loading.h"
#include "Management.h"
#include "InfoSubject.h"
#include "SoundMgr.h"

CActionCamera::CActionCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
	, m_hFile(INVALID_HANDLE_VALUE)
	, m_eNextCamType(CAMERA_END)
	, m_eCutSceneType(CUTSCENE_END)
	, m_eSceneId(SCENE_END)
{
}


CActionCamera::~CActionCamera(void)
{
}

HRESULT CActionCamera::Ready_GameObject()
{
	m_vEye = _vec3(0.0f, 10.f, -20.f);
	m_vAt = _vec3(0.f, 0.f, 0.f);
	m_vUp = _vec3(0.f, 1.f, 0.f);

	m_fFovy = D3DXToRadian(60.f);
	m_fAspect = float(WINCX) / WINCY;
	m_fNear = PLANE_NEAR;
	m_fFar = PLANE_FAR;

	Engine::CCamera::Ready_GameObject();

	CInfoSubject::GetInstance()->Add_Data(MESSACE_CUTSCENE, &m_bShowCutScene);
	CInfoSubject::GetInstance()->Add_Data(MESSAGE_SCENECHANGE, &m_eSceneId);

	return S_OK;
}

void CActionCamera::Change_Camera(_uint uNum, float fAttachTime)
{
	m_eCutSceneType = (CUTSCENE_TYPE)uNum;
	Set_CutScene();
}

int CActionCamera::Update_GameObject(const float & fTimeDelta)
{
	Show_CutScene();

	if (CInputDev::GetInstance()->StayKeyDown(DIK_SPACE))
		m_bShowCutScene = false;
		
	return Engine::CCamera::Update_GameObject(fTimeDelta);
}

void CActionCamera::Set_CutScene()
{
	m_bShowCutScene = true;
	CInfoSubject::GetInstance()->Notify_Message(MESSACE_CUTSCENE);

	switch (m_eCutSceneType)
	{
	case CUTSCENE_MENU: // 맨 처음 Menu
		m_hFile = CreateFile(L"../Resources/Data/CamData/Menu.dat", GENERIC_READ, NULL,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		m_eNextCamType = CAMERA_STAND;
		break;

	case CUTSCENE_MENU2: // Menu -> Lobby
		m_hFile = CreateFile(L"../Resources/Data/CamData/Menu2.dat", GENERIC_READ, NULL,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		m_eNextCamType = CAMERA_DYNAMIC;

		m_eSceneId = SCENE_MENU_TO_STAGE; // title이랑 ui 사라지게 하려고..
		CInfoSubject::GetInstance()->Notify_Message(MESSAGE_SCENECHANGE);

		CSoundMgr::GetInstance()->Play_Sound(L"StartGame2D.ogg", Engine::CHANNEL_GAMESTART);
		break;

	case CUTSCENE_STAGE: // 맨 처음 Stage
		m_hFile = CreateFile(L"../Resources/Data/CamData/Stage.dat", GENERIC_READ, NULL,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		m_eNextCamType = CAMERA_STATIC;

		CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", CText_Title::Create(m_pGraphicDev, 0, 3.f, 6.f));
		m_fAttachTime = 1.f;
		break;

	case CUTSCENE_BOSS:
		m_hFile = CreateFile(L"../Resources/Data/CamData/Boss.dat", GENERIC_READ, NULL,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		m_eNextCamType = CAMERA_STATIC;
		m_fAttachTime = 1.f;
		break;

	case CUTSCENE_LOBBY:
		m_hFile = CreateFile(L"../Resources/Data/CamData/Lobby.dat", GENERIC_READ, NULL,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		m_eNextCamType = CAMERA_STATIC;

		CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", CText_Title::Create(m_pGraphicDev, 2, 10.f, 13.f));
		m_fAttachTime = 1.f;
		break;

	case CUTSCENE_STAGE_2:
		m_hFile = CreateFile(L"../Resources/Data/CamData/Stage_2.dat", GENERIC_READ, NULL,
			NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		m_eNextCamType = CAMERA_FLOAT;

		CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", CText_Title::Create(m_pGraphicDev, 1, 7.f, 10.f));
		break;

	default:
		cout << "CutScene Default" << endl;
		break;
	}
}

void CActionCamera::Show_CutScene(void)
{
	if (FALSE == m_bShowCutScene) // 컷 씬 종료될 때
	{
		CInfoSubject::GetInstance()->Notify_Message(MESSACE_CUTSCENE);
		CCamera_Manager::GetInstance()->Change_Camera(m_eNextCamType, 0, m_fAttachTime);
		if (INVALID_HANDLE_VALUE != m_hFile)
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}

		if (CUTSCENE_MENU2 == m_eCutSceneType) // 메뉴2 컷-씬이 종료되면 Stage로 넘어간다
		{
			m_eSceneId = SCENE_LOBBY;
			CInfoSubject::GetInstance()->Notify_Message(MESSAGE_SCENECHANGE);
		}

		if (CUTSCENE_LOBBY == m_eCutSceneType || CUTSCENE_STAGE == m_eCutSceneType)
		{
			// 종료되면 여기
			g_bTextShow = true;
		}
			
		return;
	}
		
	DWORD dwByte = 0;

	CAM_INFO* pCamInfo = new CAM_INFO;

	ReadFile(m_hFile, pCamInfo, sizeof(CAM_INFO), &dwByte, NULL);

	if (dwByte == 0) // 씬 종료
	{
		Engine::Safe_Delete(pCamInfo);
		m_bShowCutScene = false;

		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
		return;
	}

	m_vEye = pCamInfo->vecEye;
	m_vAt = pCamInfo->vecAt;

	Engine::Safe_Delete(pCamInfo);
}

CActionCamera* CActionCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CActionCamera* pCamera = new CActionCamera(pGraphicDev);
	if (FAILED(pCamera->Ready_GameObject()))
	{
		MSG_BOX(L"CActionCamera Create Failed - CActionCamera::Create()");
		Engine::Safe_Release(pCamera);
	}

	return pCamera;
}

void CActionCamera::Free(void)
{
	if (INVALID_HANDLE_VALUE != m_hFile)
	{
		CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	CInfoSubject::GetInstance()->Remove_Data(MESSACE_CUTSCENE, &m_bShowCutScene);
	CInfoSubject::GetInstance()->Remove_Data(MESSAGE_SCENECHANGE, &m_eSceneId);
		
	Engine::CCamera::Free();
}
