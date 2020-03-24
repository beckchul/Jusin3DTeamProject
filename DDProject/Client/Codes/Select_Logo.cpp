#include "stdafx.h"
#include "Select_Logo.h"
#include "Renderer.h"
#include "InputDev.h"
#include "InfoSubject.h"

USING(Engine)

CSelect_Logo::CSelect_Logo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_fSelectX(0.f)
	, m_fSelectY(0.f)
	, m_eSceneId(SCENE_END)
{
}

CSelect_Logo::~CSelect_Logo()
{
}

HRESULT CSelect_Logo::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(2.f, 2.f, 1.f));
	
	m_fSelectX = -1.f;

	return NOERROR;
}

int CSelect_Logo::Update_GameObject(const float & fTimeDelta)
{
	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_PRIORITY, this);

	static POINT ptMouse;
	// 현재 마우스 커서의 좌표를 얻어오는 함수.
	GetCursorPos(&ptMouse);

	// 현재 전체 창에 대한 마우스의 좌표를 해당 윈도우 창 영역 안의 좌표로 변환해주는 함수.
	ScreenToClient(g_hWnd, &ptMouse);
	
	if (ptMouse.x <= WINCX / 4.f)
	{
		if (ptMouse.y <= WINCY / 2.f)
		{
			m_fSelectY = 1.f;
			m_eSceneId = TESTSCENE_SUENGHO;
		}
		else
		{
			m_fSelectY = -0.f;
			m_eSceneId = TESTSCENE_SUENGHO2;
		}
		m_fSelectX = -0.5f;
	}
		
	else if (ptMouse.x <= WINCX / 4.f * 2.f)
	{
		if (ptMouse.y <= WINCY / 2.f)
		{
			m_fSelectY = 1.f;
			m_eSceneId = TESTSCENE_SEOCHAN;
		}
		else
		{
			m_fSelectY = 0.f;
			m_eSceneId = TESTSCENE_SEOCHAN2;
		}
		m_fSelectX = 0.f;
	}
		
	else if (ptMouse.x <= WINCX / 4.f * 3.f)
	{
		if (ptMouse.y <= WINCY / 2.f)
		{
			m_fSelectY = 1.f;
			m_eSceneId = TESTSCENE_BACKCHUL;
		}
		else
		{
			m_fSelectY = -0.f;
			m_eSceneId = TESTSCENE_BACKCHUL2;
		}
		m_fSelectX = 0.5f;
	}
		
	else if (ptMouse.x <= WINCX)
	{
		if (ptMouse.y <= WINCY / 2.f)
		{
			m_fSelectY = 1.f;
			m_eSceneId = TESTSCENE_MINKYEONG;
		}
		else
		{
			m_fSelectY = -0.f;
			m_eSceneId = TESTSCENE_MINKYEONG2;
		}
		m_fSelectX = 1.f;
	}
		
	// 좌클릭 하면 이동
	if (Engine::CInputDev::GetInstance()->OnceMouseClick(Engine::CInputDev::DIM_LBUTTON))
		Engine::CInfoSubject::GetInstance()->Add_Data(MESSAGE_SCENECHANGE, &m_eSceneId);

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CSelect_Logo::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == m_pShaderCom)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	if (FAILED(Set_ConstantTable(pEffect)))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(1);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CSelect_Logo::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Shader
	AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_Back", Engine::CComponent::COM_STATIC, L"Com_Shader");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_LOGO, L"Com_Texture_Select", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CSelect_Logo::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matTmp;
	pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matTmp));
	pEffect->SetMatrix("g_matProj", D3DXMatrixIdentity(&matTmp));
	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", 0);

	pEffect->SetFloat("g_fSelectX", m_fSelectX);
	pEffect->SetFloat("g_fSelectY", m_fSelectY);
	
	return NOERROR;
}

CSelect_Logo * CSelect_Logo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSelect_Logo* pInstance = new CSelect_Logo(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CSelect_Logo Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CSelect_Logo::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBufferCom);

	Engine::CInfoSubject::GetInstance()->Remove_Data(MESSAGE_SCENECHANGE, &m_eSceneId);

	Engine::CGameObject::Free();
}
