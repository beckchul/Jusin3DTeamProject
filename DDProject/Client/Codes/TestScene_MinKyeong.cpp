#include "stdafx.h"

#include "MagusQuarters.h"
#include "Player_Spear.h"
#include "Player_Staff.h"
#include "Crystal.h"
#include "Accessory1.h"
#include "Accessory2.h"
#include "CrystalLight.h"
#include "3DMaxObject.h"

#include "Player_Huntress.h"
#include "Monster_Goblin.h"
#include "Monster_OrcBruiser.h"
#include "Monster_Kobold.h"
#include "TresureBox.h"
#include "LeftDoor.h"
#include "RightDoor.h"

#include "StaticCamera.h"
#include "DynamicCamera.h"
#include "RecordCamera.h"
#include "MiniMap.h"

#include "Panel_Level.h"
#include "Panel_SkillIcon.h"
#include "Panel_Wave.h"
#include "MapTexture.h"


#include "TestScene_MinKyeong.h"
#include "Component_Manager.h"
#include "DynamicCamera.h"
#include "InputDev.h"

#include "Camera_Manager.h"

// Map
#include "MagusQuarters.h"

// Player
#include "Player_Huntress.h"
#include "Player_Monk.h"
#include "Player_Spear.h"
#include "OldMap.h"

// UI
#include "StaticCamera.h"
#include "RecordCamera.h"
#include "DynamicCamera.h"
#include "TowerCamera.h"
#include "ActionCamera.h"
#include "SummonCamera.h"
#include "MousePointer.h"

// Font
#include "Font_Score.h"
#include "Font_Level.h"
#include "Font_Hp.h"
#include "Font_Mp.h"
#include "Font_MaxHp.h"
#include "Font_MaxMp.h"
#include "Font_Wave.h"
#include "Font_DefenseUnits.h"
#include "Font_NextEnemies.h"
#include "Font_FPS.h"

#include "Font_NavMesh.h"
#include "MapTexture.h"

#include "StaticUI.h"


///////// Effect_Mgr
#include "EffectMgr.h"

///////// Effect_2D
#include "Effect_Single.h"
#include "Effect_Frame.h"

///////// Effect_Particle
#include "Effect_Particle.h"
#include "Effect_RectParticle.h"

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
#include "Effect_Mesh_Projectile1.h"
#include "Effect_Mesh_GlowBall.h"
#include "Effect_Decal.h"

#include "CollisionManager.h"
#include "NavMgr.h"
#include "Font_NavMesh.h"

#include "Crystal.h"
#include "Monster_Goblin.h"
#include "Light_Manager.h"
#include "DataManager.h"
#include "ClientNetwork.h"

// Fog
#include "Effect_Single_Fog.h"
#include "Effect_Mesh_FogPlane.h"
#include "Effect_Mesh_FogGroup1.h"
#include "Effect_Mesh_FogGroup4.h"

