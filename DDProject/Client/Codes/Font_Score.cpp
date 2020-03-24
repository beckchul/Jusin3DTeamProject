#include "stdafx.h"
#include "Font_Score.h"
#include "Renderer.h"
#include "PlayerObserver.h"

USING(Engine)

CFont_Score::CFont_Score(LPDIRECT3DDEVICE9 pGraphicDev)
	: CFont_Number(pGraphicDev)
{
}


CFont_Score::~CFont_Score()
{
}

HRESULT CFont_Score::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 128.f / 6.f, SCALE_WINCY * 128.f / 6.f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 941.f, SCALE_WINCY * 743.f, 0.f));

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	return NOERROR;
}

int CFont_Score::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	const PLAYER_INFO* pPlayerInfo = m_pPlayerObserver->Get_PlayerInfo(g_iClientIndex); // 현재 클라이언트의 점수
	if (nullptr == pPlayerInfo)
		return 0;

	Get_Digits(pPlayerInfo->iPlayerScore);

	return 0;
}

void CFont_Score::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHABLEND);

	CFont_Number::Draw_FontNumber(pArgEffect);

	pArgEffect->EndPass();
}

HRESULT CFont_Score::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Font_Number_Green", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

CFont_Score * CFont_Score::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFont_Score* pInstance = new CFont_Score(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CFont_Score Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CFont_Score::Free(void)
{
	CFont_Number::Free();
}
