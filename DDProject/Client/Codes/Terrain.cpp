#include "stdafx.h"
#include "Terrain.h"
#include "Renderer.h"

USING(Engine)

CTerrain::CTerrain(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CGameObject(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pTransformCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pTextureCom(nullptr)

{

}


CTerrain::~CTerrain()
{
}

HRESULT CTerrain::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	if (FAILED(Ready_FilterTexture()))
		return E_FAIL;

	return NOERROR;
}

int CTerrain::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_STATICMESH_NONEALPHA, this);

	return 0;
}

void CTerrain::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
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

HRESULT CTerrain::Add_Component(void)
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
	pComponent = m_pShaderCom = ((Engine::CShader*)m_pComponentMgr->Clone_Component(SCENE_STATIC, L"Com_Shader_Terrain"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", pComponent));
	m_pShaderCom->Add_Ref();

	// For.Texture
	pComponent = m_pTextureCom = ((Engine::CTexture*)m_pComponentMgr->Clone_Component(SCENE_STAGE, L"Com_Texture_Terrain"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", pComponent));
	m_pTextureCom->Add_Ref();

	// For.Buffer
	pComponent = m_pBufferCom = ((Engine::CTerrain_Texture*)m_pComponentMgr->Clone_Component(SCENE_STATIC, L"Com_Buffer_Terrain"));
	if (nullptr == pComponent)
		return E_FAIL;
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", pComponent));
	m_pBufferCom->Add_Ref();

	return NOERROR;
}

HRESULT CTerrain::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", 0);
	m_pTextureCom->SetUp_OnShader(pEffect, "g_DestTexture", 1);
	pEffect->SetTexture("g_FilterTexture", m_pFilterTexture);

	return NOERROR;
}

HRESULT CTerrain::Ready_FilterTexture(void)
{
	if (FAILED(D3DXCreateTexture(m_pGraphicDev, 129, 129, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pFilterTexture)))
		return E_FAIL;

	//D3DXGetImageInfoFromFile

	D3DLOCKED_RECT			LockRect;

	m_pFilterTexture->LockRect(0, &LockRect, nullptr, 0);

	for (size_t i = 0; i < 129; ++i)
	{
		for (size_t j = 0; j < 129; ++j)
		{
			_uint			iIndex = i * 129 + j;

			if (j > 65)
				((_ulong*)LockRect.pBits)[iIndex] = D3DXCOLOR(1.f, 1.f, 1.f, 1.f);
			else
				((_ulong*)LockRect.pBits)[iIndex] = D3DXCOLOR(0.f, 0.f, 0.f, 1.f);
		}
	}

	m_pFilterTexture->UnlockRect(0);

	D3DXSaveTextureToFile(L"../Bin/Tmp.png", D3DXIFF_PNG, m_pFilterTexture, nullptr);

	return NOERROR;
}

CTerrain * CTerrain::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CTerrain* pInstance = new CTerrain(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CTerrain Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CTerrain::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pFilterTexture);

	Engine::CGameObject::Free();
}
