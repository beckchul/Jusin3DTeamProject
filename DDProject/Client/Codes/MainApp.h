#pragma once
#ifndef MainApp_h__
#define MainApp_h__

#include "Defines.h"
#include "Engine_Defines.h"
#include "GraphicDev.h"

namespace Engine
{
	class CInputDev;
	class CComponent_Manager;
	class CManagement;
	class CSoundMgr;
}


class CLoading;
class CMainApp : public CBase
{
private:
	explicit CMainApp(void);
	virtual ~CMainApp(void);

public:
	HRESULT Ready_MainApp(void);
	int Update_MainApp(const float& fTimeDelta);
	void Render_MainApp(void);
private:
	HRESULT Ready_DefaultSetting(Engine::CGraphicDev::WINMODE eMode, const WORD& wSizeX, const WORD& wSizeY);
	HRESULT Ready_StartScene(void);
private:
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;
	LPDIRECTINPUT8				m_pInpuDev = nullptr;
private:
	Engine::CComponent_Manager*	m_pComponentMgr = nullptr;
	Engine::CManagement*		m_pManagementMgr = nullptr;
	Engine::CSoundMgr*			m_pSoundMgr = nullptr;
public:
	static CMainApp* Create(void);

private:
	virtual void Free(void) override;
};

#endif