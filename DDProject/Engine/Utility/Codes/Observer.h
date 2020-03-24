#ifndef Observer_h__
#define Observer_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CObserver : public CBase
{
protected:
	explicit CObserver(void);
	virtual ~CObserver(void);

protected:
	virtual HRESULT Ready_Observer(void) PURE;
public:
	virtual void Update_Observer(int message) PURE;

protected:
	virtual void Free(void) PURE;
};

END

#endif // Observer_h__