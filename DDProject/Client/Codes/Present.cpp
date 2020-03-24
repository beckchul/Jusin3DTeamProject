
#include "stdafx.h"
#include "Present.h"
#include "NavMgr.h"
#include "PlayerObserver.h"
#include "InfoSubject.h"
#include "SoundMgr.h"
#include "DataManager.h"

#include "EffectMgr.h"
#include "Effect.h"

CPresent::CPresent(LPDIRECT3DDEVICE9 pGraphicDev)
	: CStaticObject(pGraphicDev)
	, m_dwIndex(-1)
	, m_pNavMgr(CNavMgr::GetInstance())
	, m_bOnGround(false)
	, m_pPlayerObserver(nullptr)
	, m_PairPlayerMp(make_pair(0, 0))
{
	m_pNavMgr->Add_Ref();
}


CPresent::~CPresent()
{
}

HRESULT CPresent::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Information(CTransform::INFO_SCALE, &_vec3(0.2f, 0.2f, 0.2f));

	m_vecSpeed = _vec3(2.f, 2.f, 2.f);

	m_pPlayerObserver = CPlayerObserver::Create();
	CInfoSubject::GetInstance()->Subscribe_Observer(m_pPlayerObserver);

	int iRand = Random(2);
	switch (iRand)
	{
	case 0:
		m_listAttachedEffect = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Attached_GreenManaCrystal1", m_pTransformCom->Get_WorldMatrix());
		break;
	case 1:
		m_listAttachedEffect = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Attached_GreenManaCrystal2", m_pTransformCom->Get_WorldMatrix());
		break;
	}

	return NOERROR;
}

int CPresent::Update_GameObject(const float & fTimeDelta)
{
	m_pTransformCom->Change_Information(CTransform::INFO_ANGLE, &_vec3(0.f, D3DXToRadian(180.f * fTimeDelta), 0.f));

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if(-1 == m_dwIndex)
		m_dwIndex = CNavMgr::GetInstance()->FindNavMesh(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));
	
	Present_Fall(fTimeDelta); // 중력
	Present_Move(fTimeDelta); // 이동

	if (m_bDead)
	{
		if (m_listAttachedEffect.size() > 0)
			Engine::CEffectMgr::GetInstance()->Kill_TheseEffectlist(m_listAttachedEffect);
		
		int iRand = Random(2);
		switch (iRand)
		{
		case 0:
			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Get_GreenManaCrystal", &m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION));
			break;
		case 1:
			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Get_BlueManaCrystal", &m_pTransformCom->Get_Info(Engine::CTransform::INFO_POSITION));
			break;
		}
		return 1;
	}
	Follow_Player(fTimeDelta); // 플레이어 탐색

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_ALPHA, this);

	return 0;
}

void CPresent::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	/*if (nullptr == pArgEffect)
		return;

	if (FAILED(Set_ConstantTable(pArgEffect)))
		return;

	m_pStaticMeshCom->Render_Mesh(pArgEffect, uPassIdx);*/

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	if (FAILED(Set_ConstantTable(pEffect)))
		return;

	// 여기서 카메라 위치 전달. 포워드로 해야함.
	pEffect->SetVector("g_worldCameraPosition", (_vec4*)&CDataManager::GetInstance()->Get_CamPoistion());

	pEffect->Begin(nullptr, 0);

	m_pStaticMeshCom->Render_Mesh(pEffect);

	pEffect->End();


	Safe_Release(pEffect);
}

void CPresent::Present_Fall(const float & fTimeDelta)
{
	if (TRUE == m_bOnGround)
		return;

	m_vecSpeed.y -= 8.f * fTimeDelta;

	bool bDead = false;
	if (TRUE == m_pNavMgr->FallOnNavMesh(m_pTransformCom, m_dwIndex, bDead)) // Collide with NavMesh
	{
		m_pTransformCom->Change_Information(CTransform::INFO_POSITION, &_vec3(0.f, 2.f, 0.f));
		ZeroMemory(m_vecSpeed, sizeof(_vec3));
		m_bOnGround = true;

		if (TRUE == bDead) // 용암에 빠짐
			m_bDead = true;
	}
}

void CPresent::Present_Move(const float & fTimeDelta)
{
	_vec3 vecMove = { m_vDir.x * m_vecSpeed.x, m_vDir.y * m_vecSpeed.y, m_vDir.z * m_vecSpeed.z };
	vecMove *= fTimeDelta;

	m_pTransformCom->Change_Information(CTransform::INFO_POSITION, &vecMove);
	m_dwIndex = m_pNavMgr->MoveOnNavMesh(&m_pTransformCom->Get_Info(CTransform::INFO_POSITION), &vecMove, m_dwIndex);
}

void CPresent::Follow_Player(const float & fTimeDelta)
{
	if (FALSE == m_bOnGround)
		return;

	for (int i = 1; i < MAX_CLIENT_NUM; ++i)
	{
		const PLAYER_INFO*	pInfo = m_pPlayerObserver->Get_PlayerInfo(i);
		if (nullptr == pInfo)
			continue;

		// 플레이어 위치(+5) - 자신의 위치
		_vec3 vecDir = pInfo->pTransform->Get_Info(CTransform::INFO_POSITION) + _vec3(0.f, 5.f, 0.f) - m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
		float fDistance =  D3DXVec3Length(&vecDir);
		if (fDistance < 10.f)
		{
			CSoundMgr::GetInstance()->Play_Sound(L"Token_SuperMana.ogg", Engine::CHANNEL_PRESENT, 0.4f);
			m_bDead = true;

			m_PairPlayerMp = make_pair(i, pInfo->tUnitInfo.iMp + 30);
			CInfoSubject::GetInstance()->Add_Data(MESSAGE_PLAYER_MP, &m_PairPlayerMp);

			break;
		}
		else if (fDistance < 60.f)
		{
			D3DXVec3Length(&vecDir);
			m_pTransformCom->Change_Information(CTransform::INFO_POSITION, &(vecDir * 5.f * fTimeDelta));
		}
	}
}

HRESULT CPresent::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Shader
	AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_StaticMesh", Engine::CComponent::COM_STATIC, L"Com_Shader");

	// For.StaticMesh
	AddComponent(m_pStaticMeshCom, Engine::CStaticMesh*, SCENE_STATIC, L"Com_Mesh_ManaToken", Engine::CComponent::COM_STATIC, L"Com_Mesh");

	return NOERROR;

}

HRESULT CPresent::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pStaticMeshCom)
		return E_FAIL;

	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	return NOERROR;
}

CPresent * CPresent::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{
	CPresent* pInstance = new CPresent(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CPresent Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CPresent::Free(void)
{
	Engine::Safe_Release(m_pNavMgr);

	CInfoSubject::GetInstance()->UnSubscribe_Observer(m_pPlayerObserver);
	Engine::Safe_Release(m_pPlayerObserver);

	CInfoSubject::GetInstance()->Remove_Data(MESSAGE_PLAYER_MP, &m_PairPlayerMp);
		
	CStaticObject::Free();
}
