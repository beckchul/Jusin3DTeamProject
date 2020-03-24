#pragma once

#include "Defines.h"
#include "Scene.h"
namespace Engine
{
	class CNavMgr;
	class CEffectMgr;
}
class CCamera_Manager;
class CClientNetwork;

class CTestScene_BackChul : public Engine::CScene
{
public:
	explicit CTestScene_BackChul(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTestScene_BackChul();
public:
	virtual HRESULT Ready_Scene(void);
	virtual int Update_Scene(const float& fTimeDelta);
	virtual void Render_Scene(void);
private:
	HRESULT Ready_LayerForBackGround(void);
	HRESULT Ready_LayerForGameObject(void);
	HRESULT Ready_LayerForUI(void);
	HRESULT Ready_Light(void);
	HRESULT Ready_NavMesh(void); // NavMesh Load

	HRESULT Ready_MapObject(void); // Object Load
	HRESULT Add_MapObject(Engine::OBJECT& tObject);
private:
	CNavMgr*				m_pNavMgr = nullptr;
	CCamera_Manager*	m_pCameraMgr = nullptr;
	CEffectMgr*			m_pEffect_Manager = nullptr;
	CClientNetwork*		m_pClinetNetwork = nullptr;
private:
	bool						m_bShowNavMesh = false;
public:
	static CTestScene_BackChul* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

