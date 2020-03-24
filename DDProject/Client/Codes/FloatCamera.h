#ifndef FloatCamera_h__
#define FloatCamera_h__

#include "Defines.h"
#include "Camera.h"

class CFloatCamera : public CCamera
{
private:
	explicit CFloatCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CFloatCamera(void);
public:
	virtual HRESULT Add_Component(void) final { return S_OK; }
	virtual HRESULT Ready_GameObject();
public:
	virtual void Change_Camera(_uint uNum = 0, float fAttachTime = 0.f);
public:
	virtual _int Update_GameObject(const float & fTimeDelta) override;
public:
	static CFloatCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void)final;
};

#endif