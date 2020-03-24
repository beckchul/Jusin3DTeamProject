#include "stdafx.h"
#include "EffectAdd_Manager.h"


// ÀÌÆåÆ®µé..
#include "Effect.h"
#include "EffectMgr.h"
#include "Effect_Frame.h"
#include "Effect_Single.h"
#include "Effect_Mesh_Shield.h"
#include "Effect_Mesh_Bolt.h"
#include "Effect_Mesh_ElectricGroup1.h"
#include "Effect_Mesh_ElectricGroup2.h"
#include "Effect_Mesh_LightningTree.h"
#include "Effect_Mesh_Wave.h"
#include "Effect_Mesh_AuraSphere.h"
#include "Effect_Mesh_Spiral.h"
#include "Effect_Particle.h"

IMPLEMENT_SINGLETON(CEffectAdd_Manager)

CEffectAdd_Manager::CEffectAdd_Manager()
	:m_pEffectMgr(Engine::CEffectMgr::GetInstance())
	, m_eEffectID(EFFECT_END)
{
	m_pEffectMgr->Add_Ref();
}

CEffectAdd_Manager::~CEffectAdd_Manager()
{

}

HRESULT CEffectAdd_Manager::Add_Effect(LPDIRECT3DDEVICE9 pGraphicDev, EFFECT_ID eID, _vec3 vCreatePos)
{

	Engine::CEffect* pEffect = nullptr;

	switch (eID)		//¤Ð¤Ð
	{
	case EFFECT_AURA:
	{
		pEffect = CEffect_Mesh_AuraSphere::Create(pGraphicDev);
		if (nullptr == pEffect)
			return E_FAIL;
		pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.14f, 0.14f, 0.14f));
		pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_POSITION, &_vec3(vCreatePos.x, vCreatePos.y + 10.f, vCreatePos.z));
		m_pEffectMgr->Add_Effect(pEffect);

		//Particle
		pEffect = CEffect_Particle::Create(pGraphicDev
			, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Shield")));
		dynamic_cast<CEffect_Particle*>(pEffect)->Set_OriginPos(_vec3(vCreatePos.x, vCreatePos.y + 17.f, vCreatePos.z));
		if (nullptr == pEffect)
			return E_FAIL;
		m_pEffectMgr->Add_Effect(pEffect);
		break;
	}
	case EFFECT_TORCH:
	{	
		break;
	}
	default:
		return E_FAIL;
	}

	//if (pEffect != nullptr)
	//	m_pEffectMgr->Add_Effect(pEffect);

	return S_OK;
}

void CEffectAdd_Manager::Free(void)
{
	Engine::Safe_Release(m_pEffectMgr);
}
