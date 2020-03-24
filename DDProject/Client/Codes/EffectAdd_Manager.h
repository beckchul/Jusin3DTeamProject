#ifndef EffectAdd_Manager_h__
#define EffectAdd_Manager_h__

#include "Defines.h"

namespace Engine
{
	class CEffectMgr;
}

class CEffectAdd_Manager : public CBase
{
	DECLARE_SINGLETON(CEffectAdd_Manager)
public:
	explicit CEffectAdd_Manager();
	virtual ~CEffectAdd_Manager();
	
public:
	HRESULT Add_Effect(LPDIRECT3DDEVICE9 pGraphicDev, EFFECT_ID eID, _vec3 vCreatePos);

private:		// ぬぬ?ばば..
	Engine::CEffectMgr*		m_pEffectMgr = nullptr;
	EFFECT_ID					m_eEffectID = EFFECT_END;

private:
	virtual void Free(void);
};

#endif //EffectAdd_Manager_h__