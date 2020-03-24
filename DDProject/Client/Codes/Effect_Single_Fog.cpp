#include "stdafx.h"
#include "Effect_Single_Fog.h"

#include "Defines.h"
#include "Target_Manager.h"

CEffect_Single_Fog::CEffect_Single_Fog(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect_2D(pGraphicDev)
	, m_eSceneID(SCENE_END)
	, m_fFadeSpeed(-0.03f)
{
}

CEffect_Single_Fog::CEffect_Single_Fog(const CEffect_Single_Fog& rhs)
	: CEffect_2D(rhs)
	, m_eSceneID(rhs.m_eSceneID)
	, m_pTextureMaskCom(rhs.m_pTextureMaskCom)
	, m_pTextureFaderCom(rhs.m_pTextureFaderCom)
	, m_fFadeSpeed(rhs.m_fFadeSpeed)
{
	m_pTextureMaskCom->Add_Ref();
	m_pTextureFaderCom->Add_Ref();

	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Render", m_pRendererCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", m_pTextureCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", m_pBufferCom));
	m_pRendererCom->Add_Ref();
	m_pTextureCom->Add_Ref();
	m_pBufferCom->Add_Ref();

	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_MaskTexture", m_pTextureMaskCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_FaderTexture", m_pTextureFaderCom));

	m_pTextureMaskCom->Add_Ref();
	m_pTextureFaderCom->Add_Ref();

	m_pTransformCom = ((Engine::CTransform*)Engine::CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Com_Transform"));
	if (nullptr == m_pTransformCom)
		return;
	m_mapComponent[Engine::CComponent::COM_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", m_pTransformCom));

	m_pTransformCom->Add_Ref();
}

CEffect_Single_Fog::~CEffect_Single_Fog(void)
{
}

CEffect_Single_Fog* CEffect_Single_Fog::Clone(void)
{
	return new CEffect_Single_Fog(*this);
}

HRESULT CEffect_Single_Fog::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_eShaderPass = EFFECT_MESH_FOGMASK;

	return NOERROR;
}

int CEffect_Single_Fog::Update_GameObject(const float & fTimeDelta)
{
	if (m_bUseTimeDead && m_fTime <= 0.f || m_bDead)		// 시간 설정시
		return 1;

	Revision_Pos();

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_EFFECT, this);

	// 알파 블렌딩을 위한 거리에 따른 Sorting
	Compute_ViewZ(&m_pTransformCom->Get_Info(CTransform::INFO_POSITION));
	Perform_Function(fTimeDelta);
	Bill_Perform(m_eBillState);
	Toward_FinalPos(fTimeDelta);

	m_fTime -= fTimeDelta;
	m_fUVTime += fTimeDelta;

	return 0;
}

void CEffect_Single_Fog::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(m_eShaderPass);

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();
}

HRESULT CEffect_Single_Fog::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, m_eSceneID, L"Com_Texture_FogSheet", Engine::CComponent::COM_STATIC, L"Com_Texture");
	
	// For.MaskTexture
	AddComponent(m_pTextureMaskCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_FogMerge", Engine::CComponent::COM_STATIC, L"Com_MaskTexture");

	// For.FaderTexture
	AddComponent(m_pTextureFaderCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_FogFader", Engine::CComponent::COM_STATIC, L"Com_FaderTexture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}


HRESULT CEffect_Single_Fog::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	m_pTextureMaskCom->SetUp_OnShader(pEffect, "g_MaskTexture", 0);
	m_pTextureFaderCom->SetUp_OnShader(pEffect, "g_FadeTexture", 0);

	pEffect->SetFloat("fFadeSpeed", m_fFadeSpeed);

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_DiffuseTexture", 0); // Textrue 번호
	pEffect->SetVector("g_vColor", &m_vColor);
	pEffect->SetVector("g_vUV", (D3DXVECTOR4*)&m_vecUV);

	// UVAnimation
	pEffect->SetFloat("g_fTime", m_fUVTime);

	if (m_bUVAnimation)
	{
		pEffect->SetVector("g_vUVAni", (D3DXVECTOR4*)&m_vecUVAnimation);
		pEffect->SetFloat("g_fUVAniSpeed", m_fUVAniSpeed);
	}
	else
		pEffect->SetFloat("g_fUVAniSpeed", 0.f);

	CTarget_Manager::GetInstance()->SetUp_OnShader(pEffect, L"Target_Depth", "g_DepthTexture"); // Soft Effect를 위함

	return NOERROR;
}


void CEffect_Single_Fog::Set_Tag(SCENEID eID)
{
	m_eSceneID = eID;
}

CEffect_Single_Fog * CEffect_Single_Fog::Create(LPDIRECT3DDEVICE9 pGraphicDev, SCENEID eID)
{
	CEffect_Single_Fog* pInstance = new CEffect_Single_Fog(pGraphicDev);

	pInstance->Set_Tag(eID);	// 이름 할당.

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CEffect_Single_Fog Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CEffect_Single_Fog::Free(void)
{
	Safe_Release(m_pTextureMaskCom);
	Safe_Release(m_pTextureFaderCom);

	CEffect_2D::Free();
	cout << "CEffect_Single_Fog 소멸~" << endl;
}
