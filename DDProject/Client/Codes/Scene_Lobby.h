#pragma once

#include "Defines.h"
#include "Scene.h"


namespace Engine
{
	class CEffectMgr;
}

class CClientNetwork;
class CCamera_Manager;
class CScene_Lobby : public Engine::CScene
{
public:
	explicit CScene_Lobby(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_Lobby();
public:
	virtual void FirstRender_Finish();
public:
	virtual HRESULT Ready_Scene(void);
	virtual int Update_Scene(const float& fTimeDelta);
	virtual void Render_Scene(void);
private:
	HRESULT Ready_Effect(void);
	HRESULT Ready_Light(void);
	HRESULT Ready_NavMesh(void); // NavMesh Load
	HRESULT Ready_MapObject(void); // Object Load
	HRESULT Add_MapObject(Engine::OBJECT& tObject);

	HRESULT Ready_LayerForBackGround(void);
	HRESULT Ready_LayerForGameObject(void);
	HRESULT Ready_LayerForUI(void);
private:
	CCamera_Manager*	m_pCameraMgr = nullptr;
private:
	CEffectMgr*				m_pEffectMgr = nullptr;
	CClientNetwork*			m_pClinetNetwork = nullptr;
	SCENEID					m_eSceneId = SCENE_LOBBY;

private:
	bool m_bColorAdd = false;
	_vec4 m_vecFadeColor;

public:
	static CScene_Lobby* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

