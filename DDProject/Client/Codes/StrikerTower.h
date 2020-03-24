#ifndef StrikerTower_h__
#define StrikerTower_h__

#include "Tower.h"

namespace Engine {	
	class CEffect;
}

class CStrikerTower : public CTower
{
public:
	explicit CStrikerTower(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStrikerTower();
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
private:
	_bool		m_bIsCharging = FALSE;
	list<Engine::CEffect*> m_listChargingEffect;
public:
	static CStrikerTower* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

#endif//StrikerTower_h__