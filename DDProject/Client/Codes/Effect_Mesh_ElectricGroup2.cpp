#include "stdafx.h"
#include "Effect_Mesh_ElectricGroup2.h"

USING(Engine)

CEffect_Mesh_ElectricGroup2::CEffect_Mesh_ElectricGroup2(LPDIRECT3DDEVICE9 pGraphicDev)
	:CEffect_Mesh(pGraphicDev)
{
}

CEffect_Mesh_ElectricGroup2::CEffect_Mesh_ElectricGroup2(const CEffect_Mesh_ElectricGroup2& rhs)
	: CEffect_Mesh(rhs)
{

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

	if (m_bInitRotationRand)
	{
		_float fRandX = D3DXToRadian((float)Random(360));
		_float fRandY = D3DXToRadian((float)Random(360));
		_float fRandZ = D3DXToRadian((float)Random(360));
		m_pTransformCom->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(fRandX, fRandY, fRandZ));
	}

	Reset_Effect();
}

CEffect_Mesh_ElectricGroup2::~CEffect_Mesh_ElectricGroup2(void)
{

}

CEffect_Mesh_ElectricGroup2* CEffect_Mesh_ElectricGroup2::Clone(void)
{
	return new CEffect_Mesh_ElectricGroup2(*this);
}

HRESULT CEffect_Mesh_ElectricGroup2::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_eShaderPass = EFFECT_MESH_ALPHABLEND_CULLNONE;

	//m_bUseTimeDead = TRUE;
	//m_bFadeOut = TRUE;

	//m_vColor = _vec4(1.f, 0.f, 0.f, 1.f);
	//m_fTime = 8.f;		// 8.f초 지나면 사라짐.

	m_bFadeIn = TRUE;

	return NOERROR;
}

int CEffect_Mesh_ElectricGroup2::Update_GameObject(const float & fTimeDelta)
{
	if (m_bUseTimeDead && m_fTime <= 0.f || m_bDead)		// 시간 설정시
		return 1;

	m_fStartTime -= fTimeDelta;
	if (m_bIsStartTimeControl && 0.f <= m_fStartTime)
		return 0;

	Perform_Function(fTimeDelta);		// Bool형 변수에 따른 기능 수행.

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_EFFECT, this);

	// 알파 블렌딩을 위한 거리에 따른 Sorting
	Compute_ViewZ(&m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	m_fUVTime += fTimeDelta;
	m_fTime -= fTimeDelta;

	if (m_bFadeIn && m_fUVTime <= 0.8f)
	{		// UVTime은 점진적으로 더해지는 시간이니, 이걸 사용하자.
		m_vColor.w += fTimeDelta * 1.8f;
		if (m_vColor.w >= 1.f)
			m_vColor.w = 1.f;
		//cout << "Fade In -- Alpha : " << m_vColor.w << endl;
	}

	Perform_Flicker(fTimeDelta);	// 깜빡깜빡

	return 0;
}

void CEffect_Mesh_ElectricGroup2::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (m_bIsStartTimeControl && 0.f <= m_fStartTime)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh_Effect(pArgEffect, m_eShaderPass);
}

HRESULT CEffect_Mesh_ElectricGroup2::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Mesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STATIC, L"Com_Mesh_ElectricityGroup02", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;
}

HRESULT CEffect_Mesh_ElectricGroup2::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CEffect_Mesh_ElectricGroup2* CEffect_Mesh_ElectricGroup2::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffect_Mesh_ElectricGroup2* pInstance = new CEffect_Mesh_ElectricGroup2(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CEffect_Mesh_ElectricGroup2 Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CEffect_Mesh_ElectricGroup2::Free(void)
{
	CEffect_Mesh::Free();
	cout << "CEffect_Mesh_ElectricGroup2 소멸!" << endl;
}

