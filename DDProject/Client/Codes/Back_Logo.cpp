#include "stdafx.h"
#include "Back_Logo.h"
#include "Renderer.h"

USING(Engine)

CBack_Logo::CBack_Logo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_iRand(0)
{

}


CBack_Logo::~CBack_Logo()
{
}

HRESULT CBack_Logo::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(2.0, 2.f, 1.f));

	m_iRand = rand() % 18;

	return NOERROR;
}

int CBack_Logo::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_PRIORITY, this);

	return 0;
}

void CBack_Logo::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
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
	pEffect->BeginPass(0);
	
	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();	
	pEffect->End();

	pEffect->Release();
}

HRESULT CBack_Logo::Add_Component(void)
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
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_LOGO, L"Com_Texture_Back", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CBack_Logo::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");	

	_matrix			matTmp;
	pEffect->SetMatrix("g_matView", D3DXMatrixIdentity(&matTmp));
	pEffect->SetMatrix("g_matProj", D3DXMatrixIdentity(&matTmp));
	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", m_iRand);

	return NOERROR;
}

CBack_Logo * CBack_Logo::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CBack_Logo* pInstance = new CBack_Logo(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CBack_Logo Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CBack_Logo::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBufferCom);	

	Engine::CGameObject::Free();
}
