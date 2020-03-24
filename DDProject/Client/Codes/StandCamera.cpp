#include "stdafx.h"
#include "StandCamera.h"
#include "InputDev.h"
#include "Object_Manager.h"
#include "Camera_Manager.h"
#include "Title.h"
#include "Text_PressStart.h"

CStandCamera::CStandCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
	, m_pInputDev(Engine::CInputDev::GetInstance())
{
	m_pInputDev->Add_Ref();
}


CStandCamera::~CStandCamera(void)
{
}

HRESULT CStandCamera::Ready_GameObject()
{
	m_vEye = _vec3(0.0f, 10.f, -20.f);
	m_vAt = _vec3(0.f, 0.f, 0.f);
	m_vUp = _vec3(0.f, 1.f, 0.f);

	m_fFovy = D3DXToRadian(60.f);
	m_fAspect = float(WINCX) / WINCY;
	m_fNear = PLANE_NEAR;
	m_fFar = PLANE_FAR;

	Engine::CCamera::Ready_GameObject();

	return S_OK;
}

void CStandCamera::Change_Camera(_uint uNum, float fAttachTime)
{
	Initialize_Camera();
	m_bCamAttach = true;

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CTitle::Create(m_pGraphicDev);
	Engine::CObject_Manager::GetInstance()->Add_GameObject(SCENE_MENU, L"Layer_GameObject", pGameObject);

	pGameObject = CText_PressStart::Create(m_pGraphicDev);
	Engine::CObject_Manager::GetInstance()->Add_GameObject(SCENE_MENU, L"Layer_GameObject", pGameObject);
}

int CStandCamera::Update_GameObject(const float & fTimeDelta)
{
	KeyCheck(fTimeDelta);

	return Engine::CCamera::Update_GameObject(fTimeDelta);
}

void CStandCamera::KeyCheck(const float & fTimeDelta)
{
	if (m_pInputDev->OnceMouseClick(CInputDev::DIM_LBUTTON))
		CCamera_Manager::GetInstance()->Change_Camera(CAMERA_ACTION, CUTSCENE_MENU2);
}

CStandCamera* CStandCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStandCamera* pCamera = new CStandCamera(pGraphicDev);
	if (FAILED(pCamera->Ready_GameObject()))
	{
		MSG_BOX(L"CStandCamera Create Failed - CStandCamera::Create()");
		Engine::Safe_Release(pCamera);
	}

	return pCamera;
}

void CStandCamera::Free(void)
{
	Engine::Safe_Release(m_pInputDev);
	Engine::CCamera::Free();
}
