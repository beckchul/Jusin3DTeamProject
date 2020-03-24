#pragma once

#include "Defines.h"
#include "Scene.h"
#include "Loading.h"

namespace Engine
{
	class CMediaMgr;
}

class CLoading;
class CScene_Transition : public Engine::CScene
{
public:
	explicit CScene_Transition(LPDIRECT3DDEVICE9 pGraphicDev, CLoading::LOADINGTYPE eType);
	virtual ~CScene_Transition();
public:
	virtual HRESULT Ready_Scene(void);
	virtual int Update_Scene(const float& fTimeDelta);
	virtual void Render_Scene(void);
private:
	CLoading::LOADINGTYPE			m_eLoadingTpye; // ¿Ãµø«“ Scene
private:
	CLoading*				m_pLoading = nullptr;
	Engine::CMediaMgr*		m_pMediaMgr = nullptr;
private:
	_bool					m_bShowVideo = true;
	
private:
	HRESULT Ready_LayerForBackGround(void);
	HRESULT Ready_Loading(void);
public:
	static CScene_Transition* Create(LPDIRECT3DDEVICE9 pGraphicDev, CLoading::LOADINGTYPE eType);
private:
	virtual void Free(void);
};

