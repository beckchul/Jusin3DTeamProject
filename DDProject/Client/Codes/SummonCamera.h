#ifndef SummonCamera_h__
#define SummonCamera_h__

#include "Defines.h"
#include "Camera.h"

class CPlayerObserver;
class CSummonCamera : public CCamera
{
private:
	explicit CSummonCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CSummonCamera(void);
public:
	virtual HRESULT Add_Component(void) final { return S_OK; }
	virtual HRESULT Ready_GameObject();
public:
	virtual void Change_Camera(_uint uNum = 0, float fAttachTime = 0.f);
public:
	void Restrict_MouseCursor(void);
public:
	virtual _int Update_GameObject(const float & fTimeDelta) override;
public:
	static CSummonCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	CPlayerObserver*	m_pPlayerObserver = nullptr;

	float				m_fRadius = 0.f;
private:
	virtual void Free(void)final;
};

#endif