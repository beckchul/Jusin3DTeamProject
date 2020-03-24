#include "stdafx.h"
#include "Effect_Magnetic_Sphere.h"
#include "DataManager.h"

#include "CollisionSphere.h"
#include "CollisionManager.h"

USING(Engine)

CEffect_Magnetic_Sphere::CEffect_Magnetic_Sphere(LPDIRECT3DDEVICE9 pGraphicDev)
	:CEffect_Mesh(pGraphicDev)
	, m_pCollisionManager(CCollisionManager::GetInstance())
	, m_eState(PREVIEW)
	, m_bRender(TRUE)
	, m_iPos(0)
{
	m_pCollisionManager->Add_Ref();
}

CEffect_Magnetic_Sphere::CEffect_Magnetic_Sphere(const CEffect_Magnetic_Sphere& rhs)
	: CEffect_Mesh(rhs)
	, m_pTextureMaskCom(rhs.m_pTextureMaskCom)
	, m_pTextureNormalCom(rhs.m_pTextureNormalCom)
	, m_pCollisionManager(rhs.m_pCollisionManager)
	, m_eState(rhs.m_eState)
	, m_bRender(rhs.m_bRender)
	, m_iPos(rhs.m_iPos)
{
	m_pCollisionManager->Add_Ref();
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
	// Transform 컴포넌트만 다른 주소값을 가지게 할당 

	Reset_Effect();
}

CEffect_Magnetic_Sphere::~CEffect_Magnetic_Sphere(void)
{

}

CEffect_Magnetic_Sphere* CEffect_Magnetic_Sphere::Clone(void)
{
	return new CEffect_Magnetic_Sphere(*this);
}

void CEffect_Magnetic_Sphere::Set_InitMagnetic(_int iRound)
{
	m_fScale = 15.f;
	m_fCollisionScale = 1500.f;
	_int iTemp = iRound;
	iTemp--;
	while (iTemp > 3)
		--iTemp;

	if (m_iPos > 3)
		--m_iPos;

	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &m_pDataMgr->Get_MagneticInitPos(m_iPos));
	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(m_fScale, m_fScale, m_fScale));
	m_pCollisionSphere->Set_Scale(m_fCollisionScale);

	++m_iPos;

}

HRESULT CEffect_Magnetic_Sphere::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_eShaderPass = EFFECT_MESH_ALPHABLEND_CULLNONE;

	m_vRealColor = _vec4(0.2f, 0.3f, 0.6f, 0.7f);
	m_vPreviewColor = _vec4(0.5f, 0.5f, 0.5f, 0.5f);

	m_vColor = m_vPreviewColor;

	m_fUVTime = 0.f;

	return NOERROR;
}

int CEffect_Magnetic_Sphere::Update_GameObject(const float & fTimeDelta)
{
	if (m_bUseTimeDead && m_fTime <= 0.f || m_bDead)		// 시간 설정시
		return 1;

	Perform_Function(fTimeDelta);		// Bool형 변수에 따른 기능 수행.

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (g_eLastMan_Progress == LASTMAN_ROUND_FINISH || !m_bRender)
		return 0;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_EFFECT, this);

	// 알파 블렌딩을 위한 거리에 따른 Sorting
	Compute_ViewZ(&m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	m_fUVTime += fTimeDelta;				// UV애니메이션 사용시 점진적인 시간값 넣어줌
	m_fTime -= fTimeDelta;					// 시간 소멸 사용시 , 시간 줄여줌

	switch (m_eState)
	{
	case ACTIVE:
	{
		m_vColor = m_vRealColor;
		m_fScale -= fTimeDelta * 0.01f;
		if (m_fScale <= 0.f)
			m_fScale = 0.f;
		m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(m_fScale, m_fScale, m_fScale));
		
		m_fCollisionScale -= fTimeDelta * 1.f;
		if (m_fCollisionScale <= 0.f)
			m_fCollisionScale = 0.f;
		m_pCollisionSphere->Set_Scale(m_fCollisionScale);
		break;
	}
	case PREVIEW:
	{
		m_vColor = m_vPreviewColor;
		m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(7.f, 7.f, 7.f));
		m_pCollisionSphere->Set_Scale(700.f);

		break;
	}
	}
	
	m_pDataMgr->Set_MageticRadius(m_fCollisionScale);

	return 0;
}

void CEffect_Magnetic_Sphere::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect || !m_bRender)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh_Effect(pArgEffect, m_eShaderPass);
}

HRESULT CEffect_Magnetic_Sphere::Add_Component(void)
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

	AddCollisionSphere(m_pCollisionSphere, SCENE_STATIC, L"Com_CollisionSphere", Engine::CComponent::COM_DYNAMIC, L"Com_Col_Magnetic", 1500.f, m_pTransformCom->Get_WorldMatrix(), 1.f);
	m_pCollisionManager->Add_Component(L"Com_Col_Magnetic", m_pCollisionSphere);

	return NOERROR;
}

HRESULT CEffect_Magnetic_Sphere::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CEffect_Magnetic_Sphere* CEffect_Magnetic_Sphere::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffect_Magnetic_Sphere* pInstance = new CEffect_Magnetic_Sphere(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CEffect_Magnetic_Sphere Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CEffect_Magnetic_Sphere::Free(void)
{
	Safe_Release(m_pTextureMaskCom);
	Safe_Release(m_pTextureNormalCom);

	m_pCollisionManager->Delete_Component(L"Com_Col_Magnetic", m_pCollisionSphere);
	Engine::Safe_Release(m_pCollisionSphere);

	Engine::Safe_Release(m_pCollisionManager);
	CEffect_Mesh::Free();
	cout << "자기장 소멸!" << endl;
}

