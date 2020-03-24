#ifndef Victory_Camera_h__
#define Victory_Camera_h__

#include "Defines.h"
#include "Camera.h"

namespace Engine
{
	class CInputDev;
}

class CLastManObserver;
class CVictory_Camera : public CCamera
{
private:
	explicit CVictory_Camera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CVictory_Camera(void);
public:
	virtual HRESULT Add_Component(void) final { return NOERROR; }
	virtual HRESULT Ready_GameObject();
public:
	virtual void Change_Camera(_uint uNum = 0, float fAttachTime = 0.f);
public:
	virtual _int Update_GameObject(const float& fTimeDelta) override;
private:
	CLastManObserver*	m_pPlayerObserver = nullptr;
	CInputDev*			m_pInputDev = nullptr;
	_matrix*			m_pPlayerMatrix = nullptr;
private:
	_bool				m_bFixMouse = true;
	_float				m_fCameraAngle = 0.f;
	_float				m_fPlayerRatio[3];

	_float				m_fCamAngle[Engine::ANGLE_END];

	_float				m_fTime = 0.f;
	_uint				m_uVectoryPlayer = 0;
private:
	void KeyCheck(const float& fTimeDelta);
	void Mouse_Move(void);
	void MouseFix(void);
	void TargetRenewal(const float& fTimeDelta);
public:
	static CVictory_Camera* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void) final;
};

#endif//Victory_Camera_h__
