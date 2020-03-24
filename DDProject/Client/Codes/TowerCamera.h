#ifndef TowerCamera_h__
#define TowerCamera_h__

#include "Defines.h"
#include "Camera.h"

class CPlayerObserver;
class CTowerCamera : public CCamera
{
private:
	explicit CTowerCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTowerCamera(void);
public:
	void Set_FixCursor(bool b) { m_bFixCursor = b; }
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
	static CTowerCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	CPlayerObserver*	m_pPlayerObserver = nullptr;

	float				m_fRadius = 0.f;
	bool				m_bFixCursor = true;
private:
	virtual void Free(void)final;
};

#endif