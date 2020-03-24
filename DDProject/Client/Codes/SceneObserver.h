
#ifndef SceneObserver_h__
#define SceneObserver_h__

#include "Observer.h"
#include "Defines.h"

class CSceneObserver: public Engine::CObserver
{
private:
	explicit CSceneObserver(void);
	virtual ~CSceneObserver(void);

public:
	// Getter
	SCENEID Get_SceneChange(void) { return m_eCurrentScene; }

public:
	virtual void Update_Observer(int message);
	virtual HRESULT Ready_Observer(void);

private:
	SCENEID			m_eCurrentScene;

public:
	static CSceneObserver* Create(void);
private:
	virtual void Free(void);
};

#endif // SceneObserver_h__