CTestScene_MinKyeong::CTestScene_MinKyeong(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
	, m_pEffect_Manager(Engine::CEffectMgr::GetInstance())
	, m_pCameraMgr(CCamera_Manager::GetInstance())
	, m_pClinetNetwork(CClientNetwork::GetInstance())
{
	m_pClinetNetwork->Add_Ref();
	m_pEffect_Manager->Add_Ref();
	m_pCameraMgr->Add_Ref();
}

CTestScene_MinKyeong::~CTestScene_MinKyeong()
{
}

HRESULT CTestScene_MinKyeong::Ready_Scene(void)
{
	if (FAILED(Ready_NavMesh()))
		return E_FAIL;

	if (FAILED(Ready_EffectList()))
		return E_FAIL;

	if (FAILED(Ready_LayerForBackGround()))
		return E_FAIL;

	if (FAILED(Ready_LayerForGameObject()))
		return E_FAIL;

	if (FAILED(Ready_MapObject()))
		return E_FAIL;

	if (FAILED(Ready_LayerForUI()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	CCollisionManager::GetInstance()->Initialize_CollisionMgr(); // NavMesh 정보 저장


	m_pClinetNetwork->SetCurrentScene(TESTSCENE_MINKYEONG);

	m_pEffect = CEffect_Decal::Create(m_pGraphicDev, L"Com_Texture_Decal");
	if (nullptr == m_pEffect)
		return E_FAIL;
	m_pEffect->Set_EffectColor(_vec4(1.f, 1.f, 0.f, 1.f));
	m_pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(20.f, 10.f, 20.f));
	m_pEffect->Set_Information(Engine::CTransform::INFO_POSITION, &_vec3(0.f, 10.f, 0.f));
	m_pEffect_Manager->Add_Effect(m_pEffect);

	//Engine::CEffect* pEffect = nullptr;
	//pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STAGE);
	//if (nullptr == pEffect)
	//	return E_FAIL;
	//dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.05f);
	//pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(100.f, 100.f, 1.f));
	//pEffect->Set_Information(Engine::CTransform::INFO_POSITION, &_vec3(-45.f, 30.f, 0.f));
	//pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	//m_pEffect_Manager->Add_Effect(pEffect);


	return NOERROR;
}

int CTestScene_MinKyeong::Update_Scene(const float & fTimeDelta)
{
	m_pClinetNetwork->Check_Player_Create();
	m_pClinetNetwork->Check_Player_Delete();
	m_pCameraMgr->Update_Camara(fTimeDelta); // 카메라

	Engine::CScene::Update_Scene(fTimeDelta);
	Engine::CNavMgr::GetInstance()->Update_NavMesh(fTimeDelta); // 내비메쉬(폰트) -> 시연회땐 주석
	
	m_pEffect_Manager->Update_Effect(fTimeDelta);

	float fRandFloat = RandFloat;
	int iRandInt = Random(50);
	_vec3 vPos = _vec3(fRandFloat * iRandInt, fRandFloat * iRandInt, fRandFloat * iRandInt);

	if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_SPACE))
	{
		m_pEffect_Manager->Add_Effect(L"Effect_Aura", &vPos);
	}

	if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_1))
	{
		m_pEffect_Manager->Add_Effect(L"Effect_Torch", &vPos);
	}

	if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_2))
	{
		m_pEffect_Manager->Add_Effect(L"Effect_Spiral", &vPos);
	}

	if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_3))
	{
		m_pEffect_Manager->Add_Effect(L"Effect_PurpleStars", &vPos);
	}

	if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_4))
	{
		m_pEffect_Manager->Add_Effect(L"Effect_ManaBomb", &vPos);
	}

	if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_5))
	{
		m_pEffect_Manager->Add_Effect(L"Effect_Flare", &vPos);
	}

	if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_9))
	{
		m_pEffect_Manager->Add_Effect(L"Effect_FireTower_Missile", &_vec3(80.f, 30.f, 80.f), _vec3(-40.f, 7.f, -40.f), 50.f);

	}

	if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_0))
	{
		CEffect_Particle* pParticle = nullptr;
		pParticle = CEffect_Particle::Create(m_pGraphicDev,
			*(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_GreenBlood")), SCENE_STAGE);
		pParticle->Set_OriginPos(vPos);
		m_pEffect_Manager->Add_Effect(pParticle);
	}

	if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_DELETE))
	{
		m_pEffect_Manager->Release_Effect();
	}

	//if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_NUMPAD1))
	//{
	//	m_pEffect_Manager->Add_Effect(L"Effect_Mage_Charging_Att", &_vec3(0.f, 0.f, 0.f));

	//	Engine::CEffect* pEffect = CEffect_RectParticle::Create(m_pGraphicDev
	//		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_ChargeAtt")));
	//	if (nullptr == pEffect)
	//		return E_FAIL;
	//	pEffect->Set_OriginPos(_vec3(0.f, 0.f, 0.f));
	//	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_RotZ();
	//	m_pEffect_Manager->Add_Effect(pEffect);
	//}

	//if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_NUMPAD2))
	//{
	//	Engine::CEffect* pEffect = CEffect_Particle::Create(m_pGraphicDev
	//		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_FireTower_Smoke")), SCENE_STAGE);
	//	if (nullptr == pEffect)
	//		return E_FAIL;
	//	pEffect->Set_ShaderPass(CEffect::EFFECT_RECTPARTICLE_INV);
	//	dynamic_cast<CEffect_Particle*>(pEffect)->Set_DestBlendInv();
	//	dynamic_cast<CEffect_Particle*>(pEffect)->Set_FireWork_YPositive();
	//	dynamic_cast<CEffect_Particle*>(pEffect)->Set_OriginPos(_vec3(20.f, 20.f, 20.f));
	//	m_pEffect_Manager->Add_Effect(pEffect);
	//}
	//if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_NUMPAD3))
	//{
	//	//m_pEffect_Manager->Add_Effect(L"Effect_FireTower_Missile_Del", &_vec3(50.f, 30.f, 0.f));
	//	m_pEffect_Manager->Add_Effect(L"Effect_GameStart_Flare", &_vec3(0.f, 25.f, 0.f)); 
	//}

	//if (Engine::CInputDev::GetInstance()->OnceKeyDown(DIK_NUMPAD4))
	//{
	//	Engine::CEffect* pEffect = CEffect_RectParticle::Create(m_pGraphicDev
	//		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Lava_Bubble")));
	//	if (nullptr == pEffect)
	//		return E_FAIL;
	//	pEffect->Set_OriginPos(_vec3(30.f, 30.f, 30.f));
	//	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_BubbleState(0.5f, 10.f);
	//	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_SpreadExceptY();
	//	m_pEffect_Manager->Add_Effect(pEffect);
	//}


	if (Engine::CInputDev::GetInstance()->StayKeyDown(DIK_NUMPAD5))
	{
		m_vPos.y -= 1.f;
		m_pEffect->Set_Information(CTransform::INFO_POSITION, &m_vPos);
	}
	if (Engine::CInputDev::GetInstance()->StayKeyDown(DIK_NUMPAD4))
	{
		m_vPos.x -= 1.f;
		m_pEffect->Set_Information(CTransform::INFO_POSITION, &m_vPos);
	}
	if (Engine::CInputDev::GetInstance()->StayKeyDown(DIK_NUMPAD6))
	{
		m_vPos.x += 1.f;
		m_pEffect->Set_Information(CTransform::INFO_POSITION, &m_vPos);
	}
	if (Engine::CInputDev::GetInstance()->StayKeyDown(DIK_NUMPAD8))
	{
		m_vPos.y += 1.f;
		m_pEffect->Set_Information(CTransform::INFO_POSITION, &m_vPos);
	}

	if (Engine::CInputDev::GetInstance()->StayKeyDown(DIK_NUMPAD7))
	{
		m_vPos.z += 1.f;
		m_pEffect->Set_Information(CTransform::INFO_POSITION, &m_vPos);
	}
	if (Engine::CInputDev::GetInstance()->StayKeyDown(DIK_NUMPAD9))
	{
		m_vPos.z -= 1.f;
		m_pEffect->Set_Information(CTransform::INFO_POSITION, &m_vPos);
	}
	if (Engine::CInputDev::GetInstance()->StayKeyDown(DIK_F1))
	{
		m_vRotation.x += D3DXToRadian(1.f);
		m_pEffect->Set_Information(CTransform::INFO_ANGLE, &m_vRotation);
	}
	return 0;
}

