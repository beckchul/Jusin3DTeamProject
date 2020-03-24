#include "stdafx.h"
#include "Scene_Transition.h"
#include "Management.h"
#include "Scene_Stage.h"
#include "Component_Manager.h"
#include "ClientNetwork.h"
#include "InputDev.h"
#include "NavMgr.h"
#include "Camera_Manager.h"
#include "EffectMgr.h"
#include "Light_Manager.h"
#include "DataManager.h"
#include "FrustumManager.h"
#include "CollisionManager.h"
#include "EffectMgr.h"
#include "Light_Manager.h"
#include "Mouse_Manager.h"
#include "SoundMgr.h"

// BackGround
#include "MagusQuarters.h"
#include "Player_Spear.h"
#include "Player_Staff.h"
#include "Crystal.h"
#include "Accessory1.h"
#include "Accessory2.h"
#include "CrystalLight.h"
#include "TresureBox.h"
#include "LeftDoor.h"
#include "RightDoor.h"
#include "3DMaxObject.h"

// GameObject
#include "Player_Mage.h"
#include "Player_Monk.h"

#include "Player_Spear.h"
#include "Player_Staff.h"

#include "Monster_Goblin.h"
#include "Monster_OrcBruiser.h"
#include "Monster_Kobold.h"
#include "Monster_DarkElfArcher.h"
#include "Monster_Demon.h"

// Camera
#include "StaticCamera.h"
#include "RecordCamera.h"
#include "DynamicCamera.h"
#include "TowerCamera.h"
#include "ActionCamera.h"
#include "SummonCamera.h"

// UI
#include "Font_NavMesh.h"
#include "Font_ActivateCrystal.h"
#include "MapTexture.h"

#include "StaticUI.h"

// Effect
#include "Effect_Particle.h"
#include "Effect_RectParticle.h"
#include "Effect_Frame.h"
#include "Effect_Single.h"
#include "Effect_Single_Fog.h"

///////// Effect_Mesh
#include "Effect_Mesh_Shield.h"
#include "Effect_Mesh_Bolt.h"
#include "Effect_Mesh_ElectricGroup1.h"
#include "Effect_Mesh_ElectricGroup2.h"
#include "Effect_Mesh_LightningTree.h"
#include "Effect_Mesh_Wave.h"
#include "Effect_Mesh_AuraSphere.h"
#include "Effect_Mesh_Spiral.h"
#include "Effect_Mesh_ManaBomb.h"
#include "Effect_Mesh_SplosionRings.h"
#include "Effect_Mesh_Swirly.h"
#include "Effect_Mesh_GlowBall.h"
#include "Effect_Mesh_Projectile1.h"

///// Observer
#include "WaveObserver.h"
#include "InfoSubject.h"

///// CollisionMgr
#include "CollisionManager.h"

