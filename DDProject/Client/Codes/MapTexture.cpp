#include "stdafx.h"
#include "MapTexture.h"
#include "Renderer.h"
#include "DataManager.h"

USING(Engine)

CMapTexture::CMapTexture(LPDIRECT3DDEVICE9 pGraphicDev, Engine::OBJECTID eObjectId)
	: CPerspectiveUI(pGraphicDev)
	, m_iTextureIdx(eObjectId)
	, m_pDataMgr(Engine::CDataManager::GetInstance())
{
	m_pDataMgr->Add_Ref();
}

CMapTexture::~CMapTexture()
{
}

HRESULT CMapTexture::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return NOERROR;
}

int CMapTexture::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DEFAULT, this);

	// ºôº¸µå
	_matrix		matScale, matBill;

	D3DXMatrixScaling(&matScale, 2.f, 2.f, 1.f);

	matBill = m_pDataMgr->Get_ViewInverseMatrix();
	ZeroMemory(&matBill.m[3][0], sizeof(_vec3));

	m_pTransformCom->Set_WorldMatrix(&(matScale * matBill * *m_pTransformCom->Get_WorldMatrix()));

	return 0;
}

void CMapTexture::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
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
	pEffect->BeginPass(UI_ALPHATEST);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CMapTexture::Add_Component(void)
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
	switch ((Engine::OBJECTID)m_iTextureIdx)
	{
	case Engine::OBJECT_POINT_LIGHT0:
	case Engine::OBJECT_POINT_LIGHT1:
	case Engine::OBJECT_POINT_LIGHT2:
	case Engine::OBJECT_POINT_LIGHT3:
	case Engine::OBJECT_POINT_LIGHT4:
	case Engine::OBJECT_POINT_LIGHT5:
		AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Light", Engine::CComponent::COM_STATIC, L"Com_Texture");
		m_iTextureIdx = m_iTextureIdx - Engine::OBJECT_POINT_LIGHT0;
		break;


	case Engine::OBJECT3_POINT_LIGHT0:
	case Engine::OBJECT3_POINT_LIGHT1:
		AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Light", Engine::CComponent::COM_STATIC, L"Com_Texture");
		m_iTextureIdx = m_iTextureIdx - Engine::OBJECT3_POINT_LIGHT0;
		break;

	case Engine::OBJECT_MONSTER_ROUTE0:
	case Engine::OBJECT_MONSTER_ROUTE1:
	case Engine::OBJECT_MONSTER_ROUTE2:
	case Engine::OBJECT_MONSTER_ROUTE3:
	case Engine::OBJECT_MONSTER_ROUTE4:
	case Engine::OBJECT_MONSTER_ROUTE5:
	case Engine::OBJECT_MONSTER_ROUTE6:
	case Engine::OBJECT_MONSTER_ROUTE7:
		AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Route", Engine::CComponent::COM_STATIC, L"Com_Texture");
		m_iTextureIdx = m_iTextureIdx - Engine::OBJECT_MONSTER_ROUTE0;
		break;
	}

	return NOERROR;
}

HRESULT CMapTexture::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", m_iTextureIdx);

	return NOERROR;
}

CMapTexture * CMapTexture::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::OBJECTID eObjectId)
{
	CMapTexture* pInstance = new CMapTexture(pGraphicDev, eObjectId);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CMapTexture Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CMapTexture::Free(void)
{
	Engine::Safe_Release(m_pDataMgr);

	CPerspectiveUI::Free();
}
