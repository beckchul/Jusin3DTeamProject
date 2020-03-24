#pragma once

#include "Defines.h"
#include "Scene.h"

class CTestScene_SeoChan : public Engine::CScene
{
public:
	explicit CTestScene_SeoChan(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CTestScene_SeoChan();
public:
	virtual HRESULT Ready_Scene(void);
	virtual int Update_Scene(const float& fTimeDelta);
	virtual void Render_Scene(void);
private:
	HRESULT Ready_LayerForBackGround(void);
	HRESULT Ready_LayerForGameObject(void);
	HRESULT Ready_Light(void);

	HRESULT Ready_LayerForUI(void);
	
	HRESULT Ready_MapObject(void); // Object Load
	HRESULT Add_MapObject(Engine::OBJECT& tObject);

private:
	vector<_vec3>& m_LightPos;

public:
	static CTestScene_SeoChan* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

