#include "stdafx.h"
#include "SkillIcon.h"
#include "Renderer.h"
#include "InputDev.h"
#include "PlayerObserver.h"
#include "InfoSubject.h"
#include "DataManager.h"

#include "Mouse_Manager.h"

USING(Engine)

CSkillIcon::CSkillIcon(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
	, m_eSkillType(SKILL_END)
	, m_iMPrequired(0)
	, m_bActivated(TRUE)
	, m_pPlayerObserver(nullptr)
	, m_ePlayerType(PLAYER_TYPE_END)
{	  
	ZeroMemory(m_pTexture_Skill, sizeof(m_pTexture_Skill));
}


CSkillIcon::~CSkillIcon()
{
}

HRESULT CSkillIcon::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	_vec3 vecScale = { WINCX * (40 / 800.f), WINCY * (40 / 600.f), 1.f };
	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &vecScale);

	m_pPlayerObserver = CPlayerObserver::Create();
	Engine::CInfoSubject::GetInstance()->Subscribe_Observer(m_pPlayerObserver);

	return NOERROR;
}

int CSkillIcon::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);

	const PLAYER_INFO* pInfo = m_pPlayerObserver->Get_PlayerInfo(g_iClientIndex);
	if (nullptr != pInfo)
	{
		m_ePlayerType = pInfo->ePlayerType; // 현재 클라이언트의 플레이어 종류

		vector<_int> vecRequireMP = CDataManager::GetInstance()->Get_RequiredMp(m_ePlayerType);
		m_iMPrequired = vecRequireMP[m_eSkillType];

		int iMP = pInfo->tUnitInfo.iMp;

		if (iMP < m_iMPrequired)
			m_bActivated = false;
		else
			m_bActivated = true;
	}

	return 0;
}

void CSkillIcon::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_COLORMULTIPLY);

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();
}

void CSkillIcon::SetInfo(SKILL_TYPE eType, _vec2 vecPos)
{
	m_eSkillType = eType;

	switch (m_eSkillType)
	{
	case SKILL_0:
		break;
	case SKILL_1:
		break;
	case SKILL_2:
		break;
	case SKILL_3:
		break;
	case SKILL_4:
		break;
	case SKILL_5:
		break;
	case SKILL_6:
		break;
	case SKILL_7:
		break;
	case SKILL_8:
		break;
	case SKILL_9:
		break;
	case SKILL_END:
		break;
	default:
		break;
	}
	

	_vec3 vecPosition = { WINCX * (vecPos.x / 800.f), WINCY * (vecPos.y / 600.f), 0.f };
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vecPosition);

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);
}

HRESULT CSkillIcon::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTexture_Skill[PLAYER_MAGE], Engine::CTexture*, SCENE_STATIC, L"Com_Texture_SkillIcon_Mage", Engine::CComponent::COM_STATIC, L"Com_Texture_Mage");
	AddComponent(m_pTexture_Skill[PLAYER_MONK], Engine::CTexture*, SCENE_STATIC, L"Com_Texture_SkillIcon_Monk", Engine::CComponent::COM_STATIC, L"Com_Texture_Monk");
	AddComponent(m_pTexture_Skill[PLAYER_HUNTRESS], Engine::CTexture*, SCENE_STATIC, L"Com_Texture_SkillIcon_Huntress", Engine::CComponent::COM_STATIC, L"Com_Texture_Huntress");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CSkillIcon::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect || nullptr == m_pTransformCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	if (PLAYER_TYPE_END == m_ePlayerType)
		return E_FAIL;

	m_pTexture_Skill[m_ePlayerType]->SetUp_OnShader(pEffect, "g_BaseTexture", m_eSkillType);

	if(FALSE == m_bActivated)
		pEffect->SetVector("g_fColor", &_vec4(1.f, 0.5f, 0.5f, 1.f));
	else
		pEffect->SetVector("g_fColor", &_vec4(1.f, 1.f, 1.f, 1.f));

	return NOERROR;
}

CSkillIcon * CSkillIcon::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CSkillIcon* pInstance = new CSkillIcon(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CSkillIcon Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CSkillIcon::Free(void)
{
	for (int i = 0; i < PLAYER_TYPE_END; ++i)
		Engine::Safe_Release(m_pTexture_Skill[i]);

	Engine::CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pPlayerObserver);
	Engine::Safe_Release(m_pPlayerObserver);

	COrthographicUI::Free();
}
