#include "stdafx.h"
#include "MousePointer.h"
#include "Renderer.h"
#include "InputDev.h"

USING(Engine)

CMousePointer::CMousePointer(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
	, m_pMouseMgr(CMouse_Manager::GetInstance())
	, m_eMousePointer(CMouse_Manager::POINTER_END)
	, m_vecColor(0.f, 1.f, 0.f, 1.f)
	, m_bShowMouse(true)
	, m_pTextureCom2(nullptr)
{
	ZeroMemory(&m_TowerPosition, sizeof(_vec3));
	// Mouse_Manager에서 이 클래스를 해제하기 때문에 여기서 Add_ref를 하면 Mouse_Manager를 Free할 수 없게 됨(상호 참조하고 있는 상태)
	//m_pMouseMgr->Add_Ref();
}


CMousePointer::~CMousePointer()
{
}

void CMousePointer::Set_TowerPosition(_vec3 & vecPos, POINT& ptDir)
{
	m_TowerPosition = vecPos;

	// 마우스 포인터를 약간 타워의 dir으로 좀 더 이동시킨다.
	POINT		ptMouse = { _long(m_pMouseMgr->GetMousePos().x + ptDir.x), 
		_long(m_pMouseMgr->GetMousePos().y + ptDir.y)}; // 마우스 커서 이동

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

HRESULT CMousePointer::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 128.f , SCALE_WINCY * 128.f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(WINCX >> 1, WINCY >> 1, 0.f));

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	m_eMousePointer = CMouse_Manager::POINTER_AIM;

	return NOERROR;
}

int CMousePointer::Update_GameObject(const float & fTimeDelta)
{
	Rotate_By_Tower();

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if(CMouse_Manager::POINTER_AIM == m_eMousePointer)
		m_pTransformCom->Change_Information(CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(90.f * fTimeDelta)));

	_vec3 vecMouse = m_pMouseMgr->GetMousePos();
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(vecMouse.x - WINCX / 2.f, -vecMouse.y + WINCY / 2.f, 0.f));
	
	if (nullptr != m_pRendererCom && m_bShowMouse)
	{
		if(g_eCurrentScene != SCENE_LOBBY)
			m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);
	}
		
	return 0;
}

void CMousePointer::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	pArgEffect->CommitChanges();

	switch (m_eMousePointer)
	{
	case Engine::CMouse_Manager::POINTER_AIM:
		pArgEffect->BeginPass(UI_ALPHATESETBLEND);
		break;
	case Engine::CMouse_Manager::POINTER_BUILD:
	case Engine::CMouse_Manager::POINTER_REPAIR:
	case Engine::CMouse_Manager::POINTER_ARROW:
		pArgEffect->BeginPass(UI_COLORMULTIPLY);
		break;
	}

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->SetVector("g_fColor", &_vec4(1.f, 1.f, 1.f, 1.f)); // 색상 원복

	pArgEffect->EndPass();
}

void CMousePointer::Change_MousePointer(CMouse_Manager::eMousePointer ePointer)
{
	m_eMousePointer = ePointer;
}

HRESULT CMousePointer::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Mouse", Engine::CComponent::COM_STATIC, L"Com_Texture");
	AddComponent(m_pTextureCom2, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Mouse2", Engine::CComponent::COM_STATIC, L"Com_Texture2");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CMousePointer::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	if(Engine::CMouse_Manager::POINTER_ARROW == m_eMousePointer)
		m_pTextureCom2->SetUp_OnShader(pEffect, "g_BaseTexture", 0);
	else
		m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", (_uint)m_eMousePointer);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	pEffect->SetVector("g_fColor", &m_vecColor);

	return NOERROR;
}

void CMousePointer::Rotate_By_Tower(void)
{
	if (Engine::CMouse_Manager::POINTER_ARROW != m_eMousePointer)
		return;

	_vec3 vecDir = m_pMouseMgr->GetMousePos() - m_TowerPosition;

	m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, atan2f(-vecDir.y, vecDir.x) + D3DXToRadian(-90.f)));
}

CMousePointer * CMousePointer::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMousePointer* pInstance = new CMousePointer(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CMousePointer Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CMousePointer::Free(void)
{
	// Mouse_Manager에서 이 클래스를 해제하기 때문에 여기서 Add_ref를 하면 Mouse_Manager를 Free할 수 없게 됨(상호 참조하고 있는 상태)
	//Engine::Safe_Release(m_pMouseMgr);

	Engine::Safe_Release(m_pTextureCom2);

	COrthographicUI::Free();
}
