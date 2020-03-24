#include "stdafx.h"
#include "TowerCamera.h"
#include "PlayerObserver.h"
#include "InfoSubject.h"
#include "Mouse_Manager.h"

CTowerCamera::CTowerCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
	, m_fRadius(0.f)
	, m_bFixCursor(true)
{
}


CTowerCamera::~CTowerCamera(void)
{
}

HRESULT CTowerCamera::Ready_GameObject()
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

void CTowerCamera::Change_Camera(_uint uNum, float fAttachTime)
{
	Initialize_Camera();
	m_bCamAttach = false;
	m_fAttachTime = fAttachTime;
	
	POINT		ptMouse = { WINCX >> 1, WINCY >> 2 }; // 마우스 커서 살짝 위로 이동
	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);

	_matrix matPlayer = *m_pPlayerObserver->Get_PlayerInfo(g_iClientIndex)->pTransform->Get_WorldMatrix();

	_vec3 Player_Look, Player_Pos;
	memcpy(&Player_Look, matPlayer.m[2], sizeof(_vec3));
	memcpy(&Player_Pos, matPlayer.m[3], sizeof(_vec3));

	D3DXVec3Normalize(&Player_Look, &Player_Look);

	m_vDestEye = Player_Pos - Player_Look * 5.f + _vec3(0.f, 120.f, 0.f); // 카메라의 목표 지점
	m_fDistance = D3DXVec3Length(&(m_vDestEye - m_vEye));

	m_vDestDir = Player_Pos - m_vDestEye;
	D3DXVec3Normalize(&m_vDestDir, &m_vDestDir);

	m_fAngle = acosf(D3DXVec3Dot(&m_vDestDir, &m_vDir));
	D3DXVec3Cross(&m_vAxis, &m_vDir, &m_vDestDir); // 회전 축

	m_bFixCursor = true;
}

void CTowerCamera::Restrict_MouseCursor(void)
{
	_vec3 vecMouse = CMouse_Manager::GetInstance()->GetMousePos();
	_vec3 vecCenter = { WINCX >> 1, WINCY >> 1, 0.f };

	_vec3 vecDir = vecMouse - vecCenter;

	float fDistance = D3DXVec3Length(&vecDir); // 중심에서의 거리

	if (fDistance > m_fRadius) // 반지름을 벗어날 수 없다
	{
		float fAngle = atan2f(vecDir.y, vecDir.x);

		POINT		ptMouse = { LONG(vecCenter.x + m_fRadius * cos(fAngle)), 
								LONG(vecCenter.y + m_fRadius * sin(fAngle)) };

		ClientToScreen(g_hWnd, &ptMouse);
		SetCursorPos(ptMouse.x, ptMouse.y);
	}
}

int CTowerCamera::Update_GameObject(const float & fTimeDelta)
{
	CCamera::Flow_Camera(fTimeDelta, m_vDestEye, m_fDistance, m_vDir, m_fAngle, m_vAxis, m_fAttachTime);

	m_vAt = m_vEye + m_vDir;
	
	if(m_bFixCursor)
		Restrict_MouseCursor(); // 마우스 범위 제한

	return Engine::CCamera::Update_GameObject(fTimeDelta);
}

CTowerCamera* CTowerCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTowerCamera* pCamera = new CTowerCamera(pGraphicDev);
	if (FAILED(pCamera->Ready_GameObject()))
	{
		MSG_BOX(L"CTowerCamera Create Failed - CTowerCamera::Create()");
		Engine::Safe_Release(pCamera);
	}

	return pCamera;
}

void CTowerCamera::Free(void)
{
	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pPlayerObserver);
	Engine::Safe_Release(m_pPlayerObserver);

	Engine::CCamera::Free();
}
