#ifndef Blockade_h__
#define Blockade_h__

#include "Tower.h"

class CBlockade : public CTower
{
public:
	explicit CBlockade(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CBlockade();
public:
	virtual HRESULT Ready_GameObject();
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	virtual void Change_State(void) {}
	virtual void Create_Bullet(void) {}
public:
	static CBlockade* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

#endif//Blockade_h__