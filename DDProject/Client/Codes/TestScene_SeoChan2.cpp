#include "stdafx.h"
#include "testScene_SeoChan2.h"
#include "Component_Manager.h"
#include "ClientNetwork.h"
#include "NavMgr.h"
#include "Camera_Manager.h"
#include "DataManager.h"
#include "Light_Manager.h"
#include "FrustumManager.h"

#include "BlockGroup.h"

#include "Crystal.h"
#include "Accessory1.h"
#include "Accessory2.h"
#include "TresureBox.h"

#include "DynamicCamera.h"

#include "MapTexture.h"

#include "Stage2_SkyBall.h"
#include "Stage2_SkyBox.h"

CTestScene_SeoChan2::CTestScene_SeoChan2(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
	, m_LightPos(CDataManager::GetInstance()->Get_LightPosition(Engine::OBJECT_POINT_LIGHT0))
{

}

CTestScene_SeoChan2::~CTestScene_SeoChan2()
{
}

HRESULT CTestScene_SeoChan2::Ready_Scene(void)
{
	if (FAILED(Ready_LayerForBackGround()))
		return E_FAIL;

	if (FAILED(Ready_LayerForGameObject()))
		return E_FAIL;

	if (FAILED(Ready_MapObject()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

	return NOERROR;
}

int CTestScene_SeoChan2::Update_Scene(const float & fTimeDelta)
{
	Engine::CScene::Update_Scene(fTimeDelta);

	_matrix matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	CFrustumManager::GetInstance()->MakeFrustumPlane(matView, matProj);

	return 0;
}

void CTestScene_SeoChan2::Render_Scene(void)
{
	int k = 0;
}

HRESULT CTestScene_SeoChan2::Ready_LayerForBackGround(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	// Map

	// Camera
	pGameObject = CDynamicCamera::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE2, L"Layer_BackGround", pGameObject)))
		return E_FAIL;

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

HRESULT CTestScene_SeoChan2::Ready_LayerForGameObject(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	return NOERROR;
}

HRESULT CTestScene_SeoChan2::Ready_Light(void)
{
	CLight_Manager::GetInstance()->Reset_Light(); // ±âÁ¸ Light Reset

	D3DLIGHT9      LightInfo;
	// Directional
	ZeroMemory(&LightInfo, sizeof(D3DLIGHT9));

	LightInfo.Type = D3DLIGHT_DIRECTIONAL;
	LightInfo.Direction = _vec3(0.f, -0.57735f, 0.f);
	LightInfo.Diffuse = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.f);
	LightInfo.Ambient = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.f);
	LightInfo.Specular = D3DXCOLOR(0.8f, 0.8f, 0.8f, 1.f);

	if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTestScene_SeoChan2::Add_MapObject(Engine::OBJECT & tObject)
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

	//case Engine::OBJECT2_MONSTER_ROUTE0:
	//case Engine::OBJECT2_MONSTER_ROUTE1:
	//case Engine::OBJECT2_MONSTER_ROUTE2:
	//case Engine::OBJECT2_MONSTER_ROUTE3:
	//case Engine::OBJECT2_MONSTER_ROUTE4:
	//case Engine::OBJECT2_MONSTER_ROUTE5:
	//case Engine::OBJECT2_MONSTER_ROUTE6:
	//case Engine::OBJECT2_MONSTER_ROUTE7:
	//	CDataManager::GetInstance()->Add_MonsterRoute(tObject.vecPos, tObject.eId, OBJECT2_MONSTER_ROUTE0);
	//	//pGameObject = CMapTexture::Create(m_pGraphicDev, tObject.eId);
	//	break;
	case Engine::OBJECT2_POINT_LIGHT0:
	case Engine::OBJECT2_POINT_LIGHT1:
	case Engine::OBJECT2_POINT_LIGHT2:
	case Engine::OBJECT2_POINT_LIGHT3:
	case Engine::OBJECT2_POINT_LIGHT4:
		CDataManager::GetInstance()->Add_PointLight(tObject.vecPos, tObject.eId, OBJECT2_POINT_LIGHT0);
		pGameObject = CMapTexture::Create(m_pGraphicDev, tObject.eId);
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

HRESULT CTestScene_SeoChan2::Ready_MapObject(void)
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

CTestScene_SeoChan2 * CTestScene_SeoChan2::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTestScene_SeoChan2* pInstance = new CTestScene_SeoChan2(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CTestScene_SeoChan2 Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}


void CTestScene_SeoChan2::Free(void)
{
	Engine::CScene::Free();
}
