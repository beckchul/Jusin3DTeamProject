#include "stdafx.h"
#include "Effect_Mesh_Spiral.h"

USING(Engine)

CEffect_Mesh_Spiral::CEffect_Mesh_Spiral(LPDIRECT3DDEVICE9 pGraphicDev)
	:CEffect_Mesh(pGraphicDev)
{
}

CEffect_Mesh_Spiral::CEffect_Mesh_Spiral(const CEffect_Mesh_Spiral& rhs)
	:CEffect_Mesh(rhs)
{
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Render", m_pRendererCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", m_pStaticMeshCom));
	m_pRendererCom->Add_Ref();
	m_pStaticMeshCom->Add_Ref();

	// ���ο� �ּ� �Ҵ�
	m_pTransformCom = ((Engine::CTransform*)Engine::CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Com_Transform"));
	if (nullptr == m_pTransformCom)
		return;
	m_mapComponent[Engine::CComponent::COM_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", m_pTransformCom));
	m_pTransformCom->Add_Ref();

}

CEffect_Mesh_Spiral::~CEffect_Mesh_Spiral(void)
{

}

CEffect_Mesh_Spiral* CEffect_Mesh_Spiral::Clone(void)
{
	return new CEffect_Mesh_Spiral(*this);
}

HRESULT CEffect_Mesh_Spiral::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;
	m_eShaderPass = EFFECT_MESH_ALPHABLEND_CULLNONE;

	m_bUVAnimation = TRUE;
	//m_bFadeOut = TRUE;
	m_bRotating = TRUE;

	m_vecUVAnimation = _vec2(0.f, -4.f);
	m_fUVAniSpeed = 0.25f;
	m_fTime = 6.f;		// 6.f�� ������ �����.
	m_vecRotating = _vec3(0.f, 120.f, 0.f);
	return NOERROR;
}

int CEffect_Mesh_Spiral::Update_GameObject(const float & fTimeDelta)
{
	if (m_bUseTimeDead && m_fTime <= 0.f || m_bDead)		// �ð� ������
		return 1;

	Perform_Function(fTimeDelta);		// Bool�� ������ ���� ��� ����.

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_EFFECT, this);

	// ���� ������ ���� �Ÿ��� ���� Sorting
	Compute_ViewZ(&m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	m_fUVTime += fTimeDelta;				// UV�ִϸ��̼� ���� �������� �ð��� �־���
	m_fTime -= fTimeDelta;					// �ð� �Ҹ� ���� , �ð� �ٿ���


	if (m_bFadeOut && m_fTime <= 2.f)		// 2�ʺ��� FadeOut
	{
		m_vColor.w -= fTimeDelta * 0.5f;
		if (m_vColor.w <= 0.f)
			m_vColor.w = 0.f;
	}

	return 0;
}

void CEffect_Mesh_Spiral::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh_Effect(pArgEffect, m_eShaderPass);
}

HRESULT CEffect_Mesh_Spiral::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Mesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STATIC, L"Com_Mesh_Spiral", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;
}

HRESULT CEffect_Mesh_Spiral::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CEffect_Mesh_Spiral* CEffect_Mesh_Spiral::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffect_Mesh_Spiral* pInstance = new CEffect_Mesh_Spiral(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CEffect_Mesh_AuraSphere Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CEffect_Mesh_Spiral::Free(void)
{
	CEffect_Mesh::Free();
	cout << "CEffect_Mesh_Spiral �Ҹ�!" << endl;
}

