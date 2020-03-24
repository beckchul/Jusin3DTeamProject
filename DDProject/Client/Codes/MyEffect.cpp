#include "stdafx.h"
#include "MyEffect.h"
#include "Defines.h"
#include "Target_Manager.h"

CMyEffect::CMyEffect(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CEffect(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pTextureCom(nullptr)
{
}

CMyEffect::~CMyEffect(void)
{
}

HRESULT CMyEffect::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	// 어떤 이펙트인지에 따라 바뀌는 값
	m_tFrame = FRAME(0.f, 64.f, 64.f); 

	m_vColor = _vec4(1.f, 1.f, 1.f, 1.f);
	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(50.f, 50.f, 50.f));

	m_fTime = 1.f;

	return NOERROR;
}

int CMyEffect::Update_GameObject(const float & fTimeDelta)
{
	if (m_fTime < 0.f) // 이펙트 쥬금..
		return 1;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_ALPHA, this);

	// 알파 블렌딩을 위한 거리에 따른 Sorting
	Compute_ViewZ(&m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	m_tFrame.fFrame += m_tFrame.fCount * fTimeDelta;
	if (m_tFrame.fFrame >= m_tFrame.fMax)
		m_tFrame.fFrame = 0.f;

	Bill_Perform(BILL_ALL);

	m_fTime -= fTimeDelta;

	return 0;
}

void CMyEffect::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
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

HRESULT CMyEffect::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Shader
	AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_Effect", Engine::CComponent::COM_STATIC, L"Com_Shader");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STAGE, L"Com_Texture_MyEffect", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

CEffect * CMyEffect::Clone(void)
{
	return new CMyEffect(*this);
}

HRESULT CMyEffect::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	D3DXMATRIX		matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", (_uint)m_tFrame.fFrame); // Textrue 번호

	CTarget_Manager::GetInstance()->SetUp_OnShader(pEffect, L"Target_Depth", "g_DepthTexture"); // Soft Effect를 위함

	return NOERROR;
}

// 받아올만한 정보 : m_tFrame, Shader Pass, Transform정보, Texture 이름 등
CMyEffect * CMyEffect::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CMyEffect* pInstance = new CMyEffect(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CMyEffect Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CMyEffect::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBufferCom);

	Engine::CEffect::Free();
}
