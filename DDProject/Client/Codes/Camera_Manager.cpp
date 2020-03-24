#include "stdafx.h"
#include "Camera_Manager.h"
#include "Camera.h"
#include "InputDev.h"
#include "StaticCamera.h"
#include "RecordCamera.h"
#include "ActionCamera.h"
#include "TowerCamera.h"
#include "Goblin_Camera.h"
#include "FrustumManager.h"

IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
	: m_eCamType(CAMERA_STATIC)
	, m_pInputDev(CInputDev::GetInstance())
	, m_pFrustumMgr(CFrustumManager::GetInstance())
{
	ZeroMemory(m_ArrCamera, sizeof(m_ArrCamera));

	m_pInputDev->Add_Ref();
}

CCamera_Manager::~CCamera_Manager()
{
}

int CCamera_Manager::Update_Camara(const float& fTimeDelta)
{
#ifndef Demonstration
	KeyCheck();
#endif
		
	if (nullptr == m_ArrCamera[m_eCamType])
		return 0;

	m_ArrCamera[m_eCamType]->Update_GameObject(fTimeDelta);

	Update_Frustum();

	return 0;
}

HRESULT CCamera_Manager::Add_Camera(Engine::CGameObject * pGameObject, CAMERA_TYPE eCamType)
{
	m_ArrCamera[eCamType] = dynamic_cast<CCamera*>(pGameObject);

	return NOERROR;
}

void CCamera_Manager::Change_Camera(CAMERA_TYPE eCamType, _uint uCutSceneNum, _float fAttachTime, bool bForceChange)
{
	if (m_eCamType == eCamType && FALSE == bForceChange)
		return;

	m_eCamType = eCamType;
	if (nullptr == m_ArrCamera[m_eCamType])
	{
		MSG_BOX(L"없는 카메라 입니다.");
		return;
	}
	m_ArrCamera[m_eCamType]->Change_Camera(uCutSceneNum, fAttachTime); // 카메라 바뀌었을 때 이벤트
}

void CCamera_Manager::Fix_MouseCursor(bool b)
{
	if (CAMERA_TOWER == m_eCamType)
	{
		dynamic_cast<CTowerCamera*>(m_ArrCamera[m_eCamType])->Set_FixCursor(b);
	}
}

void CCamera_Manager::Shake_Camera(const float & fShakePower, const float& fShakeTime, const _int iShakeCount)
{
	if (CAMERA_STATIC == m_eCamType)
		dynamic_cast<CStaticCamera*>(m_ArrCamera[m_eCamType])->Set_CameraShake(fShakePower, fShakeTime, iShakeCount);
	else if(CAMERA_GOBLIN == m_eCamType)
		dynamic_cast<CGoblin_Camera*>(m_ArrCamera[m_eCamType])->Set_CameraShake(fShakePower, fShakeTime, iShakeCount);
}

bool CCamera_Manager::Get_IsCamAttach(void)
{
	if (nullptr == m_ArrCamera[m_eCamType])
		return false;

	return m_ArrCamera[m_eCamType]->Get_CamAttach();
}

void CCamera_Manager::Reset_Camera(void)
{
	for (int i = 0; i < CAMERA_END; ++i)
	{
		Engine::Safe_Release(m_ArrCamera[i]);
	}
}

void CCamera_Manager::KeyCheck(void)
{
	//// Dynamic Camera
	//if (m_pInputDev->OnceKeyDown(DIK_F))
	//{
	//	if (CAMERA_DYNAMIC != m_eCamType)
	//	{
	//		m_eCamType = CAMERA_DYNAMIC;
	//		m_ArrCamera[m_eCamType]->Initialize_Camera();
	//	}
	//	else
	//		m_eCamType = CAMERA_STATIC;
	//}

	//// Record Camera
	//if (m_pInputDev->OnceKeyDown(DIK_H))
	//{
	//	if (CAMERA_RECORDING != m_eCamType)
	//	{
	//		m_eCamType = CAMERA_RECORDING;
	//		m_ArrCamera[m_eCamType]->Initialize_Camera();
	//	}
	//	else
	//		m_eCamType = CAMERA_STATIC;
	//}

	//// Action Camera
	//if (m_pInputDev->OnceKeyDown(DIK_C))
	//{
	//	if (CAMERA_ACTION != m_eCamType)
	//	{
	//		m_eCamType = CAMERA_ACTION;
	//		Change_Camera(CAMERA_ACTION, 0); // 첫 번째 컷씬
	//	}
	//}
}

void CCamera_Manager::Update_Frustum(void)
{
	_matrix matView, matProj;

	matView = *m_ArrCamera[m_eCamType]->Get_ViewMatrix();
	matProj = *m_ArrCamera[m_eCamType]->Get_ProjMatrix();

	m_pFrustumMgr->MakeFrustumPlane(matView, matProj);
}

void CCamera_Manager::Free(void)
{
	for (int i = 0; i < CAMERA_END; ++i)
	{
		Engine::Safe_Release(m_ArrCamera[i]);
	}

	Engine::Safe_Release(m_pInputDev);
}
