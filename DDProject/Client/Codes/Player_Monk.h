#ifndef Player_Monk_h__
#define Player_Monk_h__

#include "Player.h"

class CPlayer_Monk : public CPlayer
{
public:
	explicit CPlayer_Monk(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CPlayer_Monk();
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
	bool				m_bBackColorAdd = false;
public:
	static CPlayer_Monk* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

#endif//Player_Monk_h__