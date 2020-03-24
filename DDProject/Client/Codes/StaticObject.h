#ifndef StaticObject_h__
#define StaticObject_h__

#include "Engine_Defines.h"
#include "GameObject.h"

class CStaticObject : public Engine::CGameObject
{
protected:
	explicit CStaticObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CStaticObject(void);
protected:
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	Engine::CStaticMesh*		m_pStaticMeshCom = nullptr;
protected:
	virtual void Free(void);

};

#endif // !StaticObject_h__