CScene_Stage::CScene_Stage(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
	, m_pClinetNetwork(CClientNetwork::GetInstance())
	, m_pNavMgr(CNavMgr::GetInstance())
	, m_pCameraMgr(CCamera_Manager::GetInstance())
	, m_pEffectMgr(CEffectMgr::GetInstance())
	, m_pInfoSubject(Engine::CInfoSubject::GetInstance())
	, m_eSceneId(SCENE_STAGE)
	, m_vecFadeColor(1.f, 1.f, 1.f, 1.f)
{
	m_pClinetNetwork->Add_Ref();
	m_pNavMgr->Add_Ref();
	m_pCameraMgr->Add_Ref();
	m_pEffectMgr->Add_Ref();
	m_pInfoSubject->Add_Ref();
}


CScene_Stage::~CScene_Stage()
{
}

HRESULT CScene_Stage::Ready_Scene(void)
{
	CDataManager::GetInstance()->Set_FadeColor(_vec4(1.f, 1.f, 1.f, 1.f));
	CDataManager::GetInstance()->Set_BackColor(_vec4(0.f, 0.f, 0.f, 0.f));

	CSoundMgr::GetInstance()->StopSound(CHANNEL_MENU);
	CSoundMgr::GetInstance()->PlayBGM(L"DungeonDefense_MainTheme.ogg", Engine::CHANNEL_STAGE, 0.3f);
	//CSoundMgr::GetInstance()->PlayBGM(L"10Biotonic.mp3", Engine::CHANNEL_STAGE);
	g_eCurrentScene = SCENE_STAGE;
	CDataManager::GetInstance()->Set_Scene_ID(3);

	if (FAILED(Ready_NavMesh()))
		return E_FAIL;
	if (FAILED(Ready_Effect()))
		return E_FAIL;
	if (FAILED(Ready_LayerForUI()))
		return E_FAIL;
	if (FAILED(Ready_LayerForGameObject()))
		return E_FAIL;
	if (FAILED(Ready_LayerForBackGround()))
		return E_FAIL;
	if (FAILED(Ready_MapObject()))
		return E_FAIL;
	if (FAILED(Ready_Light())) // MapObject 아래
		return E_FAIL;
	if (FAILED(Ready_Wave()))
		return E_FAIL;
	CCollisionManager::GetInstance()->Initialize_CollisionMgr(); // NavMesh 정보 저장

	// 서버 <-> Scene 세팅 (플레이어, 채팅 등)
	m_pClinetNetwork->SetCurrentScene(SCENE_STAGE);

	Engine::CInfoSubject::GetInstance()->Add_Data(MESSAGE_SCENECHANGE, &m_eSceneId);

	m_pCameraMgr->Change_Camera(CAMERA_STATIC);

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

int CScene_Stage::Update_Scene(const float & fTimeDelta)
{
	if (g_bTextShow)
	{
		// 점점 어두워짐
		if (!m_bColorAdd)
		{
			if (m_vecFadeColor.x >= 0.f)
			{
				m_vecFadeColor -= _vec4(1.f, 1.f, 1.f, 1.f) * fTimeDelta * 1.5f;
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

	m_pCameraMgr->Update_Camara(fTimeDelta); // 카메라

	Engine::CScene::Update_Scene(fTimeDelta);

#ifndef Demonstration
	m_pNavMgr->Update_NavMesh(fTimeDelta); // 내비메쉬(폰트)
#endif
	m_pEffectMgr->Update_Effect(fTimeDelta); // 이펙트

	if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_WAVE))
		CSoundMgr::GetInstance()->Set_BGMVolume(Engine::CHANNEL_STAGE, 0.3f);

	MonsterLogic(fTimeDelta);

	return 0;
}

void CScene_Stage::Render_Scene(void)
{
	m_pNavMgr->Render_NavMesh(); // 내비메쉬(Cell)
	CCollisionManager::GetInstance()->Render_CollisionSphere();

	// Stage2 가자
	if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_END) || g_eCurrentScene == SCENE_STAGE2)
	{
		Engine::CScene*		pNewScene = CScene_Transition::Create(m_pGraphicDev, CLoading::LOADING_STAGE2);
		if (nullptr == pNewScene)
			return;
		if (FAILED(Engine::CManagement::GetInstance()->SetUp_CurrentScene(pNewScene)))
			return;
	}
}

void CScene_Stage::FirstRender_Finish(void)
{
	m_pCameraMgr->Change_Camera(CAMERA_ACTION, CUTSCENE_STAGE);
}

HRESULT CScene_Stage::Ready_Effect(void)
{	
	// 예전엔 이펙트 리스트를 불러오는 곳이었는데 ,
	// 지금은 맵 이펙트 띄우는곳으로 쓰장
	//Effect----------------------------------------------------------------------------------------------------
	Engine::CEffect* pEffect = nullptr;

	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Lava_Bubble")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_BubbleState(0.5f, 10.f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_SpreadExceptY();
	pEffect->Set_OriginPos(_vec3(-143.f, -123.f, -136.f));
	m_pEffectMgr->Add_Effect(pEffect);
	m_pEffectMgr->Add_Effect(L"Effect_Lava_Smoke1", &_vec3(-166.f, -123.f, -136.f));
	m_pEffectMgr->Add_Effect(L"Effect_Lava_Smoke2", &_vec3(-113.f, -123.f, -165.f));
	//m_pEffectMgr->Add_Effect(L"Effect_Lava_Smoke", &_vec3(-140.f, -123.f, -144.f));


	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Lava_Bubble")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_BubbleState(0.5f, 10.f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_SpreadExceptY();
	pEffect->Set_OriginPos(_vec3(-128.f, -123.f, 146.f));
	m_pEffectMgr->Add_Effect(pEffect);
	m_pEffectMgr->Add_Effect(L"Effect_Lava_Smoke1", &_vec3(-188.867f, -123.02f, 117.523f));
	m_pEffectMgr->Add_Effect(L"Effect_Lava_Smoke2", &_vec3(-124.224f, -123.036f, 157.457f));


	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Small_Lava_Bubble")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_BubbleState(0.38f, 9.f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_SpreadExceptY();
	pEffect->Set_OriginPos(_vec3(43.f, -27.f, 46.f));
	m_pEffectMgr->Add_Effect(pEffect);
	m_pEffectMgr->Add_Effect(L"Effect_SmallLava_Smoke", &_vec3(43.f, -27.f, 46.f));


	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Small_Lava_Bubble")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_BubbleState(0.38f, 9.f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_SpreadExceptY();
	pEffect->Set_OriginPos(_vec3(52.f, -27.f, -48.f));
	m_pEffectMgr->Add_Effect(pEffect);
	m_pEffectMgr->Add_Effect(L"Effect_SmallLava_Smoke", &_vec3(52.f, -27.f, -48.f));


	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Small_Lava_Bubble")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_BubbleState(0.38f, 9.f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_SpreadExceptY();
	pEffect->Set_OriginPos(_vec3(-47.f, -26.f, 49.f));
	m_pEffectMgr->Add_Effect(pEffect);
	m_pEffectMgr->Add_Effect(L"Effect_SmallLava_Smoke", &_vec3(-47.f, -26.f, 49.f));


	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Small_Lava_Bubble")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_BubbleState(0.38f, 9.f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_SpreadExceptY();
	pEffect->Set_OriginPos(_vec3(-44.f, -26.f, -45.f));
	m_pEffectMgr->Add_Effect(pEffect);
	m_pEffectMgr->Add_Effect(L"Effect_SmallLava_Smoke", &_vec3(-44.f, -26.f, -45.f));



	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_PurpleStars")), SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Particle*>(pEffect)->Set_OriginPos(_vec3(0.f, 18.f, 0.f));
	m_pEffectMgr->Add_Effect(pEffect);

	m_pEffectMgr->Add_Effect(L"Effect_Crystal_Radial", &_vec3(0.f, 30.f, 0.f));


	// 화로들
	m_pEffectMgr->Add_Effect(L"Effect_brazier", &_vec3(147.f, -48.f, 298.f));
	m_pEffectMgr->Add_Effect(L"Effect_brazier", &_vec3(145.f, -48.f, -298.f));
	m_pEffectMgr->Add_Effect(L"Effect_Rightbrazier", &_vec3(-272.f, -85.f, -505.f));
	m_pEffectMgr->Add_Effect(L"Effect_Leftbrazier", &_vec3(-273.f, -85.f, 505.f));

	// 마법진
	m_pEffectMgr->Add_Effect(L"Effect_PinkMagicCircle", &_vec3(-39.f, -63.f, 459.f));
	m_pEffectMgr->Add_Effect(L"Effect_PinkMagicCircle", &_vec3(64.f, -63.f, 459.f));

	m_pEffectMgr->Add_Effect(L"Effect_PinkMagicCircle", &_vec3(59.f, -63.f, -459.f));
	m_pEffectMgr->Add_Effect(L"Effect_PinkMagicCircle", &_vec3(-40.f, -63.f, -459.f));
	
	m_pEffectMgr->Add_Effect(L"Effect_BlueMagicCircle", &_vec3(-477.f, -63.f, 78.f));
	m_pEffectMgr->Add_Effect(L"Effect_BlueMagicCircle", &_vec3(-477.f, -63.f, -85.f));

	m_pEffectMgr->Add_Effect(L"Effect_RedMagicCircle", &_vec3(-385.f, -115.f, 352.f));
	m_pEffectMgr->Add_Effect(L"Effect_RedMagicCircle", &_vec3(-385.f, -115.f, -352.f));

	//---------------------------------------------------------------------------------------------------------


	return S_OK;
}

HRESULT CScene_Stage::Ready_LayerForBackGround(void)
{
	Engine::CGameObject*      pGameObject = nullptr;

	// Map
	AddGameObject(CMagusQuarters, SCENE_STAGE, L"Layer_BackGround");

	return NOERROR;
}

HRESULT CScene_Stage::Ready_LayerForGameObject(void)
{
	Engine::CGameObject*      pGameObject = nullptr;

	/*pGameObject = CMonster_Demon::Create(m_pGraphicDev, _vec3(-412.f, -60.f, 0.f));
	if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	return E_FAIL;*/

	return NOERROR;
}

HRESULT CScene_Stage::Ready_LayerForUI(void)
{
	Engine::CGameObject*      pGameObject = nullptr;

	//// Camera
	AddCamera(CStaticCamera, CAMERA_STATIC);
	AddCamera(CDynamicCamera, CAMERA_DYNAMIC);
	AddCamera(CRecordCamera, CAMERA_RECORDING);
	AddCamera(CActionCamera, CAMERA_ACTION);
	AddCamera(CTowerCamera, CAMERA_TOWER);
	AddCamera(CSummonCamera, CAMERA_SUMMON);

	AddGameObject(CStaticUI, SCENE_STAGE, L"Layer_UI"); // 기타 UI들

	AddGameObject(CFont_ActivateCrystal, SCENE_STAGE, L"Layer_UI");
	return NOERROR;
}

HRESULT CScene_Stage::Ready_Light(void)
{
	CLight_Manager::GetInstance()->Reset_Light(); // 기존 Light Reset
	D3DLIGHT9      LightInfo;

#ifndef BadComputer
	// Light0 (벽)
	vector<_vec3> vecLightPos0 = Engine::CDataManager::GetInstance()->Get_LightPosition(Engine::OBJECT_POINT_LIGHT0);
	CLight_Manager::GetInstance()->Reset_Light(); // 기존 Light Reset

	for (size_t i = 0; i < vecLightPos0.size(); ++i)
	{
		ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

		LightInfo.Type = D3DLIGHT_POINT;
		LightInfo.Position = vecLightPos0[i];
		LightInfo.Range = 60.f;
		LightInfo.Diffuse = D3DXCOLOR(1.f, 0.45f, 0.f, 8.4f);
		LightInfo.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
		LightInfo.Specular = D3DXCOLOR(1.f, 0.45f, 0.f, 1.f);

		if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
			return E_FAIL;


		// Effect_Torch
		// 걍 빨간색 
		Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Torch", &_vec3(vecLightPos0[i].x, vecLightPos0[i].y + 2.5f, vecLightPos0[i].z));
	}

	// Light1
	vector<_vec3> vecLightPos1 = Engine::CDataManager::GetInstance()->Get_LightPosition(Engine::OBJECT_POINT_LIGHT1);

	for (size_t i = 0; i < vecLightPos1.size(); ++i)
	{
		ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

		LightInfo.Type = D3DLIGHT_POINT;
		LightInfo.Position = vecLightPos1[i] + _vec3(0.f, 10.f, 0.f);
		LightInfo.Range = 35.f;
		LightInfo.Diffuse = D3DXCOLOR(0.f, 0.5f, 1.f, 8.4f);
		LightInfo.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
		LightInfo.Specular = D3DXCOLOR(0.f, 0.5f, 1.f, 1.f);

		if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
			return E_FAIL;

		// Effect_Torch
		m_pEffectMgr->Add_Effect(L"Effect_BlueTorch", &_vec3(vecLightPos1[i].x, vecLightPos1[i].y, vecLightPos1[i].z));
	}

	// Light2
	vector<_vec3> vecLightPos2 = Engine::CDataManager::GetInstance()->Get_LightPosition(Engine::OBJECT_POINT_LIGHT2);

	for (size_t i = 0; i < vecLightPos2.size(); ++i)
	{
		ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

		LightInfo.Type = D3DLIGHT_POINT;
		LightInfo.Position = vecLightPos2[i] + _vec3(0.f, 10.f, 0.f);
		LightInfo.Range = 35.f;
		switch (i)
		{
		case 0:
			LightInfo.Diffuse = D3DXCOLOR(1.f, 0.89f, 0.f, 8.4f); // 노
			LightInfo.Specular = D3DXCOLOR(1.f, 0.89f, 0.f, 1.f);
			m_pEffectMgr->Add_Effect(L"Effect_YellowTorch", &_vec3(vecLightPos2[i].x, vecLightPos2[i].y, vecLightPos2[i].z));
			break;

		case 1:
			LightInfo.Diffuse = D3DXCOLOR(0.12f, 1.f, 0.1f, 8.4f); // 초
			LightInfo.Specular = D3DXCOLOR(0.12f, 1.f, 0.1f, 1.f);
			m_pEffectMgr->Add_Effect(L"Effect_GreenTorch", &_vec3(vecLightPos2[i].x, vecLightPos2[i].y, vecLightPos2[i].z));
			break;

		case 2:
			LightInfo.Diffuse = D3DXCOLOR(0.95f, 0.38f, 0.65f, 8.4f); // 보
			LightInfo.Specular = D3DXCOLOR(0.95f, 0.38f, 0.65f, 1.f);
			m_pEffectMgr->Add_Effect(L"Effect_PinkTorch", &_vec3(vecLightPos2[i].x, vecLightPos2[i].y, vecLightPos2[i].z));
			break;
		}
		LightInfo.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);

		if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
			return E_FAIL;
	}

	// Light3
	vector<_vec3> vecLightPos3 = Engine::CDataManager::GetInstance()->Get_LightPosition(Engine::OBJECT_POINT_LIGHT3);

	for (size_t i = 0; i < vecLightPos3.size(); ++i)
	{
		ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

		LightInfo.Type = D3DLIGHT_POINT;
		LightInfo.Position = vecLightPos3[i];
		LightInfo.Range = 35.f;
		LightInfo.Diffuse = D3DXCOLOR(1.f, 0.45f, 0.f, 8.4f);
		LightInfo.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
		LightInfo.Specular = D3DXCOLOR(1.f, 0.45f, 0.f, 1.f);

		if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
			return E_FAIL;
	}

	// Light4
	vector<_vec3> vecLightPos4 = Engine::CDataManager::GetInstance()->Get_LightPosition(Engine::OBJECT_POINT_LIGHT4);

	for (size_t i = 0; i < vecLightPos4.size(); ++i)
	{
		ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

		LightInfo.Type = D3DLIGHT_POINT;
		LightInfo.Position = vecLightPos4[i];
		if (i < 4)
			LightInfo.Range = 35.f;
		else
			LightInfo.Range = 150.f;

		LightInfo.Diffuse = D3DXCOLOR(1.f, 0.45f, 0.f, 4.f);
		LightInfo.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
		LightInfo.Specular = D3DXCOLOR(1.f, 0.45f, 0.f, 1.f);

		if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
			return E_FAIL;

	}

	// Light5 (빛장판 파랑색) 0.26, 0.515, 0.85
	vector<_vec3> vecLightPos5 = Engine::CDataManager::GetInstance()->Get_LightPosition(Engine::OBJECT_POINT_LIGHT5);

	for (size_t i = 0; i < vecLightPos5.size(); ++i)
	{
		ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

		LightInfo.Type = D3DLIGHT_POINT;
		LightInfo.Position = vecLightPos5[i] + _vec3(0.f, 10.f, 0.f);
		LightInfo.Range = 50.f;
		LightInfo.Diffuse = D3DXCOLOR(0.26f, 0.515f, 0.85f, 3.f);
		LightInfo.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
		LightInfo.Specular = D3DXCOLOR(0.26f, 0.515f, 0.85f, 1.f);

		if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
			return E_FAIL;

	}

	// 크리스탈 보라색
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

	LightInfo.Type = D3DLIGHT_POINT;
	LightInfo.Position = _vec3(1.f, 100.f, 1.f);
	LightInfo.Range = 150.f;
	LightInfo.Diffuse = D3DXCOLOR(0.64f, 0.4f, 1.0f, 8.4f);
	LightInfo.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
	LightInfo.Diffuse = D3DXCOLOR(0.64f, 0.4f, 1.0f, 1.f);

	if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
		return E_FAIL;
#endif

	// Directional
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

	LightInfo.Type = D3DLIGHT_DIRECTIONAL;
	LightInfo.Direction = _vec3(0.f, -0.57735f, 0.f);
	LightInfo.Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);
	LightInfo.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	LightInfo.Specular = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);

	if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CScene_Stage::Ready_NavMesh(void)
{
	DWORD dwByte = 0;

	HANDLE hFile = CreateFile(L"../Resources/Data/NavMesh/Data.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

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

HRESULT CScene_Stage::Ready_MapObject(void)
{
	DWORD dwByte = 0;

	HANDLE hFile = CreateFile(L"../Resources/Data/Object/Data.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	int iTemp = 0;
	while (true)
	{
		Engine::OBJECT Object;
		ZeroMemory(&Object, sizeof(Engine::OBJECT));

		ReadFile(hFile, &Object, sizeof(Engine::OBJECT), &dwByte, NULL);

		if (dwByte == 0)
			break;

		// Add Object
		Add_MapObject(Object, iTemp);

	}
	CloseHandle(hFile);

	return NOERROR;
}

HRESULT CScene_Stage::Add_MapObject(Engine::OBJECT& tObject, int& iTemp)
{
	Engine::CGameObject* pGameObject = nullptr;

	switch (tObject.eId)
	{
	case Engine::OBJECT_TRESUREBOX:
		pGameObject = CTresureBox::Create(m_pGraphicDev);
		break;
	case Engine::OBJECT_LEFT_DOOR:
		pGameObject = CLeftDoor::Create(m_pGraphicDev);
		break;
	case Engine::OBJECT_RIGHT_DOOR:
		pGameObject = CRightDoor::Create(m_pGraphicDev);
		break;
	case Engine::OBJECT_CRYSTAL:
		pGameObject = CCrystal::Create(m_pGraphicDev);
		break;
	case Engine::OBJECT_ACCESSORY1:
		pGameObject = CAccessory1::Create(m_pGraphicDev);
		break;
	case Engine::OBJECT_ACCESSORY2:
		pGameObject = CAccessory2::Create(m_pGraphicDev);
		break;
	case 33:
		pGameObject = CCrystalLight::Create(m_pGraphicDev);
		iTemp++;
		switch (iTemp)
		{
		case 12:
			dynamic_cast<CCrystalLight*>(pGameObject)->Set_Color(D3DXCOLOR(0.95f, 0.38f, 0.65f, 0.f)); // 보
			break;
		case 19:
			dynamic_cast<CCrystalLight*>(pGameObject)->Set_Color(D3DXCOLOR(0.12f, 1.f, 0.1f, 0.f)); // 초
			break;
		case 30:
			dynamic_cast<CCrystalLight*>(pGameObject)->Set_Color(D3DXCOLOR(1.f, 0.89f, 0.f, 0.f)); // 노
			break;
		}
		break;

	case Engine::OBJECT_MONSTER_ROUTE0:
	case Engine::OBJECT_MONSTER_ROUTE1:
	case Engine::OBJECT_MONSTER_ROUTE2:
	case Engine::OBJECT_MONSTER_ROUTE3:
	case Engine::OBJECT_MONSTER_ROUTE4:
	case Engine::OBJECT_MONSTER_ROUTE5:
	case Engine::OBJECT_MONSTER_ROUTE6:
	case Engine::OBJECT_MONSTER_ROUTE7:
		CDataManager::GetInstance()->Add_MonsterRoute(tObject.vecPos, tObject.eId);
		//pGameObject = CMapTexture::Create(m_pGraphicDev, tObject.eId);
		break;
	case Engine::OBJECT_POINT_LIGHT0:
	case Engine::OBJECT_POINT_LIGHT1:
	case Engine::OBJECT_POINT_LIGHT2:
	case Engine::OBJECT_POINT_LIGHT3:
	case Engine::OBJECT_POINT_LIGHT4:
	case Engine::OBJECT_POINT_LIGHT5:
		CDataManager::GetInstance()->Add_PointLight(tObject.vecPos, tObject.eId);
		//pGameObject = CMapTexture::Create(m_pGraphicDev, tObject.eId);
		break;

	default: // 3DMax맵
		pGameObject = C3DMaxObject::Create(m_pGraphicDev, (_int)tObject.eId);
		break;
	}

	if (nullptr == pGameObject)
		return E_FAIL;

	Engine::CTransform* pTransform = (Engine::CTransform*)pGameObject->Get_Component(Engine::CComponent::COM_DYNAMIC, L"Com_Transform");
	pTransform->Set_Information(CTransform::INFO_POSITION, &tObject.vecPos);
	pTransform->Set_Information(CTransform::INFO_ANGLE, (_vec3*)(tObject.fAngle));
	pTransform->Set_Information(CTransform::INFO_SCALE, &tObject.vecScale);

	m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject);

	return NOERROR;
}

HRESULT CScene_Stage::Ready_Wave(void)
{
	Engine::CDataManager::GetInstance()->Load_WaveCreateInfo();

	m_pWaveObserver = CWaveObserver::Create(m_pGraphicDev);
	m_pInfoSubject->Subscribe_Observer(m_pWaveObserver);

	m_tWaveInfo = m_pWaveObserver->Get_WaveInfo();
	m_tWaveInfo.ePhaseType = PHASE_END;

	m_pInfoSubject->Add_Data(MESSAGE_MINUSCOUNT, &m_tWaveInfo);
	m_pInfoSubject->Add_Data(MESSAGE_PLUSCOUNT, &m_tWaveInfo);
	m_pInfoSubject->Add_Data(MESSAGE_BUILD_PHASECHANGE, &m_tWaveInfo);

	vector<WAVECREATE_INFO*> vecStageWave = Engine::CDataManager::GetInstance()->Get_StageWave(0);
	m_tWaveInfo.iTotalUnits = vecStageWave.front()->iTotalMonsterCount;
	m_pInfoSubject->Add_Data(MESSAGE_MAX_UNITCOUNT, &m_tWaveInfo.iTotalUnits);

	return NOERROR;
}

void CScene_Stage::MonsterLogic(const float& fTimeDelta)
{
	if (m_tWaveInfo.ePhaseType != m_pWaveObserver->Get_WaveInfo().ePhaseType)
	{
		m_tWaveInfo.ePhaseType = m_pWaveObserver->Get_WaveInfo().ePhaseType;
		m_fWaveTime = 0.f;
		m_fCreateTime = 0.f;
		switch (m_tWaveInfo.iCurrentWave)
		{
		case 2:
			{
			vector<WAVECREATE_INFO*> vecStageWave = Engine::CDataManager::GetInstance()->Get_StageWave(1);
			m_tWaveInfo.iTotalUnits = vecStageWave.front()->iTotalMonsterCount;
			m_pInfoSubject->Notify_Message(MESSAGE_MAX_UNITCOUNT);
			break;
			}
		case 3:
			{
			vector<WAVECREATE_INFO*> vecStageWave = Engine::CDataManager::GetInstance()->Get_StageWave(2);
			m_tWaveInfo.iTotalUnits = vecStageWave.front()->iTotalMonsterCount;
			m_pInfoSubject->Notify_Message(MESSAGE_MAX_UNITCOUNT);
			break;
			}
		}

		if (COMBAT_PHASE == m_tWaveInfo.ePhaseType)
		{
			for (int i = 0; i < m_tWaveInfo.iTotalUnits; ++i)
				m_pInfoSubject->Notify_Message(MESSAGE_PLUSCOUNT);
			m_pInfoSubject->Notify_Message(MESSAGE_MAX_UNITCOUNT);

			CSoundMgr::GetInstance()->Set_BGMVolume(Engine::CHANNEL_STAGE, 0.1f);
			if (FALSE == CSoundMgr::GetInstance()->IsPlaying(CHANNEL_WAVE))
				CSoundMgr::GetInstance()->Play_Sound(L"DD_Wave1Beat.ogg", Engine::CHANNEL_WAVE, 0.8f);
		}

	}

	if (WAVE_COMPLETE == m_tWaveInfo.ePhaseType)
	{
		m_fWaveTime += fTimeDelta;
		if (m_fWaveTime >= 1.f)
		{
			++m_fCreateTime;
			m_fWaveTime = 0.f;
		}
		if (m_fCreateTime > 3.f)
		{
			m_pInfoSubject->Notify_Message(MESSAGE_BUILD_PHASECHANGE);
			++m_tWaveInfo.iCurrentWave;
		}
	}
	else if (COMBAT_PHASE == m_tWaveInfo.ePhaseType)
	{

		m_fWaveTime += fTimeDelta;
		if (m_fWaveTime >= 1.f)
		{
			++m_fCreateTime;
			m_fWaveTime = 0.f;
		}
		if (m_fCreateTime < 6.f || (m_fCreateTime >= 20.f && m_fCreateTime <= 25.f) || (m_fCreateTime >= 11.f && m_fCreateTime <= 15.f))
		{
			switch (m_pWaveObserver->Get_WaveInfo().iCurrentWave)
			{
			case 1:
				Create_StageMonster(0);
				break;
			case 2:
				Create_StageMonster(1);
				break;
			case 3:
				Create_StageMonster(2);
				break;
			}
		}
		if (m_pWaveObserver->Get_WaveInfo().iCurrentWave == 3 && m_fCreateTime >= 50.f)
			m_fCreateTime = 10.f;
	}
}

HRESULT CScene_Stage::Create_StageMonster(_int iStage)
{
	Engine::CGameObject*      pGameObject = nullptr;

	vector<WAVECREATE_INFO*> vecStageWave = Engine::CDataManager::GetInstance()->Get_StageWave(iStage);

	for (_uint i = 0; i < vecStageWave.size(); ++i)
	{
		if (m_fCreateTime == vecStageWave[i]->fCreateTime && m_fWaveTime == 0.f)
		{
			switch (vecStageWave[i]->eMonsterType)
			{
			case Engine::MONSTER_GOBLIN:
				pGameObject = CMonster_Goblin::Create(m_pGraphicDev, vecStageWave[i]->vPosition, vecStageWave[i]->eMonsterRoute);
				if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
					return E_FAIL;
				break;
			case Engine::MONSTER_DARKELF:
				pGameObject = CMonster_DarkElfArcher::Create(m_pGraphicDev, vecStageWave[i]->vPosition, vecStageWave[i]->eMonsterRoute);
				if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
					return E_FAIL;
				break;
			case Engine::MONSTER_ORC:
				pGameObject = CMonster_OrcBruiser::Create(m_pGraphicDev, vecStageWave[i]->vPosition, vecStageWave[i]->eMonsterRoute);
				if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
					return E_FAIL;
				break;
			case Engine::MONSTER_KOBOLD:
				pGameObject = CMonster_Kobold::Create(m_pGraphicDev, vecStageWave[i]->vPosition, vecStageWave[i]->eMonsterRoute);
				if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
					return E_FAIL;
				break;
			case Engine::MONSTER_DEMON:
				pGameObject = CMonster_Demon::Create(m_pGraphicDev, vecStageWave[i]->vPosition);
				if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
					return E_FAIL;
				break;
			}
		}
	}

	return NOERROR;
}

CScene_Stage * CScene_Stage::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CScene_Stage* pInstance = new CScene_Stage(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CScene_Stage Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CScene_Stage::Free(void)
{
	m_pEffectMgr->Release_Effect();
	m_pClinetNetwork->Stage_Clear();
	m_pCameraMgr->Reset_Camera();
	m_pNavMgr->Reset_Cell();
	CDataManager::GetInstance()->Reset_Data();
	Engine::CMouse_Manager::GetInstance()->Reset();

	m_pInfoSubject->Remove_Data(MESSAGE_SCENECHANGE, &m_eSceneId);
	m_pInfoSubject->Remove_Data(MESSAGE_PLUSCOUNT, &m_tWaveInfo);
	m_pInfoSubject->Remove_Data(MESSAGE_MINUSCOUNT, &m_tWaveInfo);
	m_pInfoSubject->Remove_Data(MESSAGE_BUILD_PHASECHANGE, &m_tWaveInfo);
	m_pInfoSubject->Remove_Data(MESSAGE_MAX_UNITCOUNT, &m_tWaveInfo.iTotalUnits);

	m_pInfoSubject->UnSubscribe_Observer(m_pWaveObserver);
	Engine::Safe_Release(m_pWaveObserver);
	Engine::Safe_Release(m_pInfoSubject);
	Engine::Safe_Release(m_pCameraMgr);
	Engine::Safe_Release(m_pEffectMgr);
	Engine::Safe_Release(m_pNavMgr);
	Engine::Safe_Release(m_pClinetNetwork);

	m_pObjectMgr->Release_GameObject(SCENE_STAGE);
	m_pComponentMgr->Release_Component(SCENE_STAGE);

	CLight_Manager::GetInstance()->Reset_Light();

	Engine::CScene::Free();
}