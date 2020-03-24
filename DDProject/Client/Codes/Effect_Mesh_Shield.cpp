#include "stdafx.h"
#include "Effect_Mesh_Shield.h"

#include "UI_Skill.h"
#include "Object_Manager.h"
#include "DataManager.h"

USING(Engine)

CEffect_Mesh_Shield::CEffect_Mesh_Shield(LPDIRECT3DDEVICE9 pGraphicDev)
	:CEffect_Mesh(pGraphicDev)
	, m_bIsCreateUI(FALSE)
	, m_eAuraType(AURA_END)
{
}

CEffect_Mesh_Shield::CEffect_Mesh_Shield(const CEffect_Mesh_Shield& rhs)
	: CEffect_Mesh(rhs)
	, m_pTextureMaskCom(rhs.m_pTextureMaskCom)
	, m_pTextureNormalCom(rhs.m_pTextureNormalCom)
	, m_bIsCreateUI(rhs.m_bIsCreateUI)
	, m_eAuraType(rhs.m_eAuraType)
{
	m_pTextureMaskCom->Add_Ref();
	m_pTextureNormalCom->Add_Ref();

	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Render", m_pRendererCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", m_pStaticMeshCom));
	m_pRendererCom->Add_Ref();
	m_pStaticMeshCom->Add_Ref();

	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_MaskTexture", m_pTextureMaskCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_NormalTexture", m_pTextureNormalCom));

	m_pTextureMaskCom->Add_Ref();
	m_pTextureNormalCom->Add_Ref();

	// For.Transform
	m_pTransformCom = ((Engine::CTransform*)Engine::CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Com_Transform"));
	if (nullptr == m_pTransformCom)
		return;
	m_mapComponent[Engine::CComponent::COM_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", m_pTransformCom));
	m_pTransformCom->Add_Ref();
	// Transform ������Ʈ�� �ٸ� �ּҰ��� ������ �Ҵ� 

	Reset_Effect();
}

CEffect_Mesh_Shield::~CEffect_Mesh_Shield(void)
{

}

CEffect_Mesh_Shield* CEffect_Mesh_Shield::Clone(void)
{
	return new CEffect_Mesh_Shield(*this);
}

HRESULT CEffect_Mesh_Shield::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_eShaderPass = EFFECT_MESH_MASK;
	//m_eShaderPass = EFFECT_MESH_ALPHABLEND;

	m_bFadeIn = TRUE;

	// m_vColor = _vec4(0.4f, 0.1f, 0.4f, 0.f);
	m_fUVTime = 0.f;
	//// UI
	//Engine::CGameObject* pGameObject = CUI_Skill::Create(m_pGraphicDev, 0);
	//if (nullptr == pGameObject)
	//	return E_FAIL;
	//CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", pGameObject);


	return NOERROR;
}

int CEffect_Mesh_Shield::Update_GameObject(const float & fTimeDelta)
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

	//if (m_bFadeIn && m_fUVTime <= 0.8f)
	//{		// UVTime�� ���������� �������� �ð��̴�, �̰� �������.
	//	m_vColor.w += fTimeDelta * 1.8f;
	//	if (m_vColor.w >= 1.f)
	//		m_vColor.w = 1.f;
	//	//cout << "Fade In -- Alpha : " << m_vColor.w << endl;
	//}

	//if (m_bFadeOut && m_fTime <= 2.f)		// 2�ʺ��� FadeOut
	//{
	//	m_vColor.w -= fTimeDelta * 0.5f;
	//	if (m_vColor.w <= 0.f)
	//		m_vColor.w = 0.f;
	//	//cout << "Mesh_Alpha : " << m_vColor.w << endl;
	//}

	// ���� Aura�� UI�� �̰��� �ƴ϶� StaticMesh-Aura���� ������ ��������

	//if (!m_bIsCreateUI)
	//{
	//	// �ӽ÷� AuraType�� SLOW��� �����س���.
	//	m_eAuraType = SLOW_AURA;

	//	switch (m_eAuraType) // ��ȣ : Switch������ ���ϰ� Create �Ҷ� 2��° ���ڷ� m_eAuraType�� �Ѱ��ָ� ��.
	//	{
	//	case SLOW_AURA:
	//		Engine::CGameObject* pGameObject = CUI_Skill::Create(m_pGraphicDev, m_eAuraType); 
	//		if (nullptr == pGameObject)
	//			return 0;
	//		_vec3 vPos = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION);
	//		pGameObject->Set_Pos(&_vec3(vPos.x, vPos.y + 30.f, vPos.z));
	//		CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", pGameObject);
	//		break;
	//	}
	//	m_bIsCreateUI = TRUE;
	//}

	return 0;
}

void CEffect_Mesh_Shield::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh_Effect(pArgEffect, m_eShaderPass);
}

HRESULT CEffect_Mesh_Shield::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Mesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STATIC, L"Com_Mesh_EnergyBall", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	// For.MaskTexture
	AddComponent(m_pTextureMaskCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_EnergyBall_Mask", Engine::CComponent::COM_STATIC, L"Com_MaskTexture");

	// For.NormalTexture
	AddComponent(m_pTextureNormalCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_EnergyBall_Normal", Engine::CComponent::COM_STATIC, L"Com_NormalTexture");

	return NOERROR;
}

HRESULT CEffect_Mesh_Shield::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pStaticMeshCom)
		return E_FAIL;


	m_pTextureMaskCom->SetUp_OnShader(pEffect, "g_MaskTexture", 0);
	m_pTextureNormalCom->SetUp_OnShader(pEffect, "g_SamplerNormalTexture", 0);

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);


	pEffect->SetVector("g_vColor", &m_vColor);
	pEffect->SetVector("g_vUV", (D3DXVECTOR4*)&m_vecUV);
	pEffect->SetVector("g_worldCameraPosition", (D3DXVECTOR4*)&m_pDataMgr->Get_CamPoistion());
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

CEffect_Mesh_Shield* CEffect_Mesh_Shield::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffect_Mesh_Shield* pInstance = new CEffect_Mesh_Shield(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CEffect_Mesh_Shield Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CEffect_Mesh_Shield::Free(void)
{
	Safe_Release(m_pTextureMaskCom);
	Safe_Release(m_pTextureNormalCom);

	CEffect_Mesh::Free();
	cout << "��� �Ҹ�!" << endl;
}

