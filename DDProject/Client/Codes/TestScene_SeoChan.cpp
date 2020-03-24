#include "stdafx.h"
#include "TestScene_SeoChan.h"
#include "Component_Manager.h"
#include "ClientNetwork.h"
#include "NavMgr.h"
#include "Camera_Manager.h"
#include "DataManager.h"
#include "Light_Manager.h"
#include "FrustumManager.h"

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

CTestScene_SeoChan::CTestScene_SeoChan(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
	, m_LightPos(CDataManager::GetInstance()->Get_LightPosition(Engine::OBJECT_POINT_LIGHT0))
{

}

CTestScene_SeoChan::~CTestScene_SeoChan()
{
}

HRESULT CTestScene_SeoChan::Ready_Scene(void)
{
	if (FAILED(Ready_LayerForBackGround()))
		return E_FAIL;

	if (FAILED(Ready_LayerForGameObject()))
		return E_FAIL;

	if (FAILED(Ready_MapObject()))
		return E_FAIL;

	if (FAILED(Ready_Light()))
		return E_FAIL;

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

int CTestScene_SeoChan::Update_Scene(const float & fTimeDelta)
{
	Engine::CScene::Update_Scene(fTimeDelta);

	_matrix matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	CFrustumManager::GetInstance()->MakeFrustumPlane(matView, matProj);


	return 0;
}

void CTestScene_SeoChan::Render_Scene(void)
{
	int k = 0;
}

HRESULT CTestScene_SeoChan::Ready_LayerForBackGround(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	// Map
	AddGameObject(CMagusQuarters, SCENE_STAGE, L"Layer_BackGround");

	// Camera
	pGameObject = CDynamicCamera::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_BackGround", pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTestScene_SeoChan::Ready_LayerForGameObject(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	////Player_Huntress
	//pGameObject = CPlayer_Huntress::Create(m_pGraphicDev);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//	return E_FAIL;

	//for (int i = 0; i < 1; ++i)
	//{
	//	// Monster_Goblin
	//	pGameObject = CMonster_Goblin::Create(m_pGraphicDev, D3DXVECTOR3(90.f, -10.f, i * 30.f - 100.f));
	//	if (nullptr == pGameObject)
	//		return E_FAIL;
	//	if (FAILED(m_pObjectMgr->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pGameObject)))
	//		return E_FAIL;
	//}

	return NOERROR;
}

HRESULT CTestScene_SeoChan::Ready_Light(void)
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
	LightInfo.Diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.3f);
	LightInfo.Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.f);
	LightInfo.Specular = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.f);

	if (FAILED(CLight_Manager::GetInstance()->Ready_Light(m_pGraphicDev, &LightInfo)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTestScene_SeoChan::Add_MapObject(Engine::OBJECT & tObject)
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

HRESULT CTestScene_SeoChan::Ready_MapObject(void)
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

CTestScene_SeoChan * CTestScene_SeoChan::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTestScene_SeoChan* pInstance = new CTestScene_SeoChan(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CTestScene_SeoChan Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}


void CTestScene_SeoChan::Free(void)
{
	Engine::CScene::Free();
}
