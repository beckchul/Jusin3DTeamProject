#include "stdafx.h"
#include "SummonCamera.h"
#include "PlayerObserver.h"
#include "InfoSubject.h"
#include "Mouse_Manager.h"

CSummonCamera::CSummonCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
	, m_fRadius(0.f)
{
}


CSummonCamera::~CSummonCamera(void)
{
}

HRESULT CSummonCamera::Ready_GameObject()
{
	m_pPlayerObserver = CPlayerObserver::Create();
	Engine::CInfoSubject::GetInstance()->Subscribe_Observer(m_pPlayerObserver);

	m_vEye = _vec3(0.0f, 10.f, -20.f);
	m_vAt = _vec3(0.f, 0.f, 0.f);
	m_vUp = _vec3(0.f, 1.f, 0.f);

	m_fFovy = D3DXToRadian(60.f);
	m_fAspect = float(WINCX) / WINCY;
	m_fNear = PLANE_NEAR;
	m_fFar = PLANE_FAR;

	m_fRadius = 180.f;

	Engine::CCamera::Ready_GameObject();

	return S_OK;
}

void CSummonCamera::Change_Camera(_uint uNum, float fAttachTime)
{
	Initialize_Camera();
	m_bCamAttach = false;

	_matrix matPlayer = *m_pPlayerObserver->Get_PlayerInfo(g_iClientIndex)->pTransform->Get_WorldMatrix();

	_vec3 Player_Pos;
	memcpy(&Player_Pos, matPlayer.m[3], sizeof(_vec3));

	_vec3 vecDir = m_vDir;
	vecDir.y = 0.f;
	D3DXVec3Normalize(&vecDir, &vecDir); // 원래 카메라의 방향을 유지하자

	m_vDestEye = Player_Pos - vecDir * 45.f  + _vec3(0.f, 80.f, 0.f); // 카메라의 목표 지점
	m_fDistance = D3DXVec3Length(&(m_vDestEye - m_vEye));

	m_vDestDir = Player_Pos - m_vDestEye;
	D3DXVec3Normalize(&m_vDestDir, &m_vDestDir);

	m_fAngle = acosf(D3DXVec3Dot(&m_vDestDir, &m_vDir));
	D3DXVec3Cross(&m_vAxis, &m_vDir, &m_vDestDir); // 회전 축
}

int CSummonCamera::Update_GameObject(const float & fTimeDelta)
{
	CCamera::Flow_Camera(fTimeDelta, m_vDestEye, m_fDistance, m_vDestDir, m_fAngle, m_vAxis, 1.f);

	m_vAt = m_vEye + m_vDir;

	return Engine::CCamera::Update_GameObject(fTimeDelta);
}

CSummonCamera* CSummonCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSummonCamera* pCamera = new CSummonCamera(pGraphicDev);
	if (FAILED(pCamera->Ready_GameObject()))
	{
		MSG_BOX(L"CSummonCamera Create Failed - CSummonCamera::Create()");
		Engine::Safe_Release(pCamera);
	}

	return pCamera;
}

void CSummonCamera::Free(void)
{
	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pPlayerObserver);
	Engine::Safe_Release(m_pPlayerObserver);

	Engine::CCamera::Free();
}
