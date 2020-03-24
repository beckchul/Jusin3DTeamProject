#include "stdafx.h"
#include "UI_MapTitle.h"
#include "Renderer.h"
#include "DataManager.h"
#include "ClientNetwork.h"

#include "InfoSubject.h"
#include "PlayerObserver.h"
#include "EffectMgr.h"

USING(Engine)

CUI_MapTitle::CUI_MapTitle(LPDIRECT3DDEVICE9 pGraphicDev, _uint iTexture)
	: CPerspectiveUI(pGraphicDev)
	, m_iTextureIdx(iTexture)
	, m_pDataMgr(Engine::CDataManager::GetInstance())
	, m_pPlayerObserver(nullptr)
{
	m_pDataMgr->Add_Ref();
}

CUI_MapTitle::~CUI_MapTitle()
{
}

HRESULT CUI_MapTitle::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pPlayerObserver = CPlayerObserver::Create();
	CInfoSubject::GetInstance()->Subscribe_Observer(m_pPlayerObserver);

	return NOERROR;
}

int CUI_MapTitle::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DEFAULT, this);

	// 빌보드
	_matrix		matScale, matBill;

	D3DXMatrixScaling(&matScale, 2048.f / 32.f, 128.f / 32.f, 1.f);

	matBill = m_pDataMgr->Get_ViewInverseMatrix();
	ZeroMemory(&matBill.m[3][0], sizeof(_vec3));

	m_pTransformCom->Set_WorldMatrix(&(matScale * matBill * *m_pTransformCom->Get_WorldMatrix()));

	for (int i = 0; i < MAX_CLIENT_NUM; ++i)
	{
		const PLAYER_INFO* pInfo = m_pPlayerObserver->Get_PlayerInfo(i);
		if (nullptr == pInfo)
			continue;

		_vec3 vecDir = pInfo->pTransform->Get_Info(CTransform::INFO_POSITION) - m_pTransformCom->Get_Info(CTransform::INFO_POSITION);

		_float fdist = D3DXVec3Length(&vecDir);

		if (fdist < 40.f)
		{
			if (i == g_iClientIndex) // 내 캐릭터인 경우
			{
				if (m_iTextureIdx == 0) // magus
					g_eCurrentScene = SCENE_STAGE;

				else if (m_iTextureIdx == 1) // The King's game
					g_eCurrentScene = SCENE_STAGE2;
			}
			else // 다른 캐릭터의 경우
			{
				_vec3 vPos = pInfo->pTransform->Get_Info(CTransform::INFO_POSITION);
				Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Recall", &_vec3(vPos.x, vPos.y + 15.f, vPos.z));
				CClientNetwork::GetInstance()->Delete_Player(i); // 플레이어 삭제~
				
			}
		}
	}

	return 0;
}

void CUI_MapTitle::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == m_pShaderCom)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	if (FAILED(Set_ConstantTable(pEffect)))
	{
		Safe_Release(pEffect);
		return;
	}

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(UI_ALPHATEST);

	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
}

HRESULT CUI_MapTitle::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Shader
	AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_UI", Engine::CComponent::COM_STATIC, L"Com_Shader");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Text_Title", Engine::CComponent::COM_STATIC, L"Com_Texture");
		
	return NOERROR;
}

HRESULT CUI_MapTitle::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", _uint(m_iTextureIdx));

	return NOERROR;
}

CUI_MapTitle * CUI_MapTitle::Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint iTexture)
{
	CUI_MapTitle* pInstance = new CUI_MapTitle(pGraphicDev, iTexture);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CUI_MapTitle Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CUI_MapTitle::Free(void)
{
	Engine::Safe_Release(m_pDataMgr);

	CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pPlayerObserver);
	Engine::Safe_Release(m_pPlayerObserver);

	CPerspectiveUI::Free();
}
