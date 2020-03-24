#include "Camera.h"
#include "DataManager.h"

USING(Engine)

Engine::CCamera::CCamera(LPDIRECT3DDEVICE9 pGraphicDev)
: Engine::CGameObject(pGraphicDev)
, m_pDataMgr(CDataManager::GetInstance())
, m_bCamAttach(true)
, m_vEye(0.f, 0.f, 0.f)
, m_vAt(0.f, 0.f, 0.f)
, m_vUp(0.f, 0.f, 0.f)
, m_vDir(0.f, 0.f, 0.f)
, m_fTime(0.f)
{
	m_pDataMgr->Add_Ref();
}

Engine::CCamera::~CCamera(void)
{

}

HRESULT Engine::CCamera::Ready_GameObject(void)
{
	// For.View Matrix
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	// For.Projection Matrix
	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFovy, m_fAspect, m_fNear, m_fFar);

	m_pGraphicDev->SetTransform(D3DTS_PROJECTION, &m_matProj);

	m_pDataMgr->Set_ViewMatrix(m_matView);
	m_pDataMgr->Set_ProjMatrix(m_matProj);

	_matrix matViewInv;
	D3DXMatrixInverse(&matViewInv, nullptr, &m_matView);
	m_pDataMgr->Set_ViewInverseMatrix(matViewInv);
	m_pDataMgr->Set_CamPoistion(m_vEye);

	return S_OK;
}

_int Engine::CCamera::Update_GameObject(const _float& fTimeDelta)
{	
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);

	m_pGraphicDev->SetTransform(D3DTS_VIEW, &m_matView);

	_matrix matViewInv; 
	D3DXMatrixInverse(&matViewInv, nullptr, &m_matView);

	m_pDataMgr->Set_ViewMatrix(m_matView);
	m_pDataMgr->Set_ViewInverseMatrix(matViewInv);
	m_pDataMgr->Set_CamPoistion(m_vEye);

	return 0;
}

void Engine::CCamera::Initialize_Camera()
{
	D3DXMATRIX matViewInv = m_pDataMgr->Get_ViewInverseMatrix();

	D3DXVECTOR3 vecEye = { 0.f, 0.f, 0.f };
	D3DXVECTOR3 vecDir = { 0.f, 0.f, 1.f };

	D3DXVec3TransformCoord(&m_vEye, &vecEye, &matViewInv);
	D3DXVec3TransformNormal(&m_vDir, &vecDir, &matViewInv);

	D3DXVec3Normalize(&m_vDir, &m_vDir);

	m_vAt = m_vEye + m_vDir;
	m_fTime = 0.f;
}

void CCamera::Flow_Camera(const _float & fTimeDelta, _vec3 & vecDest, _float fDistance, _vec3 & vecDir, _float fAngle, _vec3 & vecAxis, _float fTime)
{
	if (TRUE == m_bCamAttach)
	{
		m_vEye = vecDest;
		m_vDir = vecDir;
	}
	else
	{
		// 카메라 이동
		_vec3 vecDir = vecDest - m_vEye;
		m_fTime += fTimeDelta;
		if (m_fTime > 1 / fTime)
			m_bCamAttach = true;

		D3DXVec3Normalize(&vecDir, &vecDir);
		m_vEye += vecDir * fTimeDelta * fDistance * fTime; // fTime초동안 fDistance만큼 이동
		
		// 카메라 회전
		_matrix matRot;
		D3DXMatrixRotationAxis(&matRot, &vecAxis, fAngle * fTimeDelta * fTime); // fTime초 동안 fAngle만큼 회전
		D3DXVec3TransformNormal(&m_vDir, &m_vDir, &matRot);
	}
}

void Engine::CCamera::Free(void)
{
	Safe_Release(m_pDataMgr);

	Engine::CGameObject::Free();
}


