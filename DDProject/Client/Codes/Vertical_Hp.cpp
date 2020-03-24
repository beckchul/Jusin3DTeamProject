#include "stdafx.h"
#include "Vertical_Hp.h"
#include "Renderer.h"
#include "PlayerObserver.h"
#include "InfoSubject.h"

USING(Engine)

CVertical_Hp::CVertical_Hp(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
	, m_fTime(0.f)
{
}


CVertical_Hp::~CVertical_Hp()
{
}

HRESULT CVertical_Hp::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * -50.24f, SCALE_WINCY * 83.2f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 43.f, SCALE_WINCY * 669.f, 0.f));

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	m_pPlayerObserver = CPlayerObserver::Create();
	CInfoSubject::GetInstance()->Subscribe_Observer(m_pPlayerObserver);

	return NOERROR;
}

int CVertical_Hp::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	m_fTime += fTimeDelta;

	//CUI::Get_Position();

	return 0;
}

void CVertical_Hp::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	const PLAYER_INFO* pInfo = m_pPlayerObserver->Get_PlayerInfo(g_iClientIndex);
	if (nullptr == pInfo)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	float fRatio = (float)pInfo->tUnitInfo.iHp / pInfo->tUnitInfo.iHpMax; // 1 ~ 0

	pArgEffect->SetFloat("g_fRatio", (1 - fRatio) / 2.f); // 0.0(풀피) ~ 0.5(사망)

	// 그리기 1 (뒷배경)
	m_pTextureCom->SetUp_OnShader(pArgEffect, "g_BaseTexture", 0);
	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHATEST);
	m_pBufferCom->Render_Buffer();
	pArgEffect->EndPass();

	// 그리기 2 (색 + 마스크)
	m_pTextureCom->SetUp_OnShader(pArgEffect, "g_MaskTexture", 2);
	m_pTextureCom->SetUp_OnShader(pArgEffect, "g_ColorTexture", 3);
	m_pTextureCom->SetUp_OnShader(pArgEffect, "g_ColorTexture2", 4);
	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_MERGE);
	m_pBufferCom->Render_Buffer();
	pArgEffect->EndPass();

	// 그리기 3 (유리창 같은 레이어)
	m_pTextureCom->SetUp_OnShader(pArgEffect, "g_BaseTexture", 1);
	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_DARKREMOVE);
	m_pBufferCom->Render_Buffer();
	pArgEffect->EndPass();
}

HRESULT CVertical_Hp::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_VerticalBar", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CVertical_Hp::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);
	pEffect->SetFloat("g_fTime", m_fTime);
	pEffect->SetVector("g_fColor", &_vec4(1.f, 0.f, 0.f, 0.f));

	return NOERROR;
}

CVertical_Hp * CVertical_Hp::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CVertical_Hp* pInstance = new CVertical_Hp(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CVertical_Hp Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CVertical_Hp::Free(void)
{
	CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pPlayerObserver);
	Engine::Safe_Release(m_pPlayerObserver);

	COrthographicUI::Free();
}
