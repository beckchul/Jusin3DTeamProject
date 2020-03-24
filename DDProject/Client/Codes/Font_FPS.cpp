#include "stdafx.h"
#include "Font_FPS.h"
#include "Renderer.h"
#include "InputDev.h"
#include "PlayerObserver.h"

USING(Engine)

CFont_FPS::CFont_FPS(LPDIRECT3DDEVICE9 pGraphicDev)
	: CFont_Number(pGraphicDev)
	, m_iFps(0)
	, m_fTime(0.f)
	, m_bShowFPS(false)
{
}


CFont_FPS::~CFont_FPS()
{
}

HRESULT CFont_FPS::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 128.f / 2.f, SCALE_WINCY * 128.f / 2.f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 22.f, SCALE_WINCY * 83.f, 0.f));

	//// µ¥Ä«¸£Æ® ÁÂÇ¥°è -> È­¸éÁÂÇ¥°è
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	return NOERROR;
}

int CFont_FPS::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		if(m_bShowFPS)
			m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);
	}
		
	m_fTime += fTimeDelta;
	m_iFps++;

	if (m_fTime > 1.f)
	{
		m_fTime = 0.f;

		Get_Digits(m_iFps);
		m_iFps = 0;
	}

	if (CInputDev::GetInstance()->OnceKeyDown(DIK_P))
		m_bShowFPS = (m_bShowFPS == TRUE) ? FALSE : TRUE;

	return 0;
}

void CFont_FPS::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHATEST);

	CFont_Number::Draw_FontNumber(pArgEffect, 3.f);

	pArgEffect->EndPass();
}

HRESULT CFont_FPS::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Font_Number_Blue", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CFont_FPS::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", 0);
	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	return NOERROR;
}

CFont_FPS * CFont_FPS::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFont_FPS* pInstance = new CFont_FPS(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CFont_FPS Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CFont_FPS::Free(void)
{
	CFont_Number::Free();
}
