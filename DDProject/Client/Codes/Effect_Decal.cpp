#include "stdafx.h"
#include "Effect_Decal.h"
#include "Target_Manager.h"
#include "DataManager.h"
#include "TextureManager.h"

CEffect_Decal::CEffect_Decal(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CEffect(pGraphicDev)
	, m_pTexTag(nullptr)
	, m_pRendererCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pTextureMaskCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_bUVAnimation(FALSE)
	, m_vecUV(1.f, 1.f)
	, m_vecUVAnimation(0.f, 0.f)
	, m_fUVAniSpeed(0.f)
	, m_fUVTime(0.f)
	, m_fDecalMask_FadeInTime(5.f)
	, m_fAll_FadeOutTime(1.f)
	, m_fBlendTime(0.5f)
	, m_vMaskColor(1.f, 1.f, 1.f)
	, m_fColorPower(1.f)
	, m_fFadeSpeed(1.f)
	, m_bUseMaskMap(TRUE)
	, m_PicColor(1.f, 1.f, 1.f, 1.f)
{
}

CEffect_Decal::CEffect_Decal(const CEffect_Decal& rhs)
	: Engine::CEffect(rhs)
	, m_pTexTag(rhs.m_pTexTag)
	, m_pRendererCom(rhs.m_pRendererCom)
	, m_pTextureCom(rhs.m_pTextureCom)
	, m_pTextureMaskCom(rhs.m_pTextureMaskCom)
	, m_pBufferCom(rhs.m_pBufferCom)
	, m_bUVAnimation(rhs.m_bUVAnimation)
	, m_vecUV(rhs.m_vecUV)
	, m_vecUVAnimation(rhs.m_vecUVAnimation)
	, m_fUVAniSpeed(rhs.m_fUVAniSpeed)
	, m_fUVTime(rhs.m_fUVTime)
	, m_fDecalMask_FadeInTime(rhs.m_fDecalMask_FadeInTime)
	, m_fAll_FadeOutTime(rhs.m_fAll_FadeOutTime)
	, m_fBlendTime(rhs.m_fBlendTime)
	, m_vMaskColor(rhs.m_vMaskColor)
	, m_fColorPower(rhs.m_fColorPower)
	, m_fFadeSpeed(rhs.m_fFadeSpeed)
	, m_bUseMaskMap(rhs.m_bUseMaskMap)
{
	m_pRendererCom->Add_Ref();
	m_pTextureCom->Add_Ref();
	m_pTextureMaskCom->Add_Ref();
	m_pBufferCom->Add_Ref();

	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Render", m_pRendererCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", m_pTextureCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", m_pBufferCom));
	m_pRendererCom->Add_Ref();
	m_pTextureCom->Add_Ref();
	m_pBufferCom->Add_Ref();

	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_MaskTexture", m_pTextureMaskCom));
	m_pTextureMaskCom->Add_Ref();

	m_pTransformCom = ((Engine::CTransform*)Engine::CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Com_Transform"));
	if (nullptr == m_pTransformCom)
		return;
	m_mapComponent[Engine::CComponent::COM_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", m_pTransformCom));

	m_pTransformCom->Add_Ref();
}

CEffect_Decal::~CEffect_Decal(void)
{
}

CEffect_Decal* CEffect_Decal::Clone(void)
{
	return new CEffect_Decal(*this);
}

HRESULT CEffect_Decal::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_eShaderPass = EFFECT_DECAL;

	return NOERROR;
}

int CEffect_Decal::Update_GameObject(const float & fTimeDelta)
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

	m_fTime -= fTimeDelta;
	m_fUVTime += fTimeDelta;
	m_fBlendTime -= fTimeDelta;

	//if(m_fBlendTime < 0.f)
	m_fDecalMask_FadeInTime -= fTimeDelta * m_fFadeSpeed;

	if (m_fDecalMask_FadeInTime < 0.f)
		m_bDead = TRUE;

	//if(m_fDecalMask_FadeInTime < 3.f)
	//	m_fDecalMask_FadeInTime += fTimeDelta * 4.f;

	return 0;
}

void CEffect_Decal::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (m_bIsStartTimeControl && 0.f <= m_fStartTime)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;
	
	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(m_eShaderPass);

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();

	//m_PicColor = _vec4(1.f, 1.f, 1.f, 1.f);
}

