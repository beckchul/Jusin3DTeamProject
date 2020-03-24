#ifndef ActionCamera_h__
#define ActionCamera_h__

#include "Defines.h"
#include "Camera.h"

class CActionCamera : public CCamera
{
private:
	explicit CActionCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CActionCamera(void);
public:
	virtual HRESULT Add_Component(void) final { return S_OK; }
	virtual HRESULT Ready_GameObject();
public:
	virtual void Change_Camera(_uint uNum = 0, float fAttachTime = 0.f);
public:
	virtual _int Update_GameObject(const float & fTimeDelta) override;

	void Set_CutScene(void);
	void Show_CutScene(void);

public:
	static CActionCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	HANDLE		m_hFile;
	bool		m_bShowCutScene = false;
	CAMERA_TYPE	m_eNextCamType = CAMERA_END;
	CUTSCENE_TYPE m_eCutSceneType = CUTSCENE_END;
	SCENEID		m_eSceneId = SCENE_END;
private:
	virtual void Free(void)final;
};

#endif