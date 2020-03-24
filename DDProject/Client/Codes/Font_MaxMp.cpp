#include "stdafx.h"
#include "Font_MaxMp.h"
#include "Renderer.h"
#include "InputDev.h"
#include "PlayerObserver.h"

USING(Engine)

CFont_MaxMp::CFont_MaxMp(LPDIRECT3DDEVICE9 pGraphicDev)
	: CFont_Number(pGraphicDev)
{
}


CFont_MaxMp::~CFont_MaxMp()
{
}

HRESULT CFont_MaxMp::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 128.f / 6.f, SCALE_WINCY * 128.f / 6.f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 93.f, SCALE_WINCY * 718.f, 0.f));

	//// µ¥Ä«¸£Æ® ÁÂÇ¥°è -> È­¸éÁÂÇ¥°è
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	return NOERROR;
}

int CFont_MaxMp::Update_GameObject(const float & fTimeDelta)
{
	const PLAYER_INFO* pPlayerInfo = m_pPlayerObserver->Get_PlayerInfo(g_iClientIndex);
	if (nullptr == pPlayerInfo)
		return 0;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	int iCount = Get_Digits(pPlayerInfo->tUnitInfo.iMpMax);

	if (3 == iCount)
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 88.f - WINCX / 2.f, -SCALE_WINCY * 718.f + WINCY / 2.f, 0.f));
	else if (2 == iCount)
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 93.f - WINCX / 2.f, -SCALE_WINCY * 718.f + WINCY / 2.f, 0.f));
	else if (1 == iCount)
		m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 98.f - WINCX / 2.f, -SCALE_WINCY * 718.f + WINCY / 2.f, 0.f));

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	return 0;
}

void CFont_MaxMp::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHABLEND);

	CFont_Number::Draw_FontNumber(pArgEffect);

	pArgEffect->EndPass();
}

HRESULT CFont_MaxMp::Add_Component(void)
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

CFont_MaxMp * CFont_MaxMp::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFont_MaxMp* pInstance = new CFont_MaxMp(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CFont_MaxMp Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CFont_MaxMp::Free(void)
{
	CFont_Number::Free();
}
