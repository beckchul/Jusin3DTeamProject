#include "stdafx.h"
#include "Back_Logo.h"
#include "Renderer.h"

USING(Engine)

CBack_Logo::CBack_Logo(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pTextureCom(nullptr)
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

	m_pTransformCom->Set_Infomation(Engine::CTransform::INFO_SCALE, &_vec3(2.0, 2.f, 1.f));

	return NOERROR;
}

int CBack_Logo::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_PRIORITY, this);

	return 0;
}

void CBack_Logo::Render_GameObject(void)
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

	Safe_Release(pEffect);
}

HRESULT CBack_Logo::Add_Component(void)
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

	// For.Transform
	pComponent = m_pTransformCom = ((Engine::CTransform*)m_pComponentMgr->Clone_Component(SCENE_STATIC, L"Com_Transform"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::COM_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", pComponent));
	m_pTransformCom->Add_Ref();

	// For.Shader
	pComponent = m_pShaderCom = ((Engine::CShader*)m_pComponentMgr->Clone_Component(SCENE_LOGO , L"Com_Shader_Back"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->Add_Ref();

	// For.Texture
	pComponent = m_pTextureCom = ((Engine::CTexture*)m_pComponentMgr->Clone_Component(SCENE_LOGO, L"Com_Texture_Back"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->Add_Ref();

	// For.Buffer
	pComponent = m_pBufferCom = ((Engine::CRect_Texture*)m_pComponentMgr->Clone_Component(SCENE_STATIC, L"Com_Buffer_RcTex"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->Add_Ref();	

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
	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture");

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
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBufferCom);	

	Engine::CGameObject::Free();
}
