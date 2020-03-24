#ifndef LightningTower_h__
#define LightningTower_h__

#include "Tower.h"

class CLightningTower : public CTower
{
public:
	explicit CLightningTower(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLightningTower();
public:
	virtual HRESULT Ready_GameObject();
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void FirstRender_GameObject(_bool bTrue);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
private:
	void Change_State(void);
	virtual void Create_Bullet(void);
public:
	static CLightningTower* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

#endif//LightningTower_h__