#include "stdafx.h"
#include "Player_ID.h"
#include "Renderer.h"
#include "DataManager.h"

USING(Engine)

CPlayer_ID::CPlayer_ID(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex)
	: CPerspectiveUI(pGraphicDev)
	, m_iClientIdx(iIndex)
	, m_pDataMgr(Engine::CDataManager::GetInstance())
{
	m_pDataMgr->Add_Ref();
}


CPlayer_ID::~CPlayer_ID()
{
}

HRESULT CPlayer_ID::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	// Chat 이미지와 동일해야한다
	CHAR szTemp[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k',
		'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
		'w', 'x', 'y', 'z', ';', '\'', '=', '-', '?', '!', '^', '_',
		' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.' };

	for (_uint i = 0; i < strlen(szTemp); ++i)
		m_mapChat.insert(map<CHAR, _int>::value_type(szTemp[i], i));

	m_szMessage = m_pDataMgr->Get_Player_ID(m_iClientIdx);

	return NOERROR;
}

int CPlayer_ID::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DEFAULT, this);

	// 빌보드
	_matrix		matScale, matBill;
	D3DXMatrixScaling(&matScale, 3.f, 3.f, 1.f);

	matBill = m_pDataMgr->Get_ViewInverseMatrix();
	ZeroMemory(&matBill.m[3][0], sizeof(_vec3));

	m_pTransformCom->Set_WorldMatrix(&(matScale * matBill * *m_pTransformCom->Get_WorldMatrix()));

	return 0;
}

void CPlayer_ID::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == m_pShaderCom)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	pEffect->AddRef();
	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(UI_ALPHATEST);

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	D3DXVECTOR3 vecRight; // 오른쪽 벡터를 구해서 이동시키자
	memcpy(&vecRight, (*m_pTransformCom->Get_WorldMatrix()).m[0], sizeof(D3DXVECTOR3));
	D3DXVec3Normalize(&vecRight, &vecRight);

	float fInterval = 1.5f;
	_uint uSize = m_szMessage.length();

	m_pTransformCom->Change_WorldMatrix(&(-vecRight * fInterval * (uSize / 2.f))); // 글자를 가운데로 놓기 위함

	for (_uint i = 0; i < uSize; ++i)
	{
		m_pTransformCom->Change_WorldMatrix(&(vecRight * fInterval)); // 글자간 간격
																						//	
		m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);
		m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", m_mapChat[m_szMessage[i]]); // 이미지 변경

		pEffect->CommitChanges();
		m_pBufferCom->Render_Buffer();
	}

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CPlayer_ID::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Shader
	AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_UI", Engine::CComponent::COM_STATIC, L"Com_Shader")

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Font_Chat", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

CPlayer_ID * CPlayer_ID::Create(LPDIRECT3DDEVICE9 pGraphicDev, int iIndex)
{
	CPlayer_ID* pInstance = new CPlayer_ID(pGraphicDev, iIndex);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CPlayer_ID Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CPlayer_ID::Free(void)
{
	Engine::Safe_Release(m_pDataMgr);

	CPerspectiveUI::Free();
}
