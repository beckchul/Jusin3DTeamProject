#include "stdafx.h"
#include "Victory_Camera.h"
#include "InfoSubject.h"
#include "InputDev.h"
#include "LastManObserver.h"
#include "DataManager.h"
#include "Mouse_Manager.h"
#include "MousePointer.h"

CVictory_Camera::CVictory_Camera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
	, m_pPlayerObserver(nullptr)
	, m_pInputDev(Engine::CInputDev::GetInstance())
	, m_pPlayerMatrix(nullptr)
	, m_fCameraAngle(0.f)
	, m_fTime(0.f)
	, m_bFixMouse(true)
	, m_uVectoryPlayer(0)
{
	m_pInputDev->Add_Ref();
}

CVictory_Camera::~CVictory_Camera()
{
}

HRESULT CVictory_Camera::Ready_GameObject()
{
	m_pPlayerObserver = CLastManObserver::Create();
	Engine::CInfoSubject::GetInstance()->Subscribe_Observer(m_pPlayerObserver);

	m_vEye = _vec3(0.f, 10.f, -20.f);
	m_vAt = _vec3(0.f, 0.f, 0.f);
	m_vUp = _vec3(0.f, 1.f, 0.f);

	m_fFovy = D3DXToRadian(60.f);
	m_fAspect = float(WINCX) / WINCY;
	m_fNear = PLANE_NEAR;
	m_fFar = PLANE_FAR;

	float fCameraPoisitionComparisonToPlayer[] = { 8.f, 60.f, -100.f }; // 플레이어 대비 카메라의 위치
	memcpy(m_fPlayerRatio, fCameraPoisitionComparisonToPlayer, sizeof(float) * 3);

	Engine::CCamera::Ready_GameObject();

	return NOERROR;
}

void CVictory_Camera::Change_Camera(_uint uNum, float fAttachTime)
{
	m_uVectoryPlayer = uNum; // 승리한 플레이어 인덱스
	cout << m_uVectoryPlayer << " 승리!" << endl;

	Initialize_Camera();
	m_bCamAttach = false;
	m_fAttachTime = fAttachTime;

	Engine::CGameObject* pMousePointer = CMouse_Manager::GetInstance()->Get_MousePointer();
	if (nullptr != pMousePointer)
		dynamic_cast<CMousePointer*>(pMousePointer)->Set_ShowMousePointer(true);

	if (m_fAttachTime == -1.f)
	{
		m_bCamAttach = true;
		return;
	}

	const LASTMAN_INFO* pInfo = m_pPlayerObserver->Get_PlayerInfo(m_uVectoryPlayer);

	if (nullptr != pInfo)
	{
		m_pPlayerMatrix = pInfo->pWorldMatrix;
		m_fCameraAngle = pInfo->fCameraAngle;
	}
	else
		return;

	_vec3 Player_Pos, Player_Right, Player_Look;
	memcpy(&Player_Right, &m_pPlayerMatrix->m[0][0], sizeof(_vec3));
	memcpy(&Player_Look, &m_pPlayerMatrix->m[2][0], sizeof(_vec3));
	memcpy(&Player_Pos, &m_pPlayerMatrix->m[3][0], sizeof(_vec3));

	_matrix		matTrans, matParent, matRot;
	_vec3		vecEye;

	// 플레이어 대비 카메라의 위치
	_vec3		vecTemp = { 0.f, 0.f, 0.f };
	vecTemp += Player_Right * m_fPlayerRatio[0] + Player_Look * m_fPlayerRatio[2];

	D3DXMatrixTranslation(&matTrans, vecTemp.x, vecTemp.y + m_fPlayerRatio[1], vecTemp.z);
	D3DXMatrixTranslation(&matParent, Player_Pos.x, Player_Pos.y, Player_Pos.z);

	D3DXMatrixRotationAxis(&matRot, &Player_Right, D3DXToRadian(m_fCameraAngle));

	D3DXVec3TransformNormal(&Player_Look, &Player_Look, &matRot);

	D3DXVec3TransformCoord(&vecEye, &_vec3(0.f, 0.f, 0.f), &(matTrans * matRot * matParent));

	m_vDestEye = vecEye;
	m_fDistance = D3DXVec3Length(&(vecEye - m_vEye));

	m_vDestDir = Player_Look;
	D3DXVec3Normalize(&m_vDestDir, &m_vDestDir);

	m_fAngle = acosf(D3DXVec3Dot(&m_vDestDir, &m_vDir));
	D3DXVec3Cross(&m_vAxis, &m_vDir, &m_vDestDir); // 회전 축
}

