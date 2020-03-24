#ifndef StaticCamera_h__
#define StaticCamera_h__

#include "Defines.h"
#include "Camera.h"

namespace Engine
{
	class CInputDev;
}

class CPlayerObserver;
class CStaticCamera : public CCamera
{
private:
	explicit CStaticCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStaticCamera(void);
public:
	void Set_CameraShake(const float& fShakePower, const float& fShakeTime, const _int iShakeCount);
public:
	virtual HRESULT Add_Component(void) final { return NOERROR; }
	virtual HRESULT Ready_GameObject();
public:
	virtual void Change_Camera(_uint uNum = 0, float fAttachTime = 0.f);
public:
	virtual _int Update_GameObject(const float& fTimeDelta) override;
private:
	CPlayerObserver*	m_pPlayerObserver = nullptr;
	CInputDev*			m_pInputDev = nullptr;
	_matrix*			m_pPlayerMatrix = nullptr;
private:
	_bool				m_bFixMouse = true;
	_float				m_fCameraAngle = 0.f;
	_float				m_fPlayerRatio[3];

	// Shake
	_bool				m_bShakeCam = false;
	_float				m_fShakeTime = 0.f;
	_float				m_fShakePower = 0.f;
	_int				m_iShakeCount = 0;

	_float				m_fTime = 0.f;
private:
	void KeyCheck(const float& fTimeDelta);
	void MouseFix(void);
	void TargetRenewal(const float& fTimeDelta);
	void ShakeCamera(const float& fTimeDelta);
public:
	static CStaticCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void) final;
};

#endif//StaticCamera_h__
