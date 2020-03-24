#include "stdafx.h"
#include "Font_NavMesh.h"
#include "Renderer.h"
#include "DataManager.h"

CFont_NavMesh::CFont_NavMesh(LPDIRECT3DDEVICE9 pGraphicDev, _uint uIndex)
	: CFont_Perspective(pGraphicDev)
	, m_uIndex(uIndex)
	, m_pDataMgr(Engine::CDataManager::GetInstance())
{
	m_pDataMgr->Add_Ref();
}


CFont_NavMesh::~CFont_NavMesh()
{
}

HRESULT CFont_NavMesh::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return NOERROR;
}

int CFont_NavMesh::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DEFAULT, this);

	// ºôº¸µå
	D3DXMATRIX		matScale, matBill;

	D3DXMatrixScaling(&matScale, 4.f, 4.f, 1.f);

	matBill = m_pDataMgr->Get_ViewInverseMatrix();
	ZeroMemory(&matBill.m[3][0], sizeof(_vec3));

	m_pTransformCom->Set_WorldMatrix(&(matScale * matBill * *m_pTransformCom->Get_WorldMatrix()));

	return 0;
}

void CFont_NavMesh::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == m_pShaderCom)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(UI_ALPHATEST);

	CFont_Perspective::Render_Number(m_uIndex, pEffect);

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CFont_NavMesh::Add_Component(void)
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
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Font_Number_Skin", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

CFont_NavMesh * CFont_NavMesh::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint uIndex)
{
	CFont_NavMesh* pInstance = new CFont_NavMesh(pGraphicDev, uIndex);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CFont_NavMesh Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CFont_NavMesh::Free(void)
{
	Engine::Safe_Release(m_pDataMgr);

	CFont_Perspective::Free();
}
