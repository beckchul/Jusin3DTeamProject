
#ifndef RecordCamera_h__
#define RecordCamera_h__

#include "Camera.h"
#include "Defines.h"

namespace Engine
{
	class CInputDev;
}

class CRecordCamera: public Engine::CCamera
{
public:
	explicit CRecordCamera(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CRecordCamera(void);
public:
	virtual HRESULT Ready_GameObject();
	virtual _int Update_GameObject(const float & fTimeDelta) override;
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
public:
	virtual void Change_Camera(_uint uNum = 0, float fAttachTime = 0.f);
private:
	float				m_fCamSpeed;
	bool				m_bRecordCamera;

	// 자연스런 카메라
	float				m_fAngle[Engine::ANGLE_END];
	D3DXVECTOR3			m_vDir;

	// 녹화 관련
	ID3DXFont*			m_pD3DXFont;
	RECT				m_FontRect;
	RECT				m_TimeRect;

	bool				m_bRecording;
	float				m_fRecordTime;

	TCHAR				m_szRecordState[128];
	TCHAR				m_szRecordTime[16];

	HANDLE				m_hFile;

	Engine::CRenderer*	m_pRendererCom = nullptr;
	CInputDev*			m_pInputDev = nullptr;
private:
	virtual HRESULT Add_Component(void) final;

	void KeyCheck(const float & fTimeDelta);
	void MouseMove(const float & fTimeDelta);

	// 녹화
	void RecordKey(void);
	void Record(const float & fTimeDelta);

public:
	static CRecordCamera* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void)final;
};

#endif // RecordCamera_h__