_int CVictory_Camera::Update_GameObject(const float & fTimeDelta)
{
	const LASTMAN_INFO* pInfo = m_pPlayerObserver->Get_PlayerInfo(m_uVectoryPlayer);

	if (nullptr != pInfo)
	{
		m_pPlayerMatrix = pInfo->pWorldMatrix;
		m_fCameraAngle = pInfo->fCameraAngle;
	}

	if (m_bFixMouse)
	{
		Mouse_Move();
		MouseFix();
	}
	KeyCheck(fTimeDelta);

	if (FALSE == m_bCamAttach)
	{
		CCamera::Flow_Camera(fTimeDelta, m_vDestEye, m_fDistance, m_vDestDir, m_fAngle, m_vAxis, m_fAttachTime);
		m_vAt = m_vEye + m_vDir;
	}
	else
		TargetRenewal(fTimeDelta);

	return Engine::CCamera::Update_GameObject(fTimeDelta);
}

void CVictory_Camera::KeyCheck(const float & fTimeDelta)
{
#ifndef Lastman_Client
	if (m_pInputDev->OnceKeyDown(DIK_Q))
		m_bFixMouse = (m_bFixMouse == TRUE) ? FALSE : TRUE;
#endif
}

void CVictory_Camera::Mouse_Move(void)
{
	int		iDistance = 0;

	if (iDistance = m_pInputDev->GetDIMouseMove(Engine::CInputDev::DIM_Y)) // 위 아래
	{
		if (m_fCamAngle[Engine::ANGLE_Y] + iDistance / 20.f <= 30.f && // 각도 제한
			m_fCamAngle[Engine::ANGLE_Y] + iDistance / 20.f >= -15.f)
			m_fCamAngle[Engine::ANGLE_Y] += iDistance / 20.f;
	}

	if (iDistance = m_pInputDev->GetDIMouseMove(Engine::CInputDev::DIM_X)) // 좌 우
	{
		m_fCamAngle[Engine::ANGLE_X] += iDistance / 10.f;
	}
}

void CVictory_Camera::MouseFix(void)
{
	POINT		ptMouse = { WINCX >> 1, WINCY >> 1 };
	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CVictory_Camera::TargetRenewal(const float& fTimeDelta)
{
	if (nullptr == m_pPlayerMatrix)
		return;

	_vec3 Player_Pos;
	memcpy(&Player_Pos, &m_pPlayerMatrix->m[3][0], sizeof(_vec3));

	_matrix		matTrans, matParent, matRotX, matRotY;;
	_vec3		vecEye;

	D3DXMatrixTranslation(&matTrans, m_fPlayerRatio[0], m_fPlayerRatio[1], m_fPlayerRatio[2]);
	D3DXMatrixTranslation(&matParent, Player_Pos.x, Player_Pos.y, Player_Pos.z);
	D3DXMatrixRotationY(&matRotX, D3DXToRadian(m_fCamAngle[Engine::ANGLE_X]));
	D3DXMatrixRotationX(&matRotY, D3DXToRadian(m_fCamAngle[Engine::ANGLE_Y]));

	// 플레이어 주위 공전
	D3DXVec3TransformCoord(&vecEye, &D3DXVECTOR3(0.f, 0.f, 0.f), &(matTrans * matRotY * matRotX * matParent));

	D3DXVec3TransformCoord(&vecEye, &_vec3(0.f, 0.f, 0.f), &(matTrans * matRotY * matRotX * matParent));

	m_vAt = Player_Pos;
	m_vEye = vecEye;
}

CVictory_Camera* CVictory_Camera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CVictory_Camera* pCamera = new CVictory_Camera(pGraphicDev);
	if (FAILED(pCamera->Ready_GameObject()))
	{
		MSG_BOX(L"CVictory_Camera Create Failed - CVictory_Camera::Create()");
		Engine::Safe_Release(pCamera);
	}

	return pCamera;
}

void CVictory_Camera::Free(void)
{
	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pPlayerObserver);
	Engine::Safe_Release(m_pPlayerObserver);

	Engine::Safe_Release(m_pInputDev);
	Engine::CCamera::Free();
}