#include "stdafx.h"
#include "TowerRange2.h"
#include "Renderer.h"

CTowerRange2::CTowerRange2(LPDIRECT3DDEVICE9 pGraphicDev, int iTextureNum)
	: CFont_Perspective(pGraphicDev)
	, m_iTextureNum(iTextureNum)
{
}


CTowerRange2::~CTowerRange2()
{
}

HRESULT CTowerRange2::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(32.f, 32.f, 32.f));
	m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), 0.f, 0.f)); // ´¯È÷±â

	return NOERROR;
}

int CTowerRange2::Update_GameObject(const float & fTimeDelta)
{
	if (m_bDead)
		return 1;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DEFAULT, this);

	return 0;
}

void CTowerRange2::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == m_pShaderCom)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	pEffect->AddRef();

	Set_ConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(UI_DARKREMOVECOLORMULTIPLY);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CTowerRange2::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Shader
	AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_UI", Engine::CComponent::COM_STATIC, L"Com_Shader");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_TowerRange2", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CTowerRange2::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", m_iTextureNum);

	pEffect->SetVector("g_fColor", &m_vecColor);

	return NOERROR;
}

CTowerRange2 * CTowerRange2::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iTextureNum)
{
	CTowerRange2* pInstance = new CTowerRange2(pGraphicDev, iTextureNum);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CTowerRange2 Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CTowerRange2::Free(void)
{
	CFont_Perspective::Free();
}
