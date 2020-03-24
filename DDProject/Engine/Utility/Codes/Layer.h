#ifndef Layer_h__
#define Layer_h__

#include "Engine_Defines.h"
#include "GameObject.h"
#include "Component.h"
#include "GraphicDev.h"

BEGIN(Engine)
class ENGINE_DLL CLayer : public CBase
{
private:
	explicit CLayer(void);
	virtual ~CLayer(void);	
public:
	const CComponent* Get_Component(CComponent::COMTYPE eComType, const TCHAR* pComponentKey, const _uint& uIndex = 0);
	const list<CGameObject*> Get_GameObjectlist(void) {
		return m_ObjectList; }
public:
	HRESULT Add_GameObject(CGameObject* pGameObject);
public:
	HRESULT Ready_Layer(void);
	int Update_Layer(const float& fTimeDelta);
private:
	bool							m_bFirstUpdate = FALSE;
private:
	list<CGameObject*>				m_ObjectList;
	typedef list<CGameObject*>		OBJECTLIST;
public:
	static CLayer* Create(void);
private:
	virtual void Free(void) final;
};
END
#endif
