#include "stdafx.h"
#include "Stage2_SkyBall.h"
#include "Renderer.h"
#include "DataManager.h"
#include "TextureManager.h"

USING(Engine)

CStage2_SkyBall::CStage2_SkyBall(LPDIRECT3DDEVICE9 pGraphicDev)
	: CStaticObject(pGraphicDev)
	, m_fTime(0.f)
{
}

CStage2_SkyBall::~CStage2_SkyBall()
{
}

HRESULT CStage2_SkyBall::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;
	
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &(_vec3(0.f, 0.f, 0.f)));
	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &(_vec3(100.f, 100.f, 100.f)));

	if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, L"../Resources/Texture/GlobalTexture/CloudMask0.tga", &m_pTexture)))
		MSG_BOX(L"SkyBall Texture initialize Failed");

	//m_fTime = RandFloat;

	return NOERROR;
}

int CStage2_SkyBall::FirstUpdate_GameObject(const float & fTimeDelta)
{
	return 0;
}

int CStage2_SkyBall::Update_GameObject(const float & fTimeDelta)
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

void CStage2_SkyBall::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	if (FAILED(Set_ConstantTable(pEffect)))
		return;

	pEffect->Begin(nullptr, 0);
	m_pStaticMeshCom->Render_Mesh_Effect(pEffect, 1);
	pEffect->End();


	Safe_Release(pEffect);
}

HRESULT CStage2_SkyBall::Add_Component(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Mesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STAGE2, L"Com_Mesh_SkyBoxBall", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	// For.Shader
	AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_SkyBox", Engine::CComponent::COM_STATIC, L"Com_Shader");
	
	return NOERROR;
}

HRESULT CStage2_SkyBall::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	pEffect->SetTexture("g_CloudMaskTexture", m_pTexture);

	return NOERROR;
}

CStage2_SkyBall * CStage2_SkyBall::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CStage2_SkyBall* pInstance = new CStage2_SkyBall(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CStage2_SkyBall Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CStage2_SkyBall::Free(void)
{
	CStaticObject::Free();
}
