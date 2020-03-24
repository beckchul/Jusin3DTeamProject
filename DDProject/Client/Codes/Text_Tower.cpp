#include "stdafx.h"
#include "Text_Tower.h"
#include "Renderer.h"
#include "InputDev.h"

USING(Engine)

CText_Tower::CText_Tower(LPDIRECT3DDEVICE9 pGraphicDev, _uint iTextureNum)
	: COrthographicUI(pGraphicDev)
	, m_iTextureNum(iTextureNum)
	, m_fTime(0.0f)
	, m_fAlpha(0.f)
	, m_bReadyToDie(false)
{
}


CText_Tower::~CText_Tower()
{
}

HRESULT CText_Tower::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 2048.f / 1.8f, SCALE_WINCY * 128.f / 1.8f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 512.f, SCALE_WINCY * 576.f, 0.f));

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	return NOERROR;
}

int CText_Tower::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	Appear(fTimeDelta);

	if (m_bDead)
	{
		// 죽음의 메아리 발동
		return Disappear(fTimeDelta);
	}
	return 0;
}

void CText_Tower::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHAMULTIPLY);

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();
}

HRESULT CText_Tower::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Text_Tower", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CText_Tower::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

void CText_Tower::Appear(const float & fTimeDelta) // 0.5초동안
{
	if (m_fTime > 0.5 || m_bReadyToDie)
	{
		m_bReadyToDie = true; // 죽을 준비 완료
		return;
	}
		
	m_fTime += fTimeDelta;
	m_fAlpha += fTimeDelta * 2.f;
	m_pTransformCom->Change_Information(CTransform::INFO_SCALE, &(_vec3(16.f, 1.f, 1.f) * -fTimeDelta * 20.f));
}

int CText_Tower::Disappear(const float & fTimeDelta) 
{
	if (m_fTime < 0.f) // 소멸
		return 1;

	if (FALSE == m_bReadyToDie) // 아직 죽기엔 이르다
		return 0;

	m_fTime -= fTimeDelta;
	m_fAlpha -= fTimeDelta * 2.f;
	m_pTransformCom->Change_Information(CTransform::INFO_SCALE, &(_vec3(16.f, 1.f, 1.f) * -fTimeDelta * 40.f));

	return 0;
}

CText_Tower * CText_Tower::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iTextureNum)
{
	CText_Tower* pInstance = new CText_Tower(pGraphicDev, iTextureNum);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CText_Tower Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CText_Tower::Free(void)
{
	COrthographicUI::Free();
}
