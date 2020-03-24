#include "stdafx.h"
#include "Text_Boss.h"
#include "Renderer.h"
#include "InfoSubject.h"
#include "WaveObserver.h"

USING(Engine)

CText_Boss::CText_Boss(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
	, m_pWaveObserver(nullptr)
{
}


CText_Boss::~CText_Boss()
{
}

HRESULT CText_Boss::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 512.f / 4.f, SCALE_WINCY * 128.f / 2.3f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 246.f, SCALE_WINCY * 55.f, 0.f));

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	m_pWaveObserver = CWaveObserver::Create(m_pGraphicDev);
	Engine::CInfoSubject::GetInstance()->Subscribe_Observer(m_pWaveObserver);

	return NOERROR;
}

int CText_Boss::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	WAVE_INFO tInfo = m_pWaveObserver->Get_WaveInfo();

	if (FALSE == tInfo.bBossExist)
		return 0;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	//CUI::Get_Position();

	return 0;
}

void CText_Boss::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHATEST);

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();
}

HRESULT CText_Boss::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Text_Boss", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CText_Boss::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CText_Boss * CText_Boss::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CText_Boss* pInstance = new CText_Boss(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CText_Boss Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CText_Boss::Free(void)
{
	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pWaveObserver);
	Engine::Safe_Release(m_pWaveObserver);

	COrthographicUI::Free();
}
