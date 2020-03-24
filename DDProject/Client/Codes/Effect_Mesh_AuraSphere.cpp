#include "stdafx.h"
#include "Effect_Mesh_AuraSphere.h"
#include "UI_Skill.h"
#include "Object_Manager.h"

USING(Engine)

CEffect_Mesh_AuraSphere::CEffect_Mesh_AuraSphere(LPDIRECT3DDEVICE9 pGraphicDev)
	:CEffect_Mesh(pGraphicDev)
{
}

CEffect_Mesh_AuraSphere::CEffect_Mesh_AuraSphere(const CEffect_Mesh_AuraSphere& rhs)
	: CEffect_Mesh(rhs)
{
	/*
	 정리 
	 1) 컴포넌트를 Clone해서 쓰기는 편하지만, 클래스 자체를 Clone시키니 많은 크래쉬 
	 2) 모든 부모 복사생성자에서 쓰고있는 컴포넌트의 Clone과 동시에, Add_Ref를 증가시켜준다.
	 3) 그리고, 최종적으로 이곳에서는 mapComponent에 복사된애들을 넣어주기만 함 .
	 */

	/*
	 문제가 생겼던 곳
	 1) m_pTransformCom에 Scale이나 Pos값이 들어가는걸 확인했지만 적용이 안되고 있었음
	 -> Update를 돌때 mapComponent에 실제적으로 들어가있는 Dynamic(업데이트가 필요한 컴포넌트)들을
			업데이트 해주는데, Clone은 햇지만, mapComponent에 Insert를 안해주고 있었음.

	 ---> 그래서 원래 하던대로 AddComponent(매크로)를 써서 넣어줬는데,
			   생성은 되었지만 지울떄 RefCount가 안맞아버림

	 -----> 매크로는 mapComponent에 insert만 해주는것이 아닌
				 Clone과 AddRef를 동시에 해주고있었음.

	 -------> 우리는 지금 부모에서 Clone과 AddRef를 해주고있는데,
					2번 진행되고 있던 셈 ? <- Count가 안맞을수밖에 없음

	 ---------> 최종적으로 .. Clone같은건 부모나 뭐 필요한곳에서 해주고 ,
					 한곳에서 AddRef해주고, 웬만하면 이곳에서는 Update가 잘 돌수 있도록
					 mapComponent에 지금까지 복사해온걸 넣어주도록
	*/

	// TransformCom 돌려쓰는듯
	// -> 모든 Transform의 주소값이 다 같음 ! ( 생성할때마다 생성은 되는데 . . . 다들 위치가 한번에 달라지는거지 )
	// 다른건 괜찮을 거 같은데 Transform만은 생성을 해줘야 될 거 같아.
	// 어떻게 ?

	// Clone이 필요한 컴포넌트와 아닌 컴포넌트 . .
	// 매쉬 , 쉐이더, 렌더러는 돌려써도 상관 X ( 필요없는 생성 최대한 줄이고 돌려쓰려고 클론하는거
	// .. -> 트랜스폼만 ?

	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Render", m_pRendererCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", m_pStaticMeshCom));
	m_pRendererCom->Add_Ref();
	m_pStaticMeshCom->Add_Ref();

	// For.Transform
	m_pTransformCom = ((Engine::CTransform*)Engine::CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Com_Transform"));
	if (nullptr == m_pTransformCom)
		return;
	m_mapComponent[Engine::CComponent::COM_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", m_pTransformCom));
	m_pTransformCom->Add_Ref();
	// Transform 컴포넌트만 다른 주소값을 가지게 할당 


	//Reset_Effect();

}

CEffect_Mesh_AuraSphere::~CEffect_Mesh_AuraSphere(void)
{

}

CEffect_Mesh_AuraSphere* CEffect_Mesh_AuraSphere::Clone(void)
{
	return new CEffect_Mesh_AuraSphere(*this);
}

HRESULT CEffect_Mesh_AuraSphere::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_eShaderPass = EFFECT_MESH_ALPHABLEND;

	m_bUVAnimation = TRUE;
	//m_bUseTimeDead = TRUE;
	//m_bFadeOut = TRUE;
	m_bFadeIn = TRUE;

	m_vecUV = _vec2(10.f, 40.f);
	m_vecUVAnimation = _vec2(1.f, -4.f);
	m_fUVAniSpeed = 0.25f;
	m_vColor = _vec4(0.15f, 0.25f, 0.15f, 0.f);
	m_fUVTime = 0.f;
	//m_fTime = 6.f;		// 6.f초 지나면 사라짐.

	return NOERROR;
}

