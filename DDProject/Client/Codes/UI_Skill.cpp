#include "stdafx.h"
#include "UI_Skill.h"
#include "Renderer.h"
#include "DataManager.h"

USING(Engine)

CUI_Skill::CUI_Skill(LPDIRECT3DDEVICE9 pGraphicDev, _uint iTexture)
	: CPerspectiveUI(pGraphicDev)
	, m_iTextureIdx(iTexture)
	, m_pDataMgr(Engine::CDataManager::GetInstance())
{
	m_pDataMgr->Add_Ref();
}

CUI_Skill::~CUI_Skill()
{
}

HRESULT CUI_Skill::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	// SlowAura
	m_tFrame = FRAME(0.f, 4.f, 4.f);

	return NOERROR;
}

int CUI_Skill::Update_GameObject(const float & fTimeDelta)
{
	if (m_bDead)
		return 1;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DEFAULT, this);

	// 프레임
	m_tFrame.fFrame += m_tFrame.fCount * fTimeDelta;
	if (m_tFrame.fFrame >= m_tFrame.fMax)
		m_tFrame.fFrame = 0.f;

	// 빌보드
	_matrix		matScale, matBill;

	D3DXMatrixScaling(&matScale, 6.f, 6.f, 1.f);

	matBill = m_pDataMgr->Get_ViewInverseMatrix();
	ZeroMemory(&matBill.m[3][0], sizeof(_vec3));

	m_pTransformCom->Set_WorldMatrix(&(matScale * matBill * *m_pTransformCom->Get_WorldMatrix()));

	return 0;
}

void CUI_Skill::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == m_pShaderCom)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	if (FAILED(Set_ConstantTable(pEffect)))
	{
		Safe_Release(pEffect);
		return;
	}

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(UI_DARKREMOVEALPHATEST);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CUI_Skill::Add_Component(void)
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

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	// For.Texture
	switch (m_iTextureIdx)
	{
	case TOWER_TYPE_SLOWAURA:
		AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_UI_SlowAura", Engine::CComponent::COM_STATIC, L"Com_Texture");
		break;

	case TOWER_TYPE_HEALINGAURA:
		AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_UI_HealAura", Engine::CComponent::COM_STATIC, L"Com_Texture");
		break;

	case TOWER_TYPE_ENRAGEAURA:
		AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_UI_EnlargeAura", Engine::CComponent::COM_STATIC, L"Com_Texture");
		break;

	case TOWER_TYPE_LIGHTNINGAURA:
		AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_UI_DeathlyHallow", Engine::CComponent::COM_STATIC, L"Com_Texture");
		break;

	case TOWER_TYPE_DRAINAURA:
		AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_UI_DrainAura", Engine::CComponent::COM_STATIC, L"Com_Texture");
		break;
	}

	return NOERROR;
}

HRESULT CUI_Skill::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", _uint(m_tFrame.fFrame));

	return NOERROR;
}

CUI_Skill * CUI_Skill::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iTexture)
{
	CUI_Skill* pInstance = new CUI_Skill(pGraphicDev, iTexture);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CUI_Skill Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CUI_Skill::Free(void)
{
	Engine::Safe_Release(m_pDataMgr);

	CPerspectiveUI::Free();
}
