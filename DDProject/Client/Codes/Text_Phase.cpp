#include "stdafx.h"
#include "Text_Phase.h"
#include "Renderer.h"
#include "InputDev.h"

USING(Engine)

CText_Phase::CText_Phase(LPDIRECT3DDEVICE9 pGraphicDev, _uint iTextureNum)
	: COrthographicUI(pGraphicDev)
	, m_iTextureNum(iTextureNum)
	, m_fTime(0.0f)
	, m_fAlpha(1.f)
	, m_bReadyToDie(false)
{
}


CText_Phase::~CText_Phase()
{
}

HRESULT CText_Phase::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 1024.f / 1.2f, SCALE_WINCY * 128.f / 1.2f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 512.f, SCALE_WINCY * 50.f, 0.f));

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	return NOERROR;
}

int CText_Phase::Update_GameObject(const float & fTimeDelta)
{
	m_fTime += fTimeDelta;

	if (m_fTime > 2.f)
		return 1;

	if (m_fTime < 1.f) // 내려오기
	{
		m_pTransformCom->Change_Information(CTransform::INFO_POSITION, &_vec3(0.f, -90.f * fTimeDelta, 0.f));
		m_pTransformCom->Change_Information(CTransform::INFO_SCALE, &_vec3(80.f * fTimeDelta, 10.f * fTimeDelta, 0.f));
	}
	else // 돌면서 사라지기
	{
		m_pTransformCom->Change_Information(CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(fTimeDelta * 1080.f)));
		m_pTransformCom->Change_Information(CTransform::INFO_SCALE, &_vec3(-SCALE_WINCX * 1024.f / 1.2f * fTimeDelta
			, -SCALE_WINCY * 128.f / 1.2f * fTimeDelta, 0.f));

		m_pTransformCom->Change_Information(CTransform::INFO_POSITION, &_vec3(-fTimeDelta * SCALE_WINCX * 512.f
			,0.1f * sqrtf(fTimeDelta * SCALE_WINCX * 512.f * 50.f), 0.f));
	}

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	return 0;
}

void CText_Phase::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHAMULTIPLY);

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();
}

HRESULT CText_Phase::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Text_Phase", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CText_Phase::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", m_iTextureNum);
	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	pEffect->SetFloat("g_fAlpha", m_fAlpha);

	return NOERROR;
}

CText_Phase * CText_Phase::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iTextureNum)
{
	CText_Phase* pInstance = new CText_Phase(pGraphicDev, iTextureNum);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CText_Phase Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CText_Phase::Free(void)
{
	COrthographicUI::Free();
}
