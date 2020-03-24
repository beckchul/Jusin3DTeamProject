#pragma once

#include "Defines.h"
#include "DynamicObject.h"

class CWaveObserver;
class CRightDoor : public CDynamicObject
{
protected:
	explicit CRightDoor(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRightDoor(void);
public:
	virtual HRESULT Ready_GameObject();
	virtual int FirstUpdate_GameObject(const float & fTimeDelta);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);

private:
	eDoor_State			m_eState = DOOR_IDLE;
	eDoor_State			m_eMainState = DOOR_IDLE;
	CWaveObserver*		m_pWaveObserver = nullptr;
	PHASE_TYPE			m_eCurPhaseType = PHASE_END;
private:
	void Play_Animation(const float& fTimeDelta);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);

public:
	static CRightDoor* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual void Free(void);
};

