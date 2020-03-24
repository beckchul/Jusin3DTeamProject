#include "stdafx.h"
#include "Text_ToLobby.h"
#include "Renderer.h"

USING(Engine)

CText_ToLobby::CText_ToLobby(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
	, m_fTime(9.f)
{
}


CText_ToLobby::~CText_ToLobby()
{
}

HRESULT CText_ToLobby::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 1024.f / 1.5f, SCALE_WINCY * 64.f / 1.5f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 534.f, SCALE_WINCY * 185.f, 0.f));

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	return NOERROR;
}

int CText_ToLobby::Update_GameObject(const float & fTimeDelta)
{
	m_fTime -= fTimeDelta;

	if (m_fTime < 0.f)
		return 1;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	//CUI::Get_Position();

	return 0;
}

void CText_ToLobby::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHABLEND);

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();
}

HRESULT CText_ToLobby::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Text_ToLobby", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CText_ToLobby::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", _int(m_fTime));
	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	return NOERROR;
}

CText_ToLobby * CText_ToLobby::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CText_ToLobby* pInstance = new CText_ToLobby(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CText_ToLobby Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CText_ToLobby::Free(void)
{
	COrthographicUI::Free();
}