void CTestScene_MinKyeong::Render_Scene(void)
{
	Engine::CNavMgr::GetInstance()->Render_NavMesh(); // 내비메쉬(Cell)
	CCollisionManager::GetInstance()->Render_CollisionSphere();

}

HRESULT CTestScene_MinKyeong::Ready_EffectList(void)
{

	return S_OK;
}

HRESULT CTestScene_MinKyeong::Ready_LayerForBackGround(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	// Map
	AddGameObject(CMagusQuarters, SCENE_STAGE, L"Layer_BackGround");

	return NOERROR;
}

HRESULT CTestScene_MinKyeong::Ready_LayerForGameObject(void)
{
	Engine::CGameObject*		pGameObject = nullptr;


	//////Crystal
	//pGameObject = CCrystal::Create(m_pGraphicDev);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//	return E_FAIL;

	/*pGameObject = CMonster_Goblin::Create(m_pGraphicDev, _vec3(-100.f, 30.f, 50.f), OBJECT_MONSTER_ROUTE0);
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
		return E_FAIL;*/
	//// -------------------------------------------Effect-------------------------------------------
	//Engine::CEffect* pEffect = nullptr;

	//// Rect_Particle 실험중
	//pEffect = CEffect_Particle::Create(m_pGraphicDev
	//	, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_RectParticle")), SCENE_STAGE);
	//if (nullptr == pEffect)
	//	return E_FAIL;
	//m_pEffect_Manager->Add_Effect(pEffect);

	//pEffect = CEffect_Particle::Create(m_pGraphicDev
	//	, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_PurpleStars")), SCENE_STAGE);
	//if (nullptr == pEffect)
	//	return E_FAIL;
	//dynamic_cast<CEffect_Particle*>(pEffect)->Set_OriginPos(_vec3(0.f, 15.f, 0.f));
	//Engine::CEffectMgr::GetInstance()->Add_Effect(pEffect);


	//m_pEffect_Manager->Add_Effect(L"Effect_Crystal_Radial", &_vec3(0.f, 30.f, 0.f));


	//// --------------------------------------------------------------------------------------------



	return NOERROR;
}

