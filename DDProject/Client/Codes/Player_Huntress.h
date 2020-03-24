#ifndef Player_Huntress_h__
#define Player_Huntress_h__

#include "Player.h"

class CPlayer_Huntress : public CPlayer
{
public:
	explicit CPlayer_Huntress(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer_Huntress();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int FirstUpdate_GameObject(const float& fTimeDelta);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	virtual void Change_State(void);
	virtual void KeyCheck(const float& fTimeDelta);
	virtual void Set_Collision(void);
public:
	static CPlayer_Huntress* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

#endif//#Player_Huntress_h__