#ifndef Loading_h__
#define Loading_h__

#include <winbase.h>
#include "Engine_Defines.h"
#include "Defines.h"
#include "GraphicDev.h"

namespace Engine
{
	class CComponent_Manager;
	class CEffectMgr;
}

class CLoading : public CBase
{
public:
	enum LOADINGTYPE { LOADING_LOGO, LOADING_MENU, LOADING_LOBBY, LOADING_STAGE1, LOADING_STAGE2, 
		LOADING_TESTSCENE_SEUNGHO, LOADING_TESTSCENE_SEOCHAN, LOADING_TESTSCENE_BACKCHUL, LOADING_TESTSCENE_MINKYEONG,
		LOADING_TESTSCENE_SEUNGHO2, LOADING_TESTSCENE_SEOCHAN2, LOADING_TESTSCENE_BACKCHUL2, LOADING_TESTSCENE_MINKYEONG2,
		LOADING_END
	};
private:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGTYPE eLoadID);
	virtual ~CLoading(void);
public: // Getter
	 bool Get_Complete(void) const { return m_bComplete; }
	
private:
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;
	LOADINGTYPE				m_eLoadType;
private:
	Engine::CComponent_Manager*		m_pComponentMgr = nullptr;
	Engine::CEffectMgr*						m_pEffectMgr = nullptr;
private:
	bool					m_bComplete = false;
private:
	HRESULT Loading_Function();
private:
	HRESULT Ready_Loading(void);
	HRESULT LogoLoading(void);
	HRESULT StaticLoading(void);
	HRESULT MenuLoading(void);
	HRESULT Stage1Loading(void);
	HRESULT Stage2Loading(void);
	HRESULT LobbyLoading(void);

	HRESULT SeungHoLoading(void);
	HRESULT SeoChanLoading(void);
	HRESULT BackChulLoading(void);
	HRESULT MinKyeongLoading(void);

	HRESULT SeungHoLoading2(void);
	HRESULT SeoChanLoading2(void);
	HRESULT BackChulLoading2(void);
	HRESULT MinKyeongLoading2(void);

	HRESULT Load_DynamicMesh(void);
	HRESULT Load_StaticMesh(void);
	HRESULT Load_Effect(void);
	HRESULT Load_EffectList(void);

	HRESULT Load_UI();

public:
	static CLoading* Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGTYPE eLoadType);
private:
	virtual void Free(void) override;
};

#endif