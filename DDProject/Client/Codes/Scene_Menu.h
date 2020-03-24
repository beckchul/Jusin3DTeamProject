#pragma once

#include "Defines.h"
#include "Scene.h"


namespace Engine
{
	class CEffectMgr;
}

class CSceneObserver;
class CCamera_Manager;
class CScene_Menu : public Engine::CScene
{
public:
	explicit CScene_Menu(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_Menu();
public:
	virtual void FirstRender_Finish();

	virtual HRESULT Ready_Scene(void);
	virtual int Update_Scene(const float& fTimeDelta);
	virtual void Render_Scene(void);
private:
	HRESULT Ready_Effect(void);
	HRESULT Ready_Light(void);
	HRESULT Ready_LayerForBackGround(void);
	HRESULT Ready_LayerForGameObject(void);
	HRESULT Ready_LayerForUI(void);
private:
	CSceneObserver*		m_pSceneObserver = nullptr;
	CCamera_Manager*	m_pCameraMgr = nullptr; 
private:
	CEffectMgr*				m_pEffectMgr = nullptr;
	SCENEID					m_eSceneId = SCENE_MENU;
public:
	static CScene_Menu* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

