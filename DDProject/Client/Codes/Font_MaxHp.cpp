#include "stdafx.h"
#include "Font_MaxHp.h"
#include "Renderer.h"
#include "PlayerObserver.h"

USING(Engine)

CFont_MaxHp::CFont_MaxHp(LPDIRECT3DDEVICE9 pGraphicDev)
	: CFont_Number(pGraphicDev)
{
}


CFont_MaxHp::~CFont_MaxHp()
{
}

HRESULT CFont_MaxHp::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 128.f / 6.f, SCALE_WINCY * 128.f / 6.f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 14.f, SCALE_WINCY * 718.f, 0.f));

	//// µ¥Ä«¸£Æ® ÁÂÇ¥°è -> È­¸éÁÂÇ¥°è
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	return NOERROR;
}

int CFont_MaxHp::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	const PLAYER_INFO* pPlayerInfo = m_pPlayerObserver->Get_PlayerInfo(g_iClientIndex);
	if (nullptr == pPlayerInfo)
		return 0;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	int iCount = Get_Digits(pPlayerInfo->tUnitInfo.iHpMax);

	if (3 == iCount)
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 14.f - WINCX / 2.f, -SCALE_WINCY * 718.f + WINCY / 2.f, 0.f));
	else if (2 == iCount)
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 19.f - WINCX / 2.f, -SCALE_WINCY * 718.f + WINCY / 2.f, 0.f));
	else if (1 == iCount)
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 24.f - WINCX / 2.f, -SCALE_WINCY * 718.f + WINCY / 2.f, 0.f));

	return 0;
}

void CFont_MaxHp::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHABLEND);

	CFont_Number::Draw_FontNumber(pArgEffect);

	pArgEffect->EndPass();
}

HRESULT CFont_MaxHp::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Font_Number_Red", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

CFont_MaxHp * CFont_MaxHp::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFont_MaxHp* pInstance = new CFont_MaxHp(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CFont_MaxHp Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CFont_MaxHp::Free(void)
{
	CFont_Number::Free();
}
