#include "stdafx.h"
#include "HallOfHeroes.h"
#include "Renderer.h"
#include "DataManager.h"
#include "CollisionManager.h"

USING(Engine)

CHallOfHeroes::CHallOfHeroes(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex)
	: CStaticObject(pGraphicDev)
	, m_iTextureNum(iIndex)
{
}


CHallOfHeroes::~CHallOfHeroes()
{
}

HRESULT CHallOfHeroes::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	return NOERROR;
}

int CHallOfHeroes::FirstUpdate_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	CCollisionManager::GetInstance()->Add_MouseCol(m_pGraphicDev, m_pStaticMeshCom->Get_Mesh(), *m_pTransformCom->Get_WorldMatrix());

	return 0;
}

int CHallOfHeroes::Update_GameObject(const float & fTimeDelta)
{
	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);
	}
	return 0;
}

void CHallOfHeroes::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh(pArgEffect, uPassIdx);
}

HRESULT CHallOfHeroes::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Mesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_LOBBY, Engine::CDataManager::GetInstance()->Get_ObjName3()[m_iTextureNum - Engine::OBJECT3_END]->szObjectName, Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;
}

HRESULT CHallOfHeroes::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	return NOERROR;
}

CHallOfHeroes * CHallOfHeroes::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iIndex)
{
	CHallOfHeroes* pInstance = new CHallOfHeroes(pGraphicDev, iIndex);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CHallOfHeroes Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CHallOfHeroes::Free(void)
{
	CStaticObject::Free();
}
