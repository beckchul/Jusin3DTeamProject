#include "stdafx.h"
#include "Font_ActivateCrystal.h"
#include "Renderer.h"
#include "DataManager.h"
#include "PlayerObserver.h"
#include "InfoSubject.h"

CFont_ActivateCrystal::CFont_ActivateCrystal(LPDIRECT3DDEVICE9 pGraphicDev)
	: CFont_Perspective(pGraphicDev)
	, m_pDataMgr(Engine::CDataManager::GetInstance())
{
	m_pDataMgr->Add_Ref();
}


CFont_ActivateCrystal::~CFont_ActivateCrystal()
{
}

HRESULT CFont_ActivateCrystal::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(0.f, 35.f, 0.f));

	m_pPlayerObserver = CPlayerObserver::Create();
	Engine::CInfoSubject::GetInstance()->Subscribe_Observer(m_pPlayerObserver);

	return NOERROR;
}

int CFont_ActivateCrystal::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	// ºôº¸µå
	D3DXMATRIX		matScale, matBill;

	D3DXMatrixScaling(&matScale, SCALE_WINCX * 1024.f / 24.f , SCALE_WINCY * 128.f / 24.f, 1.f);

	matBill = m_pDataMgr->Get_ViewInverseMatrix();
	ZeroMemory(&matBill.m[3][0], sizeof(_vec3));

	m_pTransformCom->Set_WorldMatrix(&(matScale * matBill * *m_pTransformCom->Get_WorldMatrix()));

	const PLAYER_INFO* pInfo = m_pPlayerObserver->Get_PlayerInfo(g_iClientIndex);
	if (nullptr == pInfo)
		return 0;

	if (nullptr != m_pRendererCom)
	{
		float fDistance = D3DXVec3Length(&(pInfo->pTransform->Get_Info(CTransform::INFO_POSITION) - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));

		if (fDistance < 60.f)
		{
			CGameObject::Compute_ViewZ(&m_pTransformCom->Get_Info(CTransform::INFO_POSITION));
			m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_ALPHA, this);
		}
			
	}

	return 0;
}

void CFont_ActivateCrystal::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == m_pShaderCom)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	if (FAILED(Set_ConstantTable(pEffect)))
	{
		Safe_Release(pEffect);
		return;
	}

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(UI_ALPHATEST);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CFont_ActivateCrystal::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Shader
	AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_UI", Engine::CComponent::COM_STATIC, L"Com_Shader");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Text_ActivateCrystal", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CFont_ActivateCrystal::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", 0);

	return NOERROR;
}

CFont_ActivateCrystal * CFont_ActivateCrystal::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFont_ActivateCrystal* pInstance = new CFont_ActivateCrystal(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CFont_ActivateCrystal Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CFont_ActivateCrystal::Free(void)
{
	Engine::Safe_Release(m_pDataMgr);

	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pPlayerObserver);
	Engine::Safe_Release(m_pPlayerObserver);

	CFont_Perspective::Free();
}
