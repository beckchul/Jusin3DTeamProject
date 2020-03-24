#include "stdafx.h"
#include "Effect_Mesh_FogGroup4.h"
#include "DataManager.h"

USING(Engine)

CEffect_Mesh_FogGroup4::CEffect_Mesh_FogGroup4(LPDIRECT3DDEVICE9 pGraphicDev)
	:CEffect_Mesh(pGraphicDev)
{
}

CEffect_Mesh_FogGroup4::CEffect_Mesh_FogGroup4(const CEffect_Mesh_FogGroup4& rhs)
	: CEffect_Mesh(rhs)
	, m_pTextureMaskCom(rhs.m_pTextureMaskCom)
{
	m_pTextureMaskCom->Add_Ref();


	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Render", m_pRendererCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Mesh", m_pStaticMeshCom));
	m_pRendererCom->Add_Ref();
	m_pStaticMeshCom->Add_Ref();

	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_MaskTexture", m_pTextureMaskCom));
	m_pTextureMaskCom->Add_Ref();

	// 새로운 주소 할당
	m_pTransformCom = ((Engine::CTransform*)Engine::CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Com_Transform"));
	if (nullptr == m_pTransformCom)
		return;
	m_mapComponent[Engine::CComponent::COM_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", m_pTransformCom));
	m_pTransformCom->Add_Ref();

}

CEffect_Mesh_FogGroup4::~CEffect_Mesh_FogGroup4(void)
{

}

CEffect_Mesh_FogGroup4* CEffect_Mesh_FogGroup4::Clone(void)
{
	return new CEffect_Mesh_FogGroup4(*this);
}

HRESULT CEffect_Mesh_FogGroup4::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;
	m_eShaderPass = EFFECT_MESH_FOGMASK;

	m_pTransformCom->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(90.f)));

	return NOERROR;
}

int CEffect_Mesh_FogGroup4::Update_GameObject(const float & fTimeDelta)
{
	if (m_bUseTimeDead && m_fTime <= 0.f || m_bDead)		// 시간 설정시
		return 1;

	m_fStartTime -= fTimeDelta;
	if (m_bIsStartTimeControl && 0.f <= m_fStartTime)
		return 0;

	Perform_Function(fTimeDelta);		// Bool형 변수에 따른 기능 수행.



	Engine::CGameObject::Update_GameObject(fTimeDelta);


	//// BillBoard----------------------------------------------------------------------------------------
	//_matrix		matBill;
	//D3DXMatrixIdentity(&matBill);

	//matBill = m_pDataMgr->Get_ViewInverseMatrix();
	//ZeroMemory(&matBill.m[3][0], sizeof(D3DXVECTOR3));

	//// 원래 있던 Transform의 정보
	//_vec3 vPos = m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION);
	//_vec3 vScale = m_pTransformCom->Get_Info(Engine::CTransform::INFO_SCALE);
	//_vec3 vRot = m_pTransformCom->Get_Info(Engine::CTransform::INFO_ANGLE);

	//_matrix	matScale, matRotZ, matTrans;

	//D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z);
	//D3DXMatrixRotationZ(&matRotZ, vRot.z);
	//D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);

	//matBill = matScale *  matRotZ * matBill * matTrans;

	//m_pTransformCom->Set_WorldMatrix(&(matBill));
	////----------------------------------------------------------------------------------------------------


	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_EFFECT, this);

	// 알파 블렌딩을 위한 거리에 따른 Sorting
	Compute_ViewZ(&m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	m_fUVTime += fTimeDelta;
	m_fTime -= fTimeDelta;

	if (m_bFadeOut && m_fTime <= 2.f)
	{
		m_vColor.w -= fTimeDelta * 0.5f;
		if (m_vColor.w <= 0.f)
			m_vColor.w = 0.f;
	}

	return 0;
}

void CEffect_Mesh_FogGroup4::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == pArgEffect)
		return;

	if (m_bIsStartTimeControl && 0.f <= m_fStartTime)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh_Effect(pArgEffect, m_eShaderPass);
}

HRESULT CEffect_Mesh_FogGroup4::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Mesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STATIC, L"Com_Mesh_FogPlane_Group4", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	// For.MaskTexture
	AddComponent(m_pTextureMaskCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_FogMerge", Engine::CComponent::COM_STATIC, L"Com_MaskTexture");

	return NOERROR;
}

HRESULT CEffect_Mesh_FogGroup4::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pStaticMeshCom)
		return E_FAIL;

	m_pTextureMaskCom->SetUp_OnShader(pEffect, "g_MaskTexture", 0);

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

CEffect_Mesh_FogGroup4* CEffect_Mesh_FogGroup4::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CEffect_Mesh_FogGroup4* pInstance = new CEffect_Mesh_FogGroup4(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CEffect_Mesh_FogGroup4 Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CEffect_Mesh_FogGroup4::Free(void)
{
	Safe_Release(m_pTextureMaskCom);

	CEffect_Mesh::Free();
	cout << "CEffect_Mesh_FogGroup4 소멸!" << endl;
}

