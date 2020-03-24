#include "stdafx.h"
#include "RecordCamera.h"

#include "InputDev.h"
#include "Mouse_Manager.h"

CRecordCamera::CRecordCamera(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CCamera(pGraphicDev)
	, m_fCamSpeed(0.f)
	, m_fRecordTime(0.f)
	, m_bRecordCamera(FALSE)
	, m_bRecording(FALSE)
	, m_vDir(D3DXVECTOR3(0.f, 0.f, 1.f))
	, m_pInputDev(Engine::CInputDev::GetInstance())
	, m_pRendererCom(nullptr)
{
	ZeroMemory(m_fAngle, sizeof(float) * 3);

	m_pInputDev->Add_Ref();
}

CRecordCamera::~CRecordCamera(void)
{
}

HRESULT CRecordCamera::Ready_GameObject()
{
	m_fCamSpeed = 100.f;

	m_vEye = _vec3(0.f, 0.f, 0.f);
	m_vAt = _vec3(0.f, 0.f, 0.f);
	m_vUp = _vec3(0.f, 1.f, 0.f);

	m_fFovy = D3DXToRadian(60.f);
	m_fAspect = float(WINCX) / WINCY;
	m_fNear = PLANE_NEAR;
	m_fFar = PLANE_FAR;

	// 녹화

	// 폰트 Rect
	RECT rc = { WINCX / 12, WINCY / 12, rc.left + 800, rc.top + 800 };
	m_FontRect = rc;

	RECT rc2 = { WINCX / 2 + 200, WINCY / 12, rc2.left + 800, rc2.top + 800 };
	m_TimeRect = rc2;

	// 폰트
	D3DXFONT_DESC		hFont;
	ZeroMemory(&hFont, sizeof(D3DXFONT_DESC));
	hFont.Width = 50;
	hFont.Height = 50;
	hFont.Weight = FW_NORMAL;
	lstrcpy(hFont.FaceName, L"Calibri");
	hFont.CharSet = HANGEUL_CHARSET;

	HRESULT hr = D3DXCreateFontIndirect(m_pGraphicDev, &hFont, &m_pD3DXFont);

	lstrcpy(m_szRecordState, L"녹화준비");
	lstrcpy(m_szRecordTime, L"0");

	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return S_OK;
}

void CRecordCamera::Change_Camera(_uint uNum, float fAttachTime)
{
	Initialize_Camera();
	m_bCamAttach = false;
}
_int CRecordCamera::Update_GameObject(const float & fTimeDelta)
{
	KeyCheck(fTimeDelta);
	MouseMove(fTimeDelta);

	m_vAt = m_vEye + m_vDir; // 카메라의 방향을 바라본다.

	// 녹화
	RecordKey();
	Record(fTimeDelta);

	Engine::CCamera::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	return 0;
}

void CRecordCamera::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	// 폰트출력 
	m_pD3DXFont->DrawTextW(nullptr, m_szRecordState, lstrlen(m_szRecordState), &m_FontRect, NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
	m_pD3DXFont->DrawTextW(nullptr, m_szRecordTime, lstrlen(m_szRecordTime), &m_TimeRect, NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
}

HRESULT CRecordCamera::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	pComponent = m_pRendererCom = ((Engine::CRenderer*)m_pComponentMgr->Clone_Component(SCENE_STATIC, L"Com_Renderer"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Render", pComponent));
	m_pRendererCom->Add_Ref();

	return NOERROR;
}

void CRecordCamera::KeyCheck(const float & fTimeDelta)
{
	D3DXVECTOR3 vecDir = { 0.f, 0.f, 0.f };

	// 앞
	if (m_pInputDev->StayKeyDown(DIK_W))
	{
		vecDir = m_vDir;
		m_vEye += fTimeDelta * m_fCamSpeed * vecDir;
	}

	// 뒤
	if (m_pInputDev->StayKeyDown(DIK_S))
	{
		vecDir = -m_vDir;
		m_vEye += fTimeDelta * m_fCamSpeed * vecDir;
	}

	// 왼쪽
	if (m_pInputDev->StayKeyDown(DIK_A))
	{
		D3DXVec3Cross(&vecDir, &m_vDir, &D3DXVECTOR3(0.f, 1.f, 0.f));
		m_vEye += fTimeDelta * m_fCamSpeed * vecDir;
	}

	// 오른쪽
	if (m_pInputDev->StayKeyDown(DIK_D))
	{
		D3DXVec3Cross(&vecDir, &D3DXVECTOR3(0.f, 1.f, 0.f), &m_vDir);
		m_vEye += fTimeDelta * m_fCamSpeed * vecDir;
	}

	// 위
	if (m_pInputDev->StayKeyDown(DIK_UP))
	{
		vecDir = D3DXVECTOR3(0.f, 1.f, 0.f);
		m_vEye += fTimeDelta * m_fCamSpeed * vecDir;
	}

	// 아래
	if (m_pInputDev->StayKeyDown(DIK_DOWN))
	{
		vecDir = D3DXVECTOR3(0.f, -1.f, 0.f);
		m_vEye += fTimeDelta * m_fCamSpeed * vecDir;
	}

	// 카메라 속도
	if (m_pInputDev->StayKeyDown(DIK_RIGHT))
		m_fCamSpeed += fTimeDelta * 30.f;

	if (m_pInputDev->StayKeyDown(DIK_LEFT))
		m_fCamSpeed -= fTimeDelta * 30.f;

	
}

void CRecordCamera::MouseMove(const float & fTimeDelta)
{
	D3DXVECTOR3 vecMouseGap = Engine::CMouse_Manager::GetInstance()->GetMouseRelativeGap();

	// 마우스 x위치의 차이로 카메라의 y축을 회전시킨다 
	m_fAngle[Engine::ANGLE_Y] += vecMouseGap.x * fTimeDelta * 0.5f;

	// 마우스 y위치의 차이로 카메라의 x축을 회전시킨다.
	m_fAngle[Engine::ANGLE_X] += vecMouseGap.y * fTimeDelta * 0.5f;

	D3DXMATRIX matRotX, matRotY;

	m_vDir = _vec3(0.f, 0.f, 1.f);

	D3DXMatrixRotationX(&matRotX, D3DXToRadian(m_fAngle[Engine::ANGLE_X]));
	D3DXMatrixRotationY(&matRotY, D3DXToRadian(m_fAngle[Engine::ANGLE_Y]));

	D3DXVec3TransformNormal(&m_vDir, &m_vDir, &matRotX);
	D3DXVec3TransformNormal(&m_vDir, &m_vDir, &matRotY);

}

void CRecordCamera::RecordKey(void)
{
	// 녹화 시작
	if (m_pInputDev->OnceKeyDown(DIK_F2) && FALSE == m_bRecording)
	{
		m_bRecording = true;
		lstrcpy(m_szRecordState, L"녹화중");

		if (m_fRecordTime == 0.f) // 녹화 처음 시작
		{
			time_t timer = time(NULL);
			tm t;
			localtime_s(&t, &timer);

			TCHAR szFileName[64];
			wsprintf(szFileName, L"../Resources/Data/CamData/%d월%d일%d시%d분%d초.dat", t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

			m_hFile = CreateFile(szFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}
	}

	// 녹화 일시 중지
	if (m_pInputDev->OnceKeyDown(DIK_F3) && TRUE == m_bRecording)
	{
		m_bRecording = false;

		lstrcpy(m_szRecordState, L"일시중지");
	}

	// 녹화 종료
	if (m_pInputDev->OnceKeyDown(DIK_F4) && TRUE == m_bRecording)
	{
		m_bRecording = false;
		m_fRecordTime = 0.f;

		lstrcpy(m_szRecordState, L"녹화 종료");

		CloseHandle(m_hFile);
	}
}

void CRecordCamera::Record(const float & fTimeDelta)
{
	if (!m_bRecording)
		return;

	m_fRecordTime += fTimeDelta; // 녹화 시간
	wsprintf(m_szRecordTime, L"%d", int(m_fRecordTime));

	DWORD dwByte;
	CAM_INFO CamInfo = { m_vEye, m_vAt };

	WriteFile(m_hFile, &CamInfo, sizeof(CAM_INFO), &dwByte, NULL);
}

CRecordCamera* CRecordCamera::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CRecordCamera*		pCamera = new CRecordCamera(pGraphicDev);
	if (FAILED(pCamera->Ready_GameObject()))
		Engine::Safe_Delete(pCamera);

	return pCamera;
}

void CRecordCamera::Free(void)
{
	CCamera::Free();

	Engine::Safe_Release(m_pInputDev);
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pD3DXFont);

	//CloseHandle(m_hFile);
}
