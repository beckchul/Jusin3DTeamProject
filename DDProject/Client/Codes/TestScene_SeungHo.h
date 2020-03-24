#pragma once

#include "Defines.h"
#include "Scene.h"

class CCamera_Manager;
class CTestScene_SeungHo : public Engine::CScene
{
public:
	explicit CTestScene_SeungHo(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTestScene_SeungHo();
public:
	virtual HRESULT Ready_Scene(void);
	virtual int Update_Scene(const float& fTimeDelta);
	virtual void Render_Scene(void); 
	virtual void FirstRender_Finish(void);
private:
	HRESULT Ready_LayerForBackGround(void);
	HRESULT Ready_LayerForGameObject(void);
	HRESULT Ready_LayerForUI(void);
	HRESULT Ready_Light(void);
private:
	CCamera_Manager*	m_pCameraMgr = nullptr;
public:
	static CTestScene_SeungHo* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

