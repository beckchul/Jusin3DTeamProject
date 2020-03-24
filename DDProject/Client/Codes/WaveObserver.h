
#ifndef WaveObserver_h__
#define WaveObserver_h__

#include "Observer.h"
#include "Defines.h"

class CWaveObserver : public Engine::CObserver
{
private:
	explicit CWaveObserver(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CWaveObserver(void);

public:
	// Getter
	const WAVE_INFO& Get_WaveInfo(void) { return m_tWaveInfo; }
public:
	virtual void Update_Observer(int message);
	virtual HRESULT Ready_Observer(void);

	void Text_Phase(PHASE_TYPE eType);
private:
	LPDIRECT3DDEVICE9	m_pGraphicDev = nullptr;
	WAVE_INFO			m_tWaveInfo;

public:
	static CWaveObserver* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	virtual void Free(void);
};

#endif // WaveObserver_h__