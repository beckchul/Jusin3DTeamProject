#include "stdafx.h"
#include "Font_DefenseUnits.h"
#include "Renderer.h"
#include "InputDev.h"

USING(Engine)

CFont_DefenseUnits::CFont_DefenseUnits(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
{
}


CFont_DefenseUnits::~CFont_DefenseUnits()
{
}

HRESULT CFont_DefenseUnits::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(SCALE_WINCX * 128.f / 8.f, SCALE_WINCY * 128.f / 8.f, 1.f));
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &_vec3(SCALE_WINCX * 958.f, SCALE_WINCY * 102.f, 0.f));

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	return NOERROR;
}

int CFont_DefenseUnits::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	return 0;
}

void CFont_DefenseUnits::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHABLEND);

	// ToDo : 디펜스 타워 정보 얻어서 갱신할 것
	int iTemp[5] = { -1, 0 , 10 , 9, 0 };

	for (int i = 0; i < 5; ++i)
	{
		if (iTemp[i] & 0x80000000)
			continue;

		m_pTextureCom->SetUp_OnShader(pArgEffect, "g_BaseTexture", iTemp[i]);

		m_pTransformCom->Change_WorldMatrix(&_vec3(SCALE_WINCX * 8.f, 0.f, 0.f)); // 글자간 간격
		m_pTransformCom->SetUp_OnShader(pArgEffect, "g_matWorldViewProj", matView, matProj); // 위치 변경
		pArgEffect->CommitChanges();

		m_pBufferCom->Render_Buffer(); // 그리기
	}

	pArgEffect->EndPass();
}

HRESULT CFont_DefenseUnits::Add_Component(void)
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

CFont_DefenseUnits * CFont_DefenseUnits::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CFont_DefenseUnits* pInstance = new CFont_DefenseUnits(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CFont_DefenseUnits Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CFont_DefenseUnits::Free(void)
{
	COrthographicUI::Free();
}
