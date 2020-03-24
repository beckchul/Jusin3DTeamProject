#include "stdafx.h"
#include "Effect_Frame.h"
#include "Defines.h"
#include "Target_Manager.h"
#include "FrustumManager.h"

CEffect_Frame::CEffect_Frame(LPDIRECT3DDEVICE9 pGraphicDev)
	: CEffect_2D(pGraphicDev)
	, m_tFrame(FRAME(0.f, 0.f, 0.f))
	, m_pTexTag(nullptr)
	, m_eSceneID(SCENE_END)
	, m_bIsLoop(TRUE)
	, m_iLoopCount(0)
	, m_iLoopLimit(1)
	, m_bMixFrame(TRUE)
{
}

CEffect_Frame::CEffect_Frame(const CEffect_Frame& rhs)
	: CEffect_2D(rhs)
	, m_tFrame(rhs.m_tFrame)
	, m_pTexTag(rhs.m_pTexTag)
	, m_eSceneID(rhs.m_eSceneID)
	, m_bIsLoop(rhs.m_bIsLoop)
	, m_iLoopCount(rhs.m_iLoopCount)
	, m_iLoopLimit(rhs.m_iLoopLimit)
	, m_bMixFrame(rhs.m_bMixFrame)
{
	// Frame이 모두 동일하니까 좀 인위적인 느낌이 듬
	// 적절하게 섞어주면 좋을듯 

	// +) 섞을때 너무 Max와 인접한 숫자가 되어버리니, Limit를 걸어놓은 경우
	// 순식간에 사라지는 문제가 발생.
	if (m_bMixFrame)
	{
		float fMax = m_tFrame.fMax - 5.f;
		int iRandCount = Random((int)fMax);
		m_tFrame.fFrame = (float)iRandCount;
	}
	else
	{
		m_tFrame.fFrame = 0.f;
	}
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

CEffect_Frame::~CEffect_Frame(void)
{
}

CEffect_Frame* CEffect_Frame::Clone(void)
{
	return new CEffect_Frame(*this);
}

HRESULT CEffect_Frame::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_eShaderPass = EFFECT_2D_SOFTBLEND;

	// 어떤 이펙트인지에 따라 바뀌는 값
	//m_tFrame = FRAME(0.f, 8.f, 4.f); // 총 4개의 텍스처, 1초에 2번 돈다 ( 8 / 4)
	return NOERROR;
}

int CEffect_Frame::Update_GameObject(const float & fTimeDelta)
{
	if (m_bUseTimeDead && m_fTime <= 0.f || m_bDead) // 이펙트 쥬금..
		return 1;

	if (!CFrustumManager::GetInstance()->IsSphereInFrustum(m_pTransformCom->Get_Info(CTransform::INFO_POSITION), 0.f))
		return 0;

	m_fStartTime -= fTimeDelta;
	if (m_bIsStartTimeControl && 0.f <= m_fStartTime)
		return 0;

	m_tFrame.fFrame += m_tFrame.fCount * fTimeDelta;

	if (m_tFrame.fFrame >= m_tFrame.fMax)
	{
		m_iLoopCount++;
		m_tFrame.fFrame = 0.f;
		if (!m_bIsLoop && m_iLoopLimit <= m_iLoopCount)
			return 1;
	}

	Revision_Pos();

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_EFFECT, this);
	}
		

	// 알파 블렌딩을 위한 거리에 따른 Sorting
	Compute_ViewZ(&m_pTransformCom->Get_Info(CTransform::INFO_POSITION));
	Perform_Function(fTimeDelta);
	Bill_Perform(m_eBillState);
	Toward_FinalPos(fTimeDelta);

	m_fTime -= fTimeDelta;
	m_fUVTime += fTimeDelta;

	return 0;
}

void CEffect_Frame::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
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

HRESULT CEffect_Frame::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	// 싱글텍스쳐나 프레임 텍스쳐의 경우에는, Tag를 받아와서 생성하게 한다.
	// Enum문도 받아와서 만든다.
	AddComponent(m_pTextureCom, Engine::CTexture*, m_eSceneID, m_pTexTag, Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}


void CEffect_Frame::Set_TexTag(const _tchar* Tag, SCENEID eID)
{
	m_pTexTag = Tag;
	m_eSceneID = eID;
}

HRESULT CEffect_Frame::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);


	m_pTextureCom->SetUp_OnShader(pEffect, "g_DiffuseTexture", (_uint)m_tFrame.fFrame); // Textrue 번호
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

// 받아올만한 정보 : m_tFrame, Shader Pass, Transform정보, Texture 이름 등
CEffect_Frame * CEffect_Frame::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pTexTag, SCENEID eID)
{
	CEffect_Frame* pInstance = new CEffect_Frame(pGraphicDev);

	pInstance->Set_TexTag(pTexTag, eID);		// 이름 할당.

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CEffect_Frame Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CEffect_Frame::Free(void)
{

	CEffect_2D::Free();
	cout << "Frame 이펙트 소멸~" << endl;
}
