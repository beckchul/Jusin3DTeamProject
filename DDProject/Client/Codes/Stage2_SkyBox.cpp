#include "stdafx.h"
#include "Stage2_SkyBox.h"
#include "Renderer.h"
#include "DataManager.h"

USING(Engine)

CStage2_SkyBox::CStage2_SkyBox(LPDIRECT3DDEVICE9 pGraphicDev)
	: CStaticObject(pGraphicDev)
	, m_fTime(0.f)
{
}

CStage2_SkyBox::~CStage2_SkyBox()
{
}

HRESULT CStage2_SkyBox::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &(_vec3(0.f, 0.f, 0.f)));
	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &(_vec3(1.f, 1.f, 1.f)));

	//m_fTime = RandFloat;

	return NOERROR;
}

int CStage2_SkyBox::FirstUpdate_GameObject(const float & fTimeDelta)
{
	return 0;
}

int CStage2_SkyBox::Update_GameObject(const float & fTimeDelta)
{
	//  뷰행렬(카메라 월드행렬의 역행렬)을 얻어온다.
	_matrix			matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, 0, &matView); // 카메라의 월드행렬

	m_pTransformCom->Set_Information(Engine::CTransform::INFO_POSITION, (_vec3*)&matView.m[3][0]);

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_pTransformCom->Rotation(Engine::CTransform::ANGLE_Y, fTimeDelta * 0.1f);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_ALPHA, this);
	}
	

	return 0;
}

void CStage2_SkyBox::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	if (FAILED(Set_ConstantTable(pEffect)))
		return;

	pEffect->Begin(nullptr, 0);
	m_pStaticMeshCom->Render_Mesh_Effect(pEffect, 0);
	pEffect->End();


	Safe_Release(pEffect);
}

HRESULT CStage2_SkyBox::Add_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Mesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STAGE2, L"Com_Mesh_SkyBox", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	// For.Shader
	AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_SkyBox", Engine::CComponent::COM_STATIC, L"Com_Shader");

	return NOERROR;
}

HRESULT CStage2_SkyBox::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	return NOERROR;
}

CStage2_SkyBox * CStage2_SkyBox::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage2_SkyBox* pInstance = new CStage2_SkyBox(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CStage2_SkyBox Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CStage2_SkyBox::Free(void)
{
	CStaticObject::Free();
}
