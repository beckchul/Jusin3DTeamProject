#ifndef InfoSubject_h__
#define InfoSubject_h__

#include "Subject.h"

BEGIN(Engine)

class ENGINE_DLL CInfoSubject : public CSubject
{
	DECLARE_SINGLETON(CInfoSubject)
public:
	explicit CInfoSubject(void);
	virtual ~CInfoSubject(void);

public:
	list<void*>* GetDatalist(int message);

public:
	void Add_Data(int message, void* pData);
	void Add_DataFront(int message, void* pData);
	void Remove_Data(int message, void* pData);

private:
	map<int, list<void*>>		    m_mapDatalist;

	typedef list<void*>				DATALIST;
	typedef map<int, DATALIST>		MAPDATALIST;
private:
	virtual void Free(void);
};

END

#endif // InfoSubject_h__