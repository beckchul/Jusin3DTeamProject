#include "stdafx.h"
#include "Font_KillEnemies.h"
#include "Renderer.h"
#include "InputDev.h"
#include "WaveObserver.h"
#include "InfoSubject.h"

USING(Engine)

CFont_KillEnemies::CFont_KillEnemies(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
	, m_pWaveObserver(nullptr)
{
	ZeroMemory(&m_tWaveInfo, sizeof(WAVE_INFO));
}


CFont_KillEnemies::~CFont_KillEnemies()
{
}

HRESULT CFont_KillEnemies::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 128.f / 3.5f, SCALE_WINCY * 128.f / 3.5f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 720.f, SCALE_WINCY * 27.f, 0.f));

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	m_pWaveObserver = CWaveObserver::Create(m_pGraphicDev);
	CInfoSubject::GetInstance()->Subscribe_Observer(m_pWaveObserver);

	return NOERROR;
}

int CFont_KillEnemies::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	m_tWaveInfo = m_pWaveObserver->Get_WaveInfo();

	if (COMBAT_PHASE != m_tWaveInfo.ePhaseType || m_tWaveInfo.bBossExist)
		return 0;

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	return 0;
}

void CFont_KillEnemies::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	pArgEffect->BeginPass(UI_ALPHABLEND);

	int iKillUnit = m_tWaveInfo.iTotalUnits - m_tWaveInfo.iCurUnits;
	int iTotalUnit = m_tWaveInfo.iTotalUnits;

	int iTemp[5] = { iKillUnit / 10, iKillUnit % 10 , 10 , iTotalUnit / 10, iTotalUnit % 10 };

	if (iTemp[0] == 0)
		iTemp[0] = -1;
	if (iTemp[3] == 0)
		iTemp[3] = -1;

	for (int i = 0; i < 5; ++i)
	{
		if (iTemp[i] & 0x80000000)
			continue;

		m_pTextureCom->SetUp_OnShader(pArgEffect, "g_BaseTexture", iTemp[i]);

		m_pTransformCom->Change_WorldMatrix(&_vec3(SCALE_WINCX * 20.f, 0.f, 0.f)); // 글자간 간격
		m_pTransformCom->SetUp_OnShader(pArgEffect, "g_matWorldViewProj", matView, matProj); // 위치 변경
		pArgEffect->CommitChanges();

		m_pBufferCom->Render_Buffer(); // 그리기
	}

	pArgEffect->EndPass();
}

HRESULT CFont_KillEnemies::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Font_Number_Skin", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

CFont_KillEnemies * CFont_KillEnemies::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFont_KillEnemies* pInstance = new CFont_KillEnemies(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CFont_KillEnemies Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CFont_KillEnemies::Free(void)
{
	CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pWaveObserver);
	Engine::Safe_Release(m_pWaveObserver);

	COrthographicUI::Free();
}
