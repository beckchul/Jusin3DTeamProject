#pragma once

#include "Defines.h"
#include "Scene.h"

namespace Engine
{
	class CNavMgr;
	class CEffectMgr;
	class CInfoSubject;
}
class CEffect_Magnetic_Sphere;		// Minkyeong
class CCamera_Manager;
class CClientNetwork;
class CScene_Stage2 : public Engine::CScene
{
public:
	explicit CScene_Stage2(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_Stage2();
public:
	virtual HRESULT Ready_Scene(void);
	virtual int Update_Scene(const float& fTimeDelta);
	virtual void Render_Scene(void);
	virtual void FirstRender_Finish(void);
private:
	HRESULT Ready_Effect(void);
	HRESULT Ready_Light(void);
	HRESULT Ready_NavMesh(void); // NavMesh Load
	HRESULT Ready_MapObject(void); // Object Load
	HRESULT Ready_RandomData(void); // Data Load
	HRESULT Add_MapObject(Engine::OBJECT& tObject);

	HRESULT Ready_LayerForBackGround(void);
	HRESULT Ready_LayerForGameObject(void);
	HRESULT Ready_LayerForUI(void);
private:
	void Lastman_Progress(void);
	void Lastman_Timer(const float& fTimeDelta);
private:
	CClientNetwork*				m_pClinetNetwork = nullptr;
	CNavMgr*					m_pNavMgr = nullptr;
	CCamera_Manager*			m_pCameraMgr = nullptr;
	CInfoSubject*				m_pInfoSubject = nullptr;
	_float						m_fRoundTime = 0.f;
private:
	SCENEID						m_eSceneId = SCENE_STAGE2;
	LASTMAN_PROGRESS			m_eSCeneState = LASTMAN_WAIT;
	_int						m_iTrack = 0;
private:
	CEffectMgr*									m_pEffectMgr = nullptr;
	CEffect_Magnetic_Sphere*					m_pMagneticSphere = nullptr;
private:
	bool m_bColorAdd = false;
	_vec4 m_vecFadeColor;
public:
	static CScene_Stage2* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

