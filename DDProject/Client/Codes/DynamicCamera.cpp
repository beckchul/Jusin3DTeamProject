#include "stdafx.h"
#include "DynamicCamera.h"
#include "InputDev.h"
#include "Mouse_Manager.h"
#include "MousePointer.h"

CDynamicCamera::CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
	, m_fCamSpeed(0.f)
	, m_bFixMouse(true)
	, m_pInputDev(Engine::CInputDev::GetInstance())
{
	m_pInputDev->Add_Ref();
}


CDynamicCamera::~CDynamicCamera(void)
{
}

HRESULT CDynamicCamera::Ready_GameObject()
{
	m_vEye = _vec3(0.0f, 10.f, -20.f);
	m_vAt = _vec3(0.f, 0.f, 0.f);
	m_vUp = _vec3(0.f, 1.f, 0.f);

	m_fFovy = D3DXToRadian(60.f);
	m_fAspect = float(WINCX) / WINCY;
	m_fNear = PLANE_NEAR;
	m_fFar = PLANE_FAR;

	m_fCamSpeed = 100.f;

	Engine::CCamera::Ready_GameObject();

	return S_OK;
}

void CDynamicCamera::Change_Camera(_uint uNum, float fAttachTime)
{
	Initialize_Camera();
	m_bCamAttach = true;

	Engine::CGameObject* pMousePointer = CMouse_Manager::GetInstance()->Get_MousePointer();
	if (nullptr == pMousePointer)
		return;

	dynamic_cast<CMousePointer*>(pMousePointer)->Set_ShowMousePointer(false);
}

int CDynamicCamera::Update_GameObject(const float & fTimeDelta)
{
	KeyCheck(fTimeDelta);

	if (m_bFixMouse)
	{
		MouseFix();
		MouseMove();
	}

	return Engine::CCamera::Update_GameObject(fTimeDelta);
}

void CDynamicCamera::KeyCheck(const float & fTimeDelta)
{
	if (m_pInputDev->OnceKeyDown(DIK_Q))
	{
		m_bFixMouse = (m_bFixMouse == TRUE) ? FALSE : TRUE;
	}

	if (m_bFixMouse == false)
		return;

	if (m_pInputDev->StayKeyDown(DIK_A))
	{
		D3DXVECTOR3		vRight;
		D3DXMATRIX		matCamState;
		D3DXMatrixInverse(&matCamState, NULL, &m_matView);
		memcpy(&vRight, &matCamState.m[0][0], sizeof(D3DXVECTOR3));
		D3DXVec3Normalize(&vRight, &vRight);

		m_vEye -= vRight * m_fCamSpeed * fTimeDelta;
		m_vAt -= vRight * m_fCamSpeed * fTimeDelta;
	}

	if (m_pInputDev->StayKeyDown(DIK_D))
	{
		D3DXVECTOR3		vRight;
		D3DXMATRIX		matCamState;
		D3DXMatrixInverse(&matCamState, NULL, &m_matView);
		memcpy(&vRight, &matCamState.m[0][0], sizeof(D3DXVECTOR3));
		D3DXVec3Normalize(&vRight, &vRight);

		m_vEye += vRight * m_fCamSpeed * fTimeDelta;
		m_vAt += vRight * m_fCamSpeed * fTimeDelta;
	}

	if (m_pInputDev->StayKeyDown(DIK_W))
	{
		D3DXVECTOR3		vLook = m_vAt - m_vEye;
		D3DXVec3Normalize(&vLook, &vLook);

		m_vEye += vLook * m_fCamSpeed * fTimeDelta;
		m_vAt += vLook * m_fCamSpeed * fTimeDelta;
	}

	if (m_pInputDev->StayKeyDown(DIK_S))
	{
		D3DXVECTOR3		vLook = m_vAt - m_vEye;
		D3DXVec3Normalize(&vLook, &vLook);

		m_vEye -= vLook * m_fCamSpeed * fTimeDelta;
		m_vAt -= vLook * m_fCamSpeed * fTimeDelta;
	}
}

void CDynamicCamera::MouseMove(void)
{
	int		iDistance = 0;
	if (iDistance = m_pInputDev->GetDIMouseMove(Engine::CInputDev::DIM_X))
	{
		D3DXMATRIX		matAxis;
		D3DXMatrixRotationAxis(&matAxis, &D3DXVECTOR3(0.f, 1.f, 0.f), D3DXToRadian(iDistance / 10.f));

		D3DXVECTOR3		vDir = m_vAt - m_vEye;
		D3DXVec3TransformNormal(&vDir, &vDir, &matAxis);

		m_vAt = m_vEye + vDir;
	}

	if (iDistance = m_pInputDev->GetDIMouseMove(Engine::CInputDev::DIM_Y))
	{
		D3DXVECTOR3		vRight;
		D3DXMATRIX		matCamState;
		D3DXMatrixInverse(&matCamState, NULL, &m_matView);
		memcpy(&vRight, &matCamState.m[0][0], sizeof(D3DXVECTOR3));
		D3DXVec3Normalize(&vRight, &vRight);

		D3DXMATRIX		matAxis;
		D3DXMatrixRotationAxis(&matAxis, &vRight, D3DXToRadian(iDistance / 10.f));

		D3DXVECTOR3		vDir = m_vAt - m_vEye;
		D3DXVec3TransformNormal(&vDir, &vDir, &matAxis);

		m_vAt = m_vEye + vDir;
	}
}

void CDynamicCamera::MouseFix(void)
{
	POINT		ptMouse = { WINCX >> 1, WINCY >> 1 };
	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

CDynamicCamera* CDynamicCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CDynamicCamera* pCamera = new CDynamicCamera(pGraphicDev);
	if (FAILED(pCamera->Ready_GameObject()))
	{
		MSG_BOX(L"CDynamicCamera Create Failed - CDynamicCamera::Create()");
		Engine::Safe_Release(pCamera);
	}

	return pCamera;
}

void CDynamicCamera::Free(void)
{
	Engine::Safe_Release(m_pInputDev);
	Engine::CCamera::Free();
}
