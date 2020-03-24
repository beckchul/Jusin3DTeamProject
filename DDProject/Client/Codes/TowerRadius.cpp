#include "stdafx.h"
#include "TowerRadius.h"
#include "Renderer.h"

CTowerRadius::CTowerRadius(LPDIRECT3DDEVICE9 pGraphicDev)
	: CFont_Perspective(pGraphicDev)
	, m_fAcc(0.f)
{
}


CTowerRadius::~CTowerRadius()
{
}

HRESULT CTowerRadius::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(96.f, 96.f, 96.f));
	m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), 0.f, 0.f)); // ´¯È÷±â

	return NOERROR;
}

int CTowerRadius::Update_GameObject(const float & fTimeDelta)
{
	if (m_bDead)
		return 1;

	m_pTransformCom->Change_Information(CTransform::INFO_ANGLE, &_vec3(0.f, D3DXToRadian(fTimeDelta * 15.f), 0.f)); // µ¹¾Æµ¹¾Æ

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DEFAULT, this);

	m_fAcc += fTimeDelta * 5.f;

	return 0;
}

void CTowerRadius::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == m_pShaderCom)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	pEffect->AddRef();

	Set_ConstantTable(pEffect);

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(UI_TOWERRADIUS);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CTowerRadius::Add_Component(void)
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
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_TowerRaidus", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CTowerRadius::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", 0);

	pEffect->SetVector("g_fColor", &_vec4(0.4f, 0.4f, 1.f, 1.f)); // ¾à°£ Çª¸£°Ô
	pEffect->SetFloat("g_fAlpha", fabs(cosf(m_fAcc))); // ±ôºý±ôºý°Å¸®±â

	return NOERROR;
}

CTowerRadius * CTowerRadius::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTowerRadius* pInstance = new CTowerRadius(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CTowerRadius Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CTowerRadius::Free(void)
{
	CFont_Perspective::Free();
}