HRESULT CTestScene_MinKyeong::Ready_NavMesh(void)
{
	Engine::CNavMgr::GetInstance()->ReserveCellSize(m_pGraphicDev, 1000);

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

		Engine::CNavMgr::GetInstance()->AddCell(&NavMesh, pFontNumber);
	}
	CloseHandle(hFile);

	Engine::CNavMgr::GetInstance()->LinkCell();

	return NOERROR;

}

HRESULT CTestScene_MinKyeong::Ready_Light(void)
{
	D3DLIGHT9		LightInfo;
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
			LightInfo.Diffuse = D3DXCOLOR(0.95f, 0.38f, 0.65f, 8.4f);
			LightInfo.Specular = D3DXCOLOR(0.95f, 0.38f, 0.65f, 1.f);
			break;

		case 1:
			LightInfo.Diffuse = D3DXCOLOR(1.f, 0.89f, 0.f, 8.4f);
			LightInfo.Specular = D3DXCOLOR(1.f, 0.89f, 0.f, 1.f);
			break;

		case 2:
			LightInfo.Diffuse = D3DXCOLOR(0.12f, 1.f, 0.1f, 8.4f);
			LightInfo.Specular = D3DXCOLOR(0.12f, 1.f, 0.1f, 1.f);
			break;
		}
		LightInfo.Ambient = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);

		if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
			return E_FAIL;

	}

	// Light3 화롯불
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

	// Light4 용암불
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

		LightInfo.Diffuse = D3DXCOLOR(1.f, 0.45f, 0.f, 8.4f);
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


#endif // BadComputer

	// Directional
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

	LightInfo.Type = D3DLIGHT_DIRECTIONAL;
	LightInfo.Direction = _vec3(0.f, -0.57735f, 0.f);
	LightInfo.Diffuse = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f);
	LightInfo.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.f);
	LightInfo.Specular = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f);

	if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
		return E_FAIL;

	return NOERROR;

}

HRESULT CTestScene_MinKyeong::Ready_LayerForUI(void)
{
	Engine::CGameObject*		pGameObject = nullptr;



	// Camera
	AddCamera(CStaticCamera, CAMERA_STATIC);
	AddCamera(CDynamicCamera, CAMERA_DYNAMIC);
	AddCamera(CRecordCamera, CAMERA_RECORDING);
	AddCamera(CActionCamera, CAMERA_ACTION);
	AddCamera(CTowerCamera, CAMERA_TOWER);
	AddCamera(CSummonCamera, CAMERA_SUMMON);

	// Mouse
	AddGameObject(CMousePointer, SCENE_STAGE, L"Layer_UI");
	Engine::CMouse_Manager::GetInstance()->Set_MousePointer(pGameObject);


	return NOERROR;
}

HRESULT CTestScene_MinKyeong::Ready_MapObject(void)
{
	DWORD dwByte = 0;

	HANDLE hFile = CreateFile(L"../Resources/Data/Object/Data.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

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

HRESULT CTestScene_MinKyeong::Add_MapObject(Engine::OBJECT & tObject)
{
	Engine::CGameObject* pGameObject = nullptr;
	int iTemp = 0;

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

CTestScene_MinKyeong * CTestScene_MinKyeong::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTestScene_MinKyeong* pInstance = new CTestScene_MinKyeong(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CTestScene_MinKyeong Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}


void CTestScene_MinKyeong::Free(void)
{
	m_pEffect_Manager->Release_Effect();
	m_pClinetNetwork->Stage_Clear();

	m_pCameraMgr->Reset_Camera();
	Engine::CNavMgr::GetInstance()->Reset_Cell();

	Engine::Safe_Release(m_pEffect_Manager);
	Engine::Safe_Release(m_pCameraMgr);
	Engine::Safe_Release(m_pClinetNetwork);


	m_pObjectMgr->Release_GameObject(SCENE_STAGE);
	m_pComponentMgr->Release_Component(SCENE_STAGE);


	Engine::CScene::Free();
}
