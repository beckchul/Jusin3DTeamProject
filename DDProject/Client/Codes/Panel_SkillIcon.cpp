#include "stdafx.h"
#include "Panel_SkillIcon.h"
#include "Renderer.h"
#include "SkillIcon.h"

USING(Engine)

CPanel_SkillIcon::CPanel_SkillIcon(LPDIRECT3DDEVICE9 pGraphicDev)
	: COrthographicUI(pGraphicDev)
{
	ZeroMemory(m_arrSkillIcon, sizeof(m_arrSkillIcon));
}

CPanel_SkillIcon::~CPanel_SkillIcon()
{
}

HRESULT CPanel_SkillIcon::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	_vec3 vecScale = { WINCX * (314 / 800.f), WINCY * (130 / 600.f), 1.f };
	_vec3 vecPosition = { vecScale.x / 2.f, WINCY - vecScale.y / 2.f, 0.f };
	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &vecScale);
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vecPosition);

	//// 데카르트 좌표계 -> 화면좌표계
	_vec3 vPos = m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
	vPos.y = -vPos.y;
	vPos.x -= WINCX / 2.f;
	vPos.y += WINCY / 2.f;
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, &vPos);

	// 스킬 아이콘들
	if (FAILED(CreateSkillIcon()))
		return E_FAIL;

	return NOERROR;
}

int CPanel_SkillIcon::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		// 먼저 출력
		for (int i = 0; i < SKILL_END; ++i)
		{
			if (nullptr != m_arrSkillIcon[i])
				m_arrSkillIcon[i]->Update_GameObject(fTimeDelta);
		}

		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_UI, this);
	}

	return 0;
}

void CPanel_SkillIcon::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	pArgEffect->CommitChanges();
	pArgEffect->BeginPass(UI_ALPHABLEND);

	m_pBufferCom->Render_Buffer(); // 그리기

	pArgEffect->EndPass();
}

HRESULT CPanel_SkillIcon::CreateSkillIcon(void)
{
	HANDLE hFile;

	DWORD dwByte = 0;

	hFile = CreateFile(L"../Resources/Data/UIData/SkillIcon/Pos.data", GENERIC_READ, NULL,NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	_vec2 vecPos;
	int i = 0;

	while (1)
	{
		ReadFile(hFile, vecPos, sizeof(_vec2), &dwByte, NULL);

		if (dwByte == 0)
		{
			CloseHandle(hFile);
			break;
		}

		m_arrSkillIcon[i] = CSkillIcon::Create(m_pGraphicDev);
		if (nullptr == m_arrSkillIcon[i])
			return E_FAIL;
		m_arrSkillIcon[i]->SetInfo(SKILL_TYPE(i), vecPos); 

		i++;
	}

	return NOERROR;
}

HRESULT CPanel_SkillIcon::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Panel_SkillIcon", Engine::CComponent::COM_STATIC, L"Com_Texture");
	
	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

HRESULT CPanel_SkillIcon::Set_ConstantTable(LPD3DXEFFECT pEffect)
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

CPanel_SkillIcon * CPanel_SkillIcon::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPanel_SkillIcon* pInstance = new CPanel_SkillIcon(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CPanel_SkillIcon Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CPanel_SkillIcon::Free(void)
{
	for (int i = 0; i < SKILL_END; ++i)
		Engine::Safe_Release(m_arrSkillIcon[i]);

	COrthographicUI::Free();
}
