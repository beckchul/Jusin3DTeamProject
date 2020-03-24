#include "stdafx.h"
#include "Effect_Trail.h"
#include "Defines.h"

CEffect_Trail::CEffect_Trail(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CEffect(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_pmatPlayerInfo(nullptr)
	, m_pmatWeaponRef(nullptr)
	, m_fAccTime(0.f)
	, m_iTrailCount(0)
	, m_fTrailSize(0.f)
	, m_fTrailEmitRate(0.f)
{

}

CEffect_Trail::CEffect_Trail(const CEffect_Trail& rhs)
	: Engine::CEffect(rhs)
	, m_pRendererCom(rhs.m_pRendererCom)
	, m_pShaderCom(rhs.m_pShaderCom)
	, m_pTextureCom(rhs.m_pTextureCom)
	, m_pBufferCom(rhs.m_pBufferCom)
	, m_pmatPlayerInfo(rhs.m_pmatPlayerInfo)
	, m_pmatWeaponRef(rhs.m_pmatWeaponRef)
	, m_fAccTime(rhs.m_fAccTime)
	, m_iTrailCount(rhs.m_iTrailCount)
	, m_fTrailSize(rhs.m_fTrailSize)
	, m_fTrailEmitRate(rhs.m_fTrailEmitRate)
{
	m_pRendererCom->Add_Ref();
	m_pShaderCom->Add_Ref();
	m_pTextureCom->Add_Ref();
	m_pBufferCom->Add_Ref();

	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Render", m_pRendererCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", m_pShaderCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", m_pTextureCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", m_pBufferCom));

	m_pRendererCom->Add_Ref();
	m_pShaderCom->Add_Ref();
	m_pTextureCom->Add_Ref();
	m_pBufferCom->Add_Ref();

	m_pTransformCom = ((Engine::CTransform*)Engine::CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Com_Transform"));
	if (nullptr == m_pTransformCom)
		return;
	m_mapComponent[Engine::CComponent::COM_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", m_pTransformCom));
	m_pTransformCom->Add_Ref();

}

CEffect_Trail::~CEffect_Trail(void)
{

}

CEffect_Trail* CEffect_Trail::Clone(void)
{
	return new CEffect_Trail(*this);
}

HRESULT CEffect_Trail::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	// 현재 지금은 Monk에만 달아만 놓음 
	// -> 세부적으로 클래스를 나누거나, 뭐 정보를 넘겨주거나 Enum문을 할당해주거나
	// 현 Monk의 Melee_Attack1에만 싱크가 그나마 맞을거임
	// -> 아니었ㅇ움

	m_pTransformCom->Set_Information(Engine::CTransform::INFO_POSITION, &_vec3(0.f, 0.f, -4.f));
	m_pTransformCom->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(3.f, 1.f, 1.f));
	m_pTransformCom->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, D3DXToRadian(339.f), 0.f));

	m_iTrailCount = 160;
	m_fTrailEmitRate = 0.0015f;
	m_fTrailSize = -50.f;

	m_vColor = _vec4(0.4f, 0.4f, 1.f, 1.f);

	return NOERROR;

}

int CEffect_Trail::Update_GameObject(const float & fTimeDelta)
{
	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_ALPHA, this);

	Compute_ViewZ(&m_pTransformCom->Get_Info(CTransform::INFO_POSITION));

	Memorize_Point(fTimeDelta);

	return 0;

}

void CEffect_Trail::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == m_pShaderCom || m_Pointlist.size() <= 3)
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	if (FAILED(Set_ConstantTable(pEffect)))
		return;
		
	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0); // Pass1 (검은색 제거)

	D3DXVECTOR3*		pPoint = new D3DXVECTOR3[m_Pointlist.size()];

	POINTLIST::iterator		iter = m_Pointlist.begin();
	POINTLIST::iterator		iter_end = m_Pointlist.end();

	int		iIndex = 0;
	for (; iter != iter_end; ++iter)
	{
		pPoint[iIndex] = (*iter);
		++iIndex;
	}

	m_pBufferCom->SetVertexTrail(pPoint, m_Pointlist.size());
	m_pBufferCom->Render_Buffer();

	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
	Engine::Safe_Delete_Array(pPoint);

}


HRESULT CEffect_Trail::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	// For.Shader
	AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_TrailEffect", Engine::CComponent::COM_STATIC, L"Com_Shader");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STAGE, L"Com_Texture_Trail", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CTrail_Texture*, SCENE_STATIC, L"Com_Buffer_TrailTexture", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	return NOERROR;
}

void CEffect_Trail::Memorize_Point(const float & fTimeDelta)
{
	while (m_Pointlist.size() >= (unsigned int)m_iTrailCount)
	{
		m_Pointlist.pop_front();
	}

	m_fAccTime += fTimeDelta;

	if (m_fTrailEmitRate < m_fAccTime)
	{
		D3DXVECTOR3		vPoint[2];

		float Point0 = m_fTrailSize / 2.f;
		float Pont1 = 0.f - m_fTrailSize / 2.f;

		vPoint[0] = D3DXVECTOR3(0.f, 0.f, Point0);
		vPoint[1] = D3DXVECTOR3(0.f, 0.f, Pont1);

		for (int i = 0; i < 2; ++i)
		{
			D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], m_pmatWeaponRef);
			D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], m_pmatPlayerInfo);

			m_Pointlist.push_back(vPoint[i]);
		}

		m_fAccTime = 0.f;
	}

}

HRESULT CEffect_Trail::Set_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;

	m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld");

	D3DXMATRIX		matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", 0); // Textrue 번호
	pEffect->SetVector("g_vColor", &m_vColor);

	return NOERROR;
}

CEffect_Trail* CEffect_Trail::Create(LPDIRECT3DDEVICE9 pGraphicDev)
{	
	CEffect_Trail* pInstance = new CEffect_Trail(pGraphicDev);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CEffect_Trail Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}
	return pInstance;
}

void CEffect_Trail::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBufferCom);

	cout << "CEffect_Trail 소멸~" << endl;
	Engine::CEffect::Free();
}
