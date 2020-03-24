#include "stdafx.h"
#include "FloatCamera.h"
#include "Object_Manager.h"
#include "Camera_Manager.h"
#include "Panel_Login.h"

CFloatCamera::CFloatCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
{
}


CFloatCamera::~CFloatCamera(void)
{
}

HRESULT CFloatCamera::Ready_GameObject()
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

void CFloatCamera::Change_Camera(_uint uNum, float fAttachTime)
{
	Initialize_Camera();
	m_bCamAttach = true;

	Engine::CGameObject* pGameObject = nullptr;

	pGameObject = CPanel_Login::Create(m_pGraphicDev);
	if (nullptr == pGameObject)
		return;
	CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE2, L"Layer_UI", pGameObject);
}

int CFloatCamera::Update_GameObject(const float & fTimeDelta)
{
	return Engine::CCamera::Update_GameObject(fTimeDelta);
}

CFloatCamera* CFloatCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFloatCamera* pCamera = new CFloatCamera(pGraphicDev);
	if (FAILED(pCamera->Ready_GameObject()))
	{
		MSG_BOX(L"CFloatCamera Create Failed - CFloatCamera::Create()");
		Engine::Safe_Release(pCamera);
	}

	return pCamera;
}

void CFloatCamera::Free(void)
{
	Engine::CCamera::Free();
}
