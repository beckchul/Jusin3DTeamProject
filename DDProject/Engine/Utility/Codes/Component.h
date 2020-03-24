#ifndef Component_h__
#define Component_h__

#include "Engine_Defines.h"
BEGIN(Engine)
class ENGINE_DLL CComponent : public CBase
{
public: // COM_STATIC 업데이트의ㅡ 필요가 없는 컴포넌트
	// COM_DYNAMIC 업데이트의ㅡ 필요가 있는 컴포넌트
	enum COMTYPE {COM_STATIC, COM_DYNAMIC, COM_END};
protected:
	explicit CComponent(void);
	virtual ~CComponent(void);

public:
	virtual CComponent* Clone(void)PURE;

public:
	virtual int Update_Component(void) { return 0; }
	
protected:
	virtual void Free(void) PURE;
};
END
#endif