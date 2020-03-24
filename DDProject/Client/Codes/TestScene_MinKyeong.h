#pragma once

#include "Defines.h"
#include "Scene.h"

namespace Engine
{
	class CEffectMgr;
	class CGameObject;
	class CEffect;
}

class CCamera_Manager;
class CClientNetwork;

class CTestScene_MinKyeong : public Engine::CScene
{
public:
	explicit CTestScene_MinKyeong(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTestScene_MinKyeong();
public:
	virtual HRESULT Ready_Scene(void);
	virtual int Update_Scene(const float& fTimeDelta);
	virtual void Render_Scene(void);
private:
	HRESULT Ready_EffectList(void);
	HRESULT Ready_LayerForBackGround(void);
	HRESULT Ready_LayerForGameObject(void);
	HRESULT Ready_NavMesh(void);
	HRESULT Ready_Light(void);
	HRESULT Ready_LayerForUI(void);

	HRESULT Ready_MapObject(void); // Object Load
	HRESULT Add_MapObject(Engine::OBJECT& tObject);
private:
	CClientNetwork*		m_pClinetNetwork = nullptr;
	CEffectMgr*				m_pEffect_Manager = nullptr;
	CCamera_Manager*	m_pCameraMgr = nullptr;
	Engine::CGameObject*	m_pGameObejct = nullptr;
	Engine::CEffect*			m_pEffect = nullptr;
	_vec4							m_vColor = _vec4(1.f, 1.f, 0.f, 1.f);
	_vec3							m_vPos = _vec3(0.f, 10.f, 0.f);
	_vec3							m_vRotation = _vec3(0.f, 0.f, 0.f);

public:
	static CTestScene_MinKyeong* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

