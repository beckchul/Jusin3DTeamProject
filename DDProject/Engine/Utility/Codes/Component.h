#ifndef Component_h__
#define Component_h__

#include "Engine_Defines.h"
BEGIN(Engine)
class ENGINE_DLL CComponent : public CBase
{
public: // COM_STATIC ������Ʈ�Ǥ� �ʿ䰡 ���� ������Ʈ
	// COM_DYNAMIC ������Ʈ�Ǥ� �ʿ䰡 �ִ� ������Ʈ
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