#pragma once

#include "Engine_Defines.h"

#include "Component.h"

// Layer들을 모아서 보관한다.
BEGIN(Engine)

class CLayer;
class CGameObject;
class ENGINE_DLL CObject_Manager : public CBase
{
	DECLARE_SINGLETON(CObject_Manager)
public:
	explicit CObject_Manager();
	virtual ~CObject_Manager();
public:
	const CComponent* Get_Component(const _uint& iSceneID, const _tchar* pLayerTag, CComponent::COMTYPE eComType, const TCHAR* pComponentKey, _uint uIndex);
public: 
	HRESULT Reserve_ObjectMgr(const int& iContainerSize);
	// 객체를 특정 레이어안에 추가한다.
	HRESULT Add_GameObject(int eID, const TCHAR* pLayerTag
		, CGameObject* pGameObject);
	int Update_ObjMgr(const float& fTimeDelta);
	void Release_GameObject(const _uint& iSceneIdx);
private:
	unordered_map<const TCHAR*, CLayer*>*			m_pMapLayer = nullptr;
	typedef unordered_map<const TCHAR*, CLayer*>	MAPLAYER;
private:
	_uint				m_iMaxContainerSize = 0;
private:
	virtual void Free(void);
};

END

