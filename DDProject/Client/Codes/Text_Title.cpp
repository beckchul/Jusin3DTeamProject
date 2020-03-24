#include "stdafx.h"
#include "Text_Title.h"
#include "Renderer.h"
#include "DataManager.h"

USING(Engine)

CText_Title::CText_Title(LPDIRECT3DDEVICE9 pGraphicDev, _int iTexture, _float fStart, _float fFinish)
	: COrthographicUI(pGraphicDev)
	, m_fTime(0.f)
	, m_fAlpha(0.f)
	, m_iTexture(iTexture)
	, m_fStartTime(fStart)
	, m_fFinishTime(fFinish)
{
}


CText_Title::~CText_Title()
{
}

HRESULT CText_Title::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 1024.f / 1.3f, SCALE_WINCY * 128.f / 1.3f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 528.f, SCALE_WINCY * 83.f, 0.f));

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	m_fAlpha = 1.f;


	return NOERROR;
}

int CText_Title::Update_GameObject(const float & fTimeDelta)
{
	m_fTime += fTimeDelta;

	if (m_fTime > m_fFinishTime)
	{
		return 1;
	}
	if (m_fTime < m_fStartTime)
	{
		return 0;
	}

	m_pTransformCom->Change_Information(CTransform::INFO_POSITION, &_vec3(0.f, -30.f * fTimeDelta, 0.f));
	m_fAlpha -= fTimeDelta / (m_fFinishTime - m_fStartTime);

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);
	}

	return 0;
}

void CText_Title::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHAMULTIPLY);

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();
}

HRESULT CText_Title::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Text_Title", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CText_Title::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", m_iTexture);
	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	return NOERROR;
}

CText_Title * CText_Title::Create(LPDIRECT3DDEVICE9 pGraphicDev, _int iTexture, _float fStart, _float fFinish)
{
	CText_Title* pInstance = new CText_Title(pGraphicDev, iTexture, fStart, fFinish);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CText_Title Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CText_Title::Free(void)
{
	COrthographicUI::Free();
}
