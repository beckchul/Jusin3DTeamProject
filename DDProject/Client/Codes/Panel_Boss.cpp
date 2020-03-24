#include "stdafx.h"
#include "Panel_Boss.h"
#include "Renderer.h"
#include "WaveObserver.h"
#include "InfoSubject.h"

USING(Engine)

CPanel_Boss::CPanel_Boss(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
	, m_bShowPanel_Boss(true)
	, m_pWaveObserver(nullptr)
	, m_pMaskTexture(nullptr)
	, m_pColorTexture(nullptr)
	, m_fRatio(0.f)
	, m_fTime(0.f)
{
}

CPanel_Boss::~CPanel_Boss()
{
}

HRESULT CPanel_Boss::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 1024.f / 2.2f, SCALE_WINCY * 128.f / 1.6f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 513.f, SCALE_WINCY * 58.f, 1.f));

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

int CPanel_Boss::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_fTime += fTimeDelta * 0.4f;

	WAVE_INFO tInfo = m_pWaveObserver->Get_WaveInfo();

	if (FALSE == tInfo.bBossExist)
		return 0;

	m_fRatio = tInfo.iBossHp / 5000.f;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	//CUI::Get_Position();

	return 0;
}

void CPanel_Boss::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_MASK);

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();
}

HRESULT CPanel_Boss::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Panel_EnemyKillBar", Engine::CComponent::COM_STATIC, L"Com_Texture");
	AddComponent(m_pMaskTexture, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_EnemyKillBarMask", Engine::CComponent::COM_STATIC, L"Com_Mask_Texture");
	AddComponent(m_pColorTexture, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_MeterTile", Engine::CComponent::COM_STATIC, L"Com_Color_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CPanel_Boss::Set_ConstantTable(LPD3DXEFFECT pEffect)
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
	pEffect->SetFloat("g_fRatio", m_fRatio); // 비율

	pEffect->SetVector("g_fColor", &_vec4(0.5f, 0.1f, 0.7f, 0.0f)); // 컬러

	return NOERROR;
}

CPanel_Boss * CPanel_Boss::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPanel_Boss* pInstance = new CPanel_Boss(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CPanel_Boss Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CPanel_Boss::Free(void)
{
	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pWaveObserver);
	Engine::Safe_Release(m_pWaveObserver);

	Engine::Safe_Release(m_pMaskTexture);
	Engine::Safe_Release(m_pColorTexture);

	COrthographicUI::Free();
}
