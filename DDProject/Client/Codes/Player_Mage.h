#pragma once

#include "Player.h"

namespace Engine {	// Minkyeong
	class CEffect;
}

class CPlayer_Mage : public CPlayer
{
public:
	explicit CPlayer_Mage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer_Mage();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int FirstUpdate_GameObject(const float& fTimeDelta);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void FirstRender_GameObject(_bool bTrue);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	virtual void Change_State(void); 
	virtual void KeyCheck(const float& fTimeDelta);
	void KeyCheck_Death(void); // 죽은 상태에서 KeyCheck
	virtual void Set_Collision(void);
private:
	_float				m_fGageCount = 0.f;
	bool				m_bBackColorAdd = false;
private:	// Minkyeong
	_bool							m_bIsCharging = FALSE;
	list<Engine::CEffect*>	m_Templist;
public:
	static CPlayer_Mage* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

