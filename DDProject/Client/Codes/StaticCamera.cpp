#include "stdafx.h"
#include "StaticCamera.h"
#include "InfoSubject.h"
#include "InputDev.h"
#include "PlayerObserver.h"
#include "DataManager.h"
#include "Mouse_Manager.h"
#include "MousePointer.h"

CStaticCamera::CStaticCamera(LPDIRECT3DDEVICE9 pGraphicDev)
: Engine::CCamera(pGraphicDev)
, m_pPlayerObserver(nullptr)
, m_pInputDev(Engine::CInputDev::GetInstance())
, m_pPlayerMatrix(nullptr)
, m_fCameraAngle(0.f)
, m_bShakeCam(false)
, m_fShakeTime(0.f)
, m_fShakePower(0.f)
, m_fTime(0.f)
, m_bFixMouse(false)
{
	m_pInputDev->Add_Ref();
}

CStaticCamera::~CStaticCamera()
{
}

void CStaticCamera::Set_CameraShake(const float & fShakePower, const float& fShakeTime, const _int iShakeCount)
{
	if(TRUE == m_bShakeCam)
		return;
	m_bShakeCam = true;
	m_fShakePower = fShakePower / 10000.f; // 데미지 100 받으면 0.01만큼 흔듬
	m_fShakeTime = fShakeTime;
	m_iShakeCount = iShakeCount;

	m_fTime = 0.f;
}

HRESULT CStaticCamera::Ready_GameObject()
{
	m_pPlayerObserver = CPlayerObserver::Create();
	Engine::CInfoSubject::GetInstance()->Subscribe_Observer(m_pPlayerObserver);

	m_vEye = _vec3(0.f, 10.f, -20.f);
	m_vAt = _vec3(0.f, 0.f, 0.f);
	m_vUp = _vec3(0.f, 1.f, 0.f);

	m_fFovy = D3DXToRadian(60.f);
	m_fAspect = float(WINCX) / WINCY;
	m_fNear = PLANE_NEAR;
	m_fFar = PLANE_FAR;

	float fCameraPoisitionComparisonToPlayer[] = { 8.f, 25.f, -60.f }; // 플레이어 대비 카메라의 위치
	memcpy(m_fPlayerRatio, fCameraPoisitionComparisonToPlayer, sizeof(float) * 3);

	Engine::CCamera::Ready_GameObject();

	return NOERROR;
}

void CStaticCamera::Change_Camera(_uint uNum, float fAttachTime)
{
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

	//if (nullptr == m_pPlayerMatrix)
	{
		const PLAYER_INFO* pInfo = m_pPlayerObserver->Get_PlayerInfo(g_iClientIndex);

		if (nullptr != pInfo)
		{
			m_pPlayerMatrix = pInfo->pTransform->Get_WorldMatrix();
			m_fCameraAngle = pInfo->fCameraAngle;
		}
		else
			return;
	}
		
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

_int CStaticCamera::Update_GameObject(const float & fTimeDelta)
{
	const PLAYER_INFO* pInfo = m_pPlayerObserver->Get_PlayerInfo(g_iClientIndex);

	if (nullptr != pInfo)
	{
		m_pPlayerMatrix = pInfo->pTransform->Get_WorldMatrix();
		m_fCameraAngle = pInfo->fCameraAngle;
	}

	if (m_bFixMouse)
	{
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

	ShakeCamera(fTimeDelta);

	return Engine::CCamera::Update_GameObject(fTimeDelta);
}

void CStaticCamera::KeyCheck(const float & fTimeDelta)
{
	if (m_pInputDev->OnceKeyDown(DIK_Q))
		m_bFixMouse = (m_bFixMouse == TRUE) ? FALSE : TRUE;
}

void CStaticCamera::MouseFix(void)
{
	POINT		ptMouse = { WINCX >> 1, WINCY >> 1 };
	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void CStaticCamera::TargetRenewal(const float& fTimeDelta)
{
	if (nullptr == m_pPlayerMatrix)
		return;

	_vec3 Player_Pos, Player_Right, Player_Look;
	memcpy(&Player_Right, &m_pPlayerMatrix->m[0][0], sizeof(_vec3));
	memcpy(&Player_Look, &m_pPlayerMatrix->m[2][0], sizeof(_vec3));
	memcpy(&Player_Pos, &m_pPlayerMatrix->m[3][0], sizeof(_vec3));

	D3DXVec3Normalize(&Player_Right, &Player_Right);
	D3DXVec3Normalize(&Player_Look, &Player_Look);

	_matrix		matTrans, matParent, matRotX;
	_vec3		vecEye;

	_vec3		vecTemp = { 0.f, 0.f, 0.f };

	vecTemp += Player_Right * m_fPlayerRatio[0] + Player_Look * m_fPlayerRatio[2];

	D3DXMatrixTranslation(&matTrans, vecTemp.x, vecTemp.y + m_fPlayerRatio[1] , vecTemp.z);
	D3DXMatrixTranslation(&matParent, Player_Pos.x, Player_Pos.y, Player_Pos.z);

	D3DXMatrixRotationAxis(&matRotX, &Player_Right, D3DXToRadian(m_fCameraAngle));
	D3DXVec3TransformNormal(&Player_Look, &Player_Look, &matRotX);

	D3DXVec3TransformCoord(&vecEye, &_vec3(0.f, 0.f, 0.f), &(matTrans * matRotX * matParent));

	m_vAt = vecEye + Player_Look;
	m_vEye = vecEye;
}

void CStaticCamera::ShakeCamera(const float & fTimeDelta)
{
	if (FALSE == m_bShakeCam)
		return;

	// m_fShakeTime동안 지속
	m_fTime += fTimeDelta;
	if (m_fTime > m_fShakeTime)
		m_bShakeCam = false;

	float fAngle = m_fTime * 180.f / m_fShakeTime * m_iShakeCount;  

	float fTemp2 = fabs(m_fShakePower * sin(D3DXToRadian(fAngle)));

	_vec3 Player_Right;
	memcpy(&Player_Right, &m_pPlayerMatrix->m[0][0], sizeof(_vec3));
	D3DXVec3Normalize(&Player_Right, &Player_Right);

	m_vEye += _vec3(Player_Right.x * fTemp2, Player_Right.y * fTemp2, Player_Right.z * fTemp2);
}

CStaticCamera* CStaticCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStaticCamera* pCamera = new CStaticCamera(pGraphicDev);
	if (FAILED(pCamera->Ready_GameObject()))
	{
		MSG_BOX(L"CStaticCamera Create Failed - CStaticCamera::Create()");
		Engine::Safe_Release(pCamera);
	}

	return pCamera;
}

void CStaticCamera::Free(void)
{
	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pPlayerObserver);
	Engine::Safe_Release(m_pPlayerObserver);

	Engine::Safe_Release(m_pInputDev);
	Engine::CCamera::Free();
}