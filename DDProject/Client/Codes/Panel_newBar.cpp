#include "stdafx.h"
#include "Panel_newBar.h"
#include "Renderer.h"
#include "PlayerObserver.h"
#include "InfoSubject.h"

USING(Engine)

CPanel_newBar::CPanel_newBar(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
	, m_bShowPanel_newBar(true)
	, m_pPlayerObserver(nullptr)
	, m_pMaskTexture(nullptr)
{
}


CPanel_newBar::~CPanel_newBar()
{
}

HRESULT CPanel_newBar::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 512.f / 2.6f, SCALE_WINCY * 128.f / 2.6f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 472.f, SCALE_WINCY * 693.f, 1.f));

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	m_pPlayerObserver = CPlayerObserver::Create();
	Engine::CInfoSubject::GetInstance()->Subscribe_Observer(m_pPlayerObserver);

	return NOERROR;
}

int CPanel_newBar::Update_GameObject(const float & fTimeDelta)
{
	const PLAYER_INFO* pInfo = m_pPlayerObserver->Get_PlayerInfo(g_iClientIndex);
	if (nullptr == pInfo)
		return 0;

		/*if (CInputDev::GetInstance()->OnceKeyDown(DIK_NUMPAD0))
		{
			m_fTime = 0.f;
			m_bShowPanel_newBar = true;
		}

		if (FALSE == m_bShowPanel_newBar)
			return 0;
		m_fTime += fTimeDelta * 0.5f;*/


	if (pInfo->eMainState == PLAYER_TOWER_SUMMONING)
	{
		m_bShowPanel_newBar = true;
		m_fTime += fTimeDelta * 0.5f;
	}
	else
	{
		m_bShowPanel_newBar = false;
		m_fTime = 0.f;
	}

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		if(m_bShowPanel_newBar)
			m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);
	}

	//CUI::Get_Position();

	return 0;
}

void CPanel_newBar::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_MASK);

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();
}

HRESULT CPanel_newBar::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Panel_newBar", Engine::CComponent::COM_STATIC, L"Com_Texture");
	AddComponent(m_pMaskTexture, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_newBarMask", Engine::CComponent::COM_STATIC, L"Com_Mask_Texture");
	AddComponent(m_pColorTexture, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_MeterTile", Engine::CComponent::COM_STATIC, L"Com_Color_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CPanel_newBar::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", 0);
	m_pMaskTexture->SetUp_OnShader(pEffect, "g_MaskTexture", 0);
	m_pColorTexture->SetUp_OnShader(pEffect, "g_ColorTexture", 0);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	pEffect->SetFloat("g_fTime", m_fTime);
	pEffect->SetFloat("g_fRatio", m_fTime);

	pEffect->SetVector("g_fColor", &_vec4(1.f, 0.2f, 0.f, 0.f));

	return NOERROR;
}

CPanel_newBar * CPanel_newBar::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPanel_newBar* pInstance = new CPanel_newBar(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CPanel_newBar Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CPanel_newBar::Free(void)
{
	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pPlayerObserver);
	Engine::Safe_Release(m_pPlayerObserver);

	Engine::Safe_Release(m_pMaskTexture);
	Engine::Safe_Release(m_pColorTexture);

	COrthographicUI::Free();
}
