#ifndef AnimationCtrl_h__
#define AnimationCtrl_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CAnimationCtrl : public CBase
{
public:
	enum PLAY_TYPE { PLAY, PLAY_BACK};
private:
	explicit CAnimationCtrl(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CAnimationCtrl(const CAnimationCtrl& rhs);
	virtual ~CAnimationCtrl(void);
public:
	LPD3DXANIMATIONCONTROLLER* Get_AnimationCtrl(void) {
		return &m_pAnimationCtrl;}
	DOUBLE GetPeriod(void) {
		return m_dPeriod;
	}
	DOUBLE GetTrackPos(void);
public:
	void Set_AnimationSet(ANI_INFO tAniInfo);
	void Set_TrackPosition(const _int& iIndex, const _float& fTimeDelta);
public:
	HRESULT Ready_AnimationCtrl(void);
	void Play_AnimationSet(const _float& fTimeDelta);
private:
	LPDIRECT3DDEVICE9				m_pGraphicDev;
	LPD3DXANIMATIONCONTROLLER		m_pAnimationCtrl;
	_uint							m_iMaxNumAniSet;
	_uint							m_iOldIndex;
	_ulong							m_dwCurrentTrack;
	_ulong							m_dwNewTrack;
	_double							m_dTimeAcc;
	_double							m_dPeriod;
public:
	static CAnimationCtrl* Create(LPDIRECT3DDEVICE9 pGraphicDev);
	static CAnimationCtrl* Create(const CAnimationCtrl& rhs);
private:
	virtual void Free(void);

};

END

#endif // AnimationCtrl_h__
