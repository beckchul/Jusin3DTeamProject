#ifndef Camera_Manager_h__
#define Camera_Manager_h__

#include "Defines.h"

namespace Engine
{
	class CCamera;
	class CGameObject;
	class CInputDev;
}

class CFrustumManager;
class CCamera_Manager : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)
private:
	explicit CCamera_Manager();
	virtual ~CCamera_Manager();
public:
	// Getter
	CAMERA_TYPE		Get_CamType(void) { return m_eCamType; }
	bool Get_IsCamAttach(void);
public:
	int Update_Camara(const float& fTimeDelta);
	HRESULT Add_Camera(Engine::CGameObject* pGameObject, CAMERA_TYPE eCamType);
	void Change_Camera(CAMERA_TYPE eCamType, _uint uCutSceneNum = 0, _float fAttachTime = 0.f, bool bForceChange = false);
	void Fix_MouseCursor(bool b);
	void Shake_Camera(const float& fShakePower, const float& fShakeTime, const _int iShakeCount);
	void Reset_Camera(void);
private:
	Engine::CInputDev*		m_pInputDev = nullptr;
	CFrustumManager*		m_pFrustumMgr = nullptr;
private:
	Engine::CCamera*		m_ArrCamera[CAMERA_END];
	CAMERA_TYPE				m_eCamType = CAMERA_END;
private:
	void KeyCheck(void);
	void Update_Frustum();
private:
	virtual void Free(void);
};

#endif