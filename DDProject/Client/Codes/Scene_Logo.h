#pragma once

#include "Defines.h"
#include "Scene.h"

class CSceneObserver;
class CScene_Logo : public Engine::CScene
{
public:
	explicit CScene_Logo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene_Logo();
public:
	virtual HRESULT Ready_Scene(void);
	virtual int Update_Scene(const float& fTimeDelta);
	virtual void Render_Scene(void);
private:
	CSceneObserver*		m_pSceneObserver = nullptr;
	SCENEID					m_eSceneId = SCENE_LOGO;
private:
	HRESULT Ready_LayerForBackGround(void);
	HRESULT Ready_LayerForGameObject(void);
	HRESULT Ready_LayerForUI(void);
private:
	void Goto_TestScene(void);
public:
	static CScene_Logo* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

