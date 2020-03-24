#include "stdafx.h"
#include "TestScene_Seungho.h"

// Manager
#include "Camera_Manager.h"
#include "Light_Manager.h"

// BackGround
#include "MagusQuarters.h"

// GameObject

// UI
#include "DynamicCamera.h"

#include "StaticUI.h"

#include "Font_ActivateCrystal.h"

#include "Text_GameStart.h"

CTestScene_SeungHo::CTestScene_SeungHo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CScene(pGraphicDev)
	, m_pCameraMgr(CCamera_Manager::GetInstance())
{
	m_pCameraMgr->Add_Ref();
}

CTestScene_SeungHo::~CTestScene_SeungHo()
{
}

HRESULT CTestScene_SeungHo::Ready_Scene(void)
{
	if (FAILED(Ready_LayerForBackGround()))
		return E_FAIL;
	if (FAILED(Ready_LayerForGameObject()))
		return E_FAIL;
	if (FAILED(Ready_LayerForUI()))
		return E_FAIL;
	if (FAILED(Ready_Light()))
		return E_FAIL;

	m_pCameraMgr->Change_Camera(CAMERA_DYNAMIC);

	return NOERROR;
}

int CTestScene_SeungHo::Update_Scene(const float & fTimeDelta)
{
	Engine::CScene::Update_Scene(fTimeDelta); 

	m_pCameraMgr->Update_Camara(fTimeDelta);
	
	return 0;
}

void CTestScene_SeungHo::Render_Scene(void)
{
}

void CTestScene_SeungHo::FirstRender_Finish(void)
{
}

HRESULT CTestScene_SeungHo::Ready_LayerForBackGround(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	// Map
	AddGameObject(CMagusQuarters, SCENE_STAGE, L"Layer_BackGround");

	return NOERROR;
}

HRESULT CTestScene_SeungHo::Ready_LayerForGameObject(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	return NOERROR;
}

HRESULT CTestScene_SeungHo::Ready_LayerForUI(void)
{
	Engine::CGameObject*		pGameObject = nullptr;

	//// Camera	
	AddCamera(CDynamicCamera, CAMERA_DYNAMIC);

	//// StaticUI
	AddGameObject(CStaticUI, SCENE_STAGE, L"Layer_UI"); // ±âÅ¸ UIµé

	AddGameObject(CFont_ActivateCrystal, SCENE_STAGE, L"Layer_UI");

	AddGameObject(CText_GameStart, SCENE_STAGE, L"Layer_UI");
	

	return NOERROR;
}

HRESULT CTestScene_SeungHo::Ready_Light(void)
{
	// Directional
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

CTestScene_SeungHo * CTestScene_SeungHo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTestScene_SeungHo* pInstance = new CTestScene_SeungHo(pGraphicDev);

	if (FAILED(pInstance->Ready_Scene()))
	{
		MessageBox(0, L"CTestScene_SeungHo Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CTestScene_SeungHo::Free(void)
{
	Engine::Safe_Release(m_pCameraMgr);

	Engine::CScene::Free();
}