HRESULT CEffect_Decal::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.DiffuseTexture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, m_pTexTag, Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.MaskTexture
	AddComponent(m_pTextureMaskCom, Engine::CTexture*, SCENE_STATIC, m_pMaskTag, Engine::CComponent::COM_STATIC, L"Com_MaskTexture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CCube_Texture*, SCENE_STATIC, L"Com_Buffer_Cube", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}


HRESULT CEffect_Decal::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	//pEffect->SetVector("g_PicColor", (_vec4*)&m_PicColor);

	// 텍스처 전달
	m_pTextureMaskCom->SetUp_OnShader(pEffect, "g_MaskTexture", 0);	// 마스크 텍스쳐
	m_pTextureCom->SetUp_OnShader(pEffect, "g_DiffuseTexture", 0); // 디퓨즈 텍스처
	CTarget_Manager::GetInstance()->SetUp_OnShader(pEffect, L"Target_Depth2", "g_DepthTexture"); // 깊이 텍스처
	CTarget_Manager::GetInstance()->SetUp_OnShader(pEffect, L"Target_Normal", "g_NormalTexture"); // 노말 텍스처

	_matrix			matView, matProj, matViewInv, matProjInv;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);


	// 뷰 공간상의 위치 구할때 필요.
	pEffect->SetVector("Deproject", &_vec4(matProj.m[0][0], matProj.m[1][1], 0.f, 0.f));

	// worldViewProj 한번에 곱해진거 전달.
	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj); 

	//// viewPorjInv와 worldViewInv를 구한다.
	//_matrix	matViewProjInv, matWorldViewInv;

	//D3DXMatrixMultiply(&matViewProjInv, &matView, &matProj);
	//D3DXMatrixInverse(&matViewProjInv, nullptr, &matViewProjInv);
	//pEffect->SetMatrix("g_matViewProjInv", &matViewProjInv);

	D3DXMatrixInverse(&matProjInv, nullptr, &matProj); // proj 역행렬 전달
	pEffect->SetMatrix("g_matProjInv", &matProjInv);
	D3DXMatrixInverse(&matViewInv, nullptr, &matView); // view 역행렬 전달.
	pEffect->SetMatrix("g_matViewInv", &matViewInv);

	_matrix matworld, matWorldInv;
	matworld = *m_pTransformCom->Get_WorldMatrix();
	D3DXMatrixInverse(&matWorldInv, nullptr, &matworld);
	pEffect->SetMatrix("g_matWorldInv", &matWorldInv); // world 역행렬 전달.

	_matrix matWorldView;
	D3DXMatrixMultiply(&matWorldView, &matworld, &matView);
	pEffect->SetMatrix("g_matWorldView", &matWorldView);


	_vec4 vTopRight;
	vTopRight.x = PLANE_FAR;
	vTopRight.y = PLANE_FAR;
	vTopRight.z = PLANE_FAR;
	vTopRight.w = PLANE_FAR;

 	D3DXVec4Transform(&vTopRight, &vTopRight, &matProjInv);
	vTopRight.y = -vTopRight.y; //?
	//vTopRight /= vTopRight.w;

	pEffect->SetVector("CameraRightTop", &vTopRight);
	pEffect->SetVector("g_vColor", &m_vColor);

	pEffect->SetFloat("fDecalMaskFadeIn", m_fDecalMask_FadeInTime);
	pEffect->SetFloat("fDecalFadeOut", m_fAll_FadeOutTime);
	pEffect->SetFloat("fBlendTime", m_fBlendTime);
	pEffect->SetVector("vMaskColor", (D3DXVECTOR4*)&m_vMaskColor);
	pEffect->SetBool("bMaskOn", m_bUseMaskMap);
	//// 그 이외
	//pEffect->SetVector("g_vColor", &m_vColor);
	//pEffect->SetVector("g_vUV", (D3DXVECTOR4*)&m_vecUV);

	//pEffect->SetVector("g_worldCameraPosition", (D3DXVECTOR4*)&m_pDataMgr->Get_CamPoistion());

	//// UVAnimation
	//pEffect->SetFloat("g_fTime", m_fUVTime);

	//if (m_bUVAnimation)
	//{
	//	pEffect->SetVector("g_vUVAni", (D3DXVECTOR4*)&m_vecUVAnimation);
	//	pEffect->SetFloat("g_fUVAniSpeed", m_fUVAniSpeed);
	//}
	//else
	//	pEffect->SetFloat("g_fUVAniSpeed", 0.f);


	return NOERROR;
}


void CEffect_Decal::Set_TexTag(const _tchar* DiffuseTag, const _tchar* MaskTag)
{
	m_pTexTag	= DiffuseTag;
	// 마스크 태그를 넘겨주었으면, 넘겨준 텍스쳐를 쓰자.
	if (MaskTag != nullptr)
		m_pMaskTag = MaskTag;
	else
	{
		// 안써도 일단 넣어놓긴 하구
		m_pMaskTag = DiffuseTag;
		m_bUseMaskMap = FALSE;
	}
}


CEffect_Decal* CEffect_Decal::Create(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar* pDiffuseTexTag, const _tchar* pMaskTag /*= nullptr*/)
{
	CEffect_Decal* pInstance = new CEffect_Decal(pGraphicDev);

	pInstance->Set_TexTag(pDiffuseTexTag, pMaskTag);		// 이름 할당.

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CEffect_Decal Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CEffect_Decal::Free(void)
{
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pTextureMaskCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);

	Engine::CEffect::Free();
	cout << "Decal 소멸" << endl;
}