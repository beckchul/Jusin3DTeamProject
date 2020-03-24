#ifndef DynamicCamera_h__
#define DynamicCamera_h__

#include "Defines.h"
#include "Camera.h"

namespace Engine
{
	class CInputDev;
}

class CDynamicCamera : public CCamera
{
private:
	explicit CDynamicCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDynamicCamera(void);
public:
	virtual HRESULT Add_Component(void) final { return S_OK; }
	virtual HRESULT Ready_GameObject();
public:
	virtual void Change_Camera(_uint uNum = 0, float fAttachTime = 0.f);
public:
	virtual _int Update_GameObject(const float & fTimeDelta) override;
private:
	float		m_fCamSpeed = 0.f;
	bool		m_bFixMouse = true;

	CInputDev*	m_pInputDev = nullptr;

private:
	void KeyCheck(const float & fTimeDelta);
	void MouseMove(void);
	void MouseFix(void);	
	
public:
	static CDynamicCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void)final;
};

#endif