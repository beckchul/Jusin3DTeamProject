#ifndef StandCamera_h__
#define StandCamera_h__

#include "Defines.h"
#include "Camera.h"

namespace Engine
{
	class CInputDev;
}

class CStandCamera : public CCamera
{
private:
	explicit CStandCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStandCamera(void);
public:
	virtual HRESULT Add_Component(void) final { return S_OK; }
	virtual HRESULT Ready_GameObject();
public:
	virtual void Change_Camera(_uint uNum = 0, float fAttachTime = 0.f);
public:
	virtual _int Update_GameObject(const float & fTimeDelta) override;
private:
	CInputDev*	m_pInputDev = nullptr;

private:
	void KeyCheck(const float & fTimeDelta);

public:
	static CStandCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void)final;
};

#endif