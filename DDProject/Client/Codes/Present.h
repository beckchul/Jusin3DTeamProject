#pragma once

#include "Defines.h"
#include "StaticObject.h"

namespace Engine
{
	class CNavMgr;
	class CEffect;
}

class CPlayerObserver;
class CPresent : public CStaticObject
{
private:
	explicit CPresent(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPresent(void);
	// Setter
public:
	void Set_Dir(_vec3& vecDir) { m_vDir = vecDir; }
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	void Present_Fall(const float& fTimeDelta);
	void Present_Move(const float & fTimeDelta);
	void Follow_Player(const float & fTimeDelta);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	DWORD			m_dwIndex = 0;
	_vec3			m_vecSpeed;
	_vec3			m_vDir;
	list<Engine::CEffect*> m_listAttachedEffect;
private:
	bool			m_bOnGround = false;
	pair<int, int>  m_PairPlayerMp;
private:
	CPlayerObserver*	m_pPlayerObserver = nullptr;
	CNavMgr*			m_pNavMgr = nullptr;
public:
	static CPresent* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual void Free(void);
};

