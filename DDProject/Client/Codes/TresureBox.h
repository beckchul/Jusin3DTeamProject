#pragma once

#include "Defines.h"
#include "DynamicObject.h"

class CPlayerObserver;
class CWaveObserver;
class CTresureBox : public CDynamicObject
{
protected:
	explicit CTresureBox(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTresureBox(void);
public:
	virtual HRESULT Ready_GameObject();
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	_bool						m_bOpen = false;
	PHASE_TYPE					m_ePhaseType = PHASE_END;
	float						m_fPresentTime = 0.f;
private:
	CPlayerObserver*			m_pPlayerObserver = nullptr;
	CWaveObserver*				m_pWaveObserver = nullptr;
private:
	void Play_Animation(const float& fTimeDelta);
	void Give_Present(const float & fTimeDelta);
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CTresureBox* Create(LPDIRECT3DDEVICE9 pGraphicDev);
protected:
	virtual void Free(void);
};

