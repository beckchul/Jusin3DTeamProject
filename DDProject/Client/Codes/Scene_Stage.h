#pragma once

#include "Defines.h"
#include "Scene.h"

namespace Engine
{
	class CNavMgr;
	class CEffectMgr;
	class CInfoSubject;
}

class CCamera_Manager;
class CClientNetwork;
class CWaveObserver;
class CScene_Stage : public Engine::CScene
{
public:
	explicit CScene_Stage(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_Stage();
public:
	virtual HRESULT Ready_Scene(void);
	virtual int Update_Scene(const float& fTimeDelta);
	virtual void Render_Scene(void);
	virtual void FirstRender_Finish(void);
private:	
	HRESULT Ready_Effect(void);
	HRESULT Ready_LayerForBackGround(void);
	HRESULT Ready_LayerForGameObject(void);
	HRESULT Ready_LayerForUI(void);

	HRESULT Ready_Light(void);
	HRESULT Ready_NavMesh(void); // NavMesh Load
	HRESULT Ready_MapObject(void); // Object Load

	HRESULT Add_MapObject(Engine::OBJECT& tObject, int& iTemp);

	HRESULT Ready_Wave(void);
private:
	void MonsterLogic(const float& fTimeDelta);
	HRESULT Create_StageMonster(_int iStage);
private:
	CClientNetwork*		m_pClinetNetwork = nullptr;
	CNavMgr*			m_pNavMgr = nullptr;
	CCamera_Manager*	m_pCameraMgr = nullptr;
	CInfoSubject*		m_pInfoSubject = nullptr;
	CWaveObserver*		m_pWaveObserver = nullptr;
	WAVE_INFO			m_tWaveInfo;
	_float				m_fWaveTime = 0.f;
	_float				m_fCreateTime = 0.f;
private:
	SCENEID				m_eSceneId = SCENE_STAGE;
private:
	
	CEffectMgr*			m_pEffectMgr = nullptr;
private:
	bool m_bColorAdd = false;
	_vec4 m_vecFadeColor;
public:
	static CScene_Stage* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

