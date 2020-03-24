#include "stdafx.h"
#include "Effect_Single.h"
#include "Defines.h"
#include "Target_Manager.h"

CEffect_Single::CEffect_Single(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect_2D(pGraphicDev)
	, m_pTexTag(nullptr)
	, m_eSceneID(SCENE_END)
{
}

CEffect_Single::CEffect_Single(const CEffect_Single& rhs)
	: CEffect_2D(rhs)
	, m_pTexTag(rhs.m_pTexTag)
	, m_eSceneID(rhs.m_eSceneID)
{
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Render", m_pRendererCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", m_pTextureCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", m_pBufferCom));
	m_pRendererCom->Add_Ref();
	m_pTextureCom->Add_Ref();
	m_pBufferCom->Add_Ref();

	m_pTransformCom = ((Engine::CTransform*)Engine::CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Com_Transform"));
	if (nullptr == m_pTransformCom)
		return;
	m_mapComponent[Engine::CComponent::COM_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", m_pTransformCom));

	m_pTransformCom->Add_Ref();
}

CEffect_Single::~CEffect_Single(void)
{
}

CEffect_Single* CEffect_Single::Clone(void)
{
	return new CEffect_Single(*this);
}

HRESULT CEffect_Single::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_eShaderPass = EFFECT_2D_ALPHABLEND;

	return NOERROR;
}

int CEffect_Single::Update_GameObject(const float & fTimeDelta)
{
	if (m_bUseTimeDead && m_fTime <= 0.f || m_bDead)		// 시간 설정시
		return 1;

	m_fStartTime -= fTimeDelta;
	if (m_bIsStartTimeControl && 0.f <= m_fStartTime)
		return 0;

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

void CEffect_Single::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (m_bIsStartTimeControl && 0.f <= m_fStartTime)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(m_eShaderPass);	

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();
}

HRESULT CEffect_Single::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, m_eSceneID, m_pTexTag, Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}


HRESULT CEffect_Single::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;


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


void CEffect_Single::Set_TexTag(const _tchar* Tag, SCENEID eID)
{
	m_pTexTag = Tag;
	m_eSceneID = eID;
}

CEffect_Single * CEffect_Single::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pTexTag, SCENEID eID)
{
	CEffect_Single* pInstance = new CEffect_Single(pGraphicDev);

	pInstance->Set_TexTag(pTexTag, eID);		// 이름 할당.

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CEffect_Single Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CEffect_Single::Free(void)
{
	CEffect_2D::Free();
	//cout << "Single 이펙트 소멸~" << endl;
}