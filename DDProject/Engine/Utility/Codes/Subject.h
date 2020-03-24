#ifndef Subject_h__
#define Subject_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class CObserver;
class ENGINE_DLL CSubject abstract : public CBase
{
protected:
	explicit CSubject(void);
	virtual ~CSubject(void);

public:
	virtual void Subscribe_Observer(CObserver* pObserver);
	virtual void UnSubscribe_Observer(CObserver* pObserver);
	virtual void Notify_Message(int message);

protected:
	typedef list<CObserver*>		OBSERVERLIST;
	OBSERVERLIST		m_Observerlist;
protected:
	virtual void Free(void);
};

END

#endif // Subject_h__