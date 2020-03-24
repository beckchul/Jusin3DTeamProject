#ifndef Scene_h__
#define Scene_h__

#include "Engine_Defines.h"
#include "Layer.h"
#include "Object_Manager.h"

BEGIN(Engine)
class CComponent;
class ENGINE_DLL CScene : public CBase
{
protected:
	explicit CScene(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CScene(void);
public:
	virtual HRESULT Ready_Scene(void) PURE;
	virtual int Update_Scene(const float& fTimeDelta);
	virtual void Render_Scene(void);
	virtual void FirstRender_Finish(void) {} // 첫 번째 렌더가 끝난 이후

protected:
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;
protected:
	Engine::CComponent_Manager*	m_pComponentMgr = nullptr;
	Engine::CObject_Manager*	m_pObjectMgr = nullptr;
protected:	
	virtual void Free(void) override;
};
END
#endif // Scene_h__


