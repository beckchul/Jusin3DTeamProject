#include "stdafx.h"
#include "TestScene_BackChul.h"
#include "Component_Manager.h"

// Camera
#include "Camera_Manager.h"
#include "RecordCamera.h"
#include "StaticCamera.h"
#include "DynamicCamera.h"
#include "ActionCamera.h"
#include "TowerCamera.h"

// UI
#include "MousePointer.h"
#include "Mouse_Manager.h"
#include "MapTexture.h"

// Map
#include "MagusQuarters.h"

// Player
#include "Player_Mage.h"
#include "Player_Monk.h"
#include "Player_Huntress.h"

//Weapon
#include "Player_Staff.h"
#include "Player_Spear.h"

// Monster
#include "Monster_Goblin.h"
#include "Monster_OrcBruiser.h"
#include "Monster_Kobold.h"
#include "Monster_DarkElfArcher.h"


// Object
#include "TresureBox.h"
#include "LeftDoor.h"
#include "RightDoor.h"
#include "Crystal.h"
#include "Accessory1.h"
#include "Accessory2.h"
#include "DataManager.h"
#include "3DMaxObject.h"
// NaviMesh
#include "Font_NavMesh.h"
#include "NavMgr.h"

// CollisionMgr
#include "CollisionManager.h"

// Effect
#include "Effect.h"
#include "Effect_Single.h"
#include "EffectMgr.h"


//Network
#include "ClientNetwork.h"
//Light
#include "Light_Manager.h"

CTestScene_BackChul::CTestScene_BackChul(LPDIRECT3DDEVICE9 pGraphicDev)
: Engine::CScene(pGraphicDev)
, m_pNavMgr(CNavMgr::GetInstance())
, m_pCameraMgr(CCamera_Manager::GetInstance()) 
, m_pEffect_Manager(CEffectMgr::GetInstance())
, m_pClinetNetwork(CClientNetwork::GetInstance())
{
	m_pNavMgr->Add_Ref();
	m_pCameraMgr->Add_Ref();
	m_pEffect_Manager->Add_Ref();
	m_pClinetNetwork->Add_Ref();
}


CTestScene_BackChul::~CTestScene_BackChul()
{
}

HRESULT CTestScene_BackChul::Ready_Scene(void)
{
	if (FAILED(Ready_LayerForBackGround()))
		return E_FAIL;

	if (FAILED(Ready_NavMesh()))
		return E_FAIL;

	if (FAILED(Ready_LayerForGameObject()))
		return E_FAIL;

	if (FAILED(Ready_LayerForUI()))
		return E_FAIL;

	if (FAILED(Ready_MapObject()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	m_pClinetNetwork->SetCurrentScene(TESTSCENE_MINKYEONG);

	CCollisionManager::GetInstance()->Initialize_CollisionMgr(); // NavMesh 정보 저장

	/////// Effect_Particle
	list<Engine::CEffect*> pEffectList;

	Engine::CEffect*	pEffect = nullptr;

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_Flare", SCENE_STAGE);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(3.f, 3.f, 3.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_EffectColor(_vec4(0.5f, 0.8f, 0.5f, 1.f));
	pEffect->Set_TimeToDead(1.f);
	pEffect->Set_FadeOut(2.f);
	

	pEffectList.push_back(pEffect);

	m_pEffect_Manager->Add_EffectList(L"Effect_Flare", pEffectList);

	//--------------------------------------------------------------------------------------------

	return NOERROR;
}

int CTestScene_BackChul::Update_Scene(const float& fTimeDelta)
{
	Engine::CScene::Update_Scene(fTimeDelta);
	
	m_pCameraMgr->Update_Camara(fTimeDelta); // 카메라
	m_pEffect_Manager->Update_Effect(fTimeDelta);

	m_pNavMgr->Update_NavMesh(fTimeDelta); // 내비메쉬
	m_pEffect_Manager->Update_Effect(fTimeDelta);
	return 0;
}

void CTestScene_BackChul::Render_Scene(void)
{
	m_pNavMgr->Render_NavMesh();
	CCollisionManager::GetInstance()->Render_CollisionSphere();
}

HRESULT CTestScene_BackChul::Ready_LayerForBackGround(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	// Map
	pGameObject = CMagusQuarters::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_BackGround", pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTestScene_BackChul::Ready_LayerForGameObject(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	//// Player_Mage
	//pGameObject = CPlayer_Mage::Create(m_pGraphicDev);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//	return E_FAIL;


	//Player_Monk
	{
		pGameObject = CPlayer_Monk::Create(m_pGraphicDev);
		if (nullptr == pGameObject)
			return E_FAIL;
		if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
			return E_FAIL;
	}
	//pGameObject = CPlayer_Huntress::Create(m_pGraphicDev);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//	return E_FAIL;


	//pGameObject = CMonster_Goblin::Create(m_pGraphicDev, D3DXVECTOR3(48.f, -62.f, -480.f), Engine::OBJECT_MONSTER_ROUTE0);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//	return E_FAIL;

	//pGameObject = CMonster_Goblin::Create(m_pGraphicDev, D3DXVECTOR3(-39.f, -63.f, -496.f), Engine::OBJECT_MONSTER_ROUTE1);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//	return E_FAIL;

	//pGameObject = CMonster_DarkElfArcher::Create(m_pGraphicDev, D3DXVECTOR3(-409.f, -115.f, -370.f), Engine::OBJECT_MONSTER_ROUTE2);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//	return E_FAIL;

	//pGameObject = CMonster_Goblin::Create(m_pGraphicDev, D3DXVECTOR3(-502.f, -63.f, -81.f), Engine::OBJECT_MONSTER_ROUTE3);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//	return E_FAIL;

	//pGameObject = CMonster_Goblin::Create(m_pGraphicDev, D3DXVECTOR3(-502.f, -63.f, 81.f), Engine::OBJECT_MONSTER_ROUTE4);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//	return E_FAIL;

	//pGameObject = CMonster_DarkElfArcher::Create(m_pGraphicDev, D3DXVECTOR3(-409.f, -115.f, 370.f), Engine::OBJECT_MONSTER_ROUTE5);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//	return E_FAIL;

	//pGameObject = CMonster_Goblin::Create(m_pGraphicDev, D3DXVECTOR3(-36.f, -63.f, 503.f), Engine::OBJECT_MONSTER_ROUTE6);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//	return E_FAIL;

	//pGameObject = CMonster_Goblin::Create(m_pGraphicDev, D3DXVECTOR3(63.f, -63.f, 503.f), Engine::OBJECT_MONSTER_ROUTE7);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//	return E_FAIL;

	pGameObject = CMonster_OrcBruiser::Create(m_pGraphicDev, D3DXVECTOR3(63.f, -63.f, 503.f), Engine::OBJECT_MONSTER_ROUTE7);
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
		return E_FAIL;

	////// Monster_OrcBruser
	//for (int i = 0; i < 1; ++i)
	//{
	//	pGameObject = CMonster_OrcBruiser::Create(m_pGraphicDev, D3DXVECTOR3(-90.f, -10.f, 0.f), Engine::OBJECT_MONSTER_ROUTE0);
	//	if (nullptr == pGameObject)
	//		return E_FAIL;
	//	if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//		return E_FAIL;
	//}

	////// Monster_Kobold
	//for (int i = 0; i < 1; ++i)
	//{
	//	pGameObject = CMonster_Kobold::Create(m_pGraphicDev, D3DXVECTOR3(-90.f, -10.f, i * 30.f - 150.f));
	//	if (nullptr == pGameObject)
	//		return E_FAIL;
	//	if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//		return E_FAIL;
	//}

	////// Monster_DarkElfArcher
	//for (int i = 0; i < 1; ++i)
	//{
	//	
	//	pGameObject = CMonster_DarkElfArcher::Create(m_pGraphicDev, D3DXVECTOR3(0.f, -10.f, i * -30.f - 200.f));
	//	if (nullptr == pGameObject)
	//		return E_FAIL;
	//	if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//		return E_FAIL;
	//}

	return NOERROR;
}

HRESULT CTestScene_BackChul::Ready_LayerForUI(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	//// Camera	
	AddCamera(CStaticCamera, CAMERA_STATIC);
	AddCamera(CDynamicCamera, CAMERA_DYNAMIC);
	AddCamera(CRecordCamera, CAMERA_RECORDING);
	AddCamera(CActionCamera, CAMERA_ACTION);
	AddCamera(CTowerCamera, CAMERA_TOWER);

	// Mouse
	AddGameObject(CMousePointer, SCENE_STAGE, L"Layer_UI");
	Engine::CMouse_Manager::GetInstance()->Set_MousePointer(pGameObject);

	return NOERROR;
}

HRESULT CTestScene_BackChul::Ready_Light(void)
{
	//// Light0 (벽)
	//vector<_vec3> vecLightPos0 = Engine::CDataManager::GetInstance()->Get_LightPosition(Engine::OBJECT_POINT_LIGHT0);
	//CLight_Manager::GetInstance()->Reset_Light(); // 기존 Light Reset

	D3DLIGHT9		LightInfo;

	// Directional
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

HRESULT CTestScene_BackChul::Ready_NavMesh(void)
{
	CNavMgr::GetInstance()->ReserveCellSize(m_pGraphicDev, 2000);

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

HRESULT CTestScene_BackChul::Add_MapObject(Engine::OBJECT & tObject)
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

	case Engine::OBJECT_MONSTER_ROUTE0:
	case Engine::OBJECT_MONSTER_ROUTE1:
	case Engine::OBJECT_MONSTER_ROUTE2:
	case Engine::OBJECT_MONSTER_ROUTE3:
	case Engine::OBJECT_MONSTER_ROUTE4:
	case Engine::OBJECT_MONSTER_ROUTE5:
	case Engine::OBJECT_MONSTER_ROUTE6:
	case Engine::OBJECT_MONSTER_ROUTE7:
		CDataManager::GetInstance()->Add_MonsterRoute(tObject.vecPos, tObject.eId);
		pGameObject = CMapTexture::Create(m_pGraphicDev, tObject.eId);
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

HRESULT CTestScene_BackChul::Ready_MapObject(void)
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

CTestScene_BackChul * CTestScene_BackChul::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTestScene_BackChul* pInstance = new CTestScene_BackChul(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CTestScene_BackChul Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}


void CTestScene_BackChul::Free(void)
{
	m_pObjectMgr->Release_GameObject(SCENE_STAGE);
	m_pComponentMgr->Release_Component(SCENE_STAGE);
	Engine::Safe_Release(m_pNavMgr);
	Engine::Safe_Release(m_pCameraMgr);
	Engine::Safe_Release(m_pEffect_Manager);
	Engine::Safe_Release(m_pClinetNetwork);
	Engine::CScene::Free();
}