int CEffect_Mesh_AuraSphere::Update_GameObject(const float & fTimeDelta)
{
	if (m_bUseTimeDead && m_fTime <= 0.f || m_bDead)		// 시간 설정시
		return 1;

	Perform_Function(fTimeDelta);		// Bool형 변수에 따른 기능 수행.

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_EFFECT, this);

	// 알파 블렌딩을 위한 거리에 따른 Sorting
	Compute_ViewZ(&m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	m_fUVTime += fTimeDelta;				// UV애니메이션 사용시 점진적인 시간값 넣어줌
	m_fTime -= fTimeDelta;					// 시간 소멸 사용시 , 시간 줄여줌

	if (m_bFadeIn && m_fUVTime <= 0.8f)
	{		// UVTime은 점진적으로 더해지는 시간이니, 이걸 사용하자.
		m_vColor.w += fTimeDelta * 0.8f;
		if (m_vColor.w >= 0.6f)
			m_vColor.w = 0.6f;
	}

	if (m_bFadeOut && m_fTime <= 2.f)		// 2초부터 FadeOut
	{
		m_vColor.w -= fTimeDelta * 0.5f;
		if (m_vColor.w <= 0.f)
			m_vColor.w = 0.f;
	}

	return 0;
}

void CEffect_Mesh_AuraSphere::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh_Effect(pArgEffect, m_eShaderPass);
}

void CEffect_Mesh_AuraSphere::Reset_Effect(void)
{
	m_bUVAnimation = TRUE;
	m_bUseTimeDead = TRUE;
	m_bFadeOut = TRUE;
	m_bFadeIn = TRUE;

	m_vecUV = _vec2(10.f, 30.f);
	m_vecUVAnimation = _vec2(1.f, 4.f);
	m_fUVAniSpeed = 0.25f;
	m_vColor = _vec4(0.15f, 0.18f, 0.15f, 0.f);
	m_fUVTime = 0.f;
	m_fTime = 6.f;		// 6.f초 지나면 사라짐.
}

HRESULT CEffect_Mesh_AuraSphere::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Mesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STATIC, L"Com_Mesh_AuraSphere", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;
}

HRESULT CEffect_Mesh_AuraSphere::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pStaticMeshCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	pEffect->SetVector("g_vColor", &m_vColor);
	pEffect->SetVector("g_vUV", (D3DXVECTOR4*)&m_vecUV);

	pEffect->SetFloat("g_fTime", m_fUVTime);

	if (m_bUVAnimation)
	{
		pEffect->SetVector("g_vUVAni", (D3DXVECTOR4*)&m_vecUVAnimation);
		pEffect->SetFloat("g_fUVAniSpeed", m_fUVAniSpeed);
	}
	else
		pEffect->SetFloat("g_fUVAniSpeed", 0.f);


	return NOERROR;
}

CEffect_Mesh_AuraSphere* CEffect_Mesh_AuraSphere::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffect_Mesh_AuraSphere* pInstance = new CEffect_Mesh_AuraSphere(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CEffect_Mesh_AuraSphere Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CEffect_Mesh_AuraSphere::Free(void)
{
	CEffect_Mesh::Free();

	cout << "CEffect_Mesh_AuraSphere 소멸!" << endl;
}

