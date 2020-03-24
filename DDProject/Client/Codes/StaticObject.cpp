#include "stdafx.h"
#include "StaticObject.h"

CStaticObject::CStaticObject(LPDIRECT3DDEVICE9 pGraphicDev)
: Engine::CGameObject(pGraphicDev)
, m_pRendererCom(nullptr)
, m_pShaderCom(nullptr)
, m_pStaticMeshCom(nullptr)
{
}

CStaticObject::~CStaticObject()
{
}

void CStaticObject::Free(void)
{
	Engine::Safe_Release(m_pRendererCom);
	Engine::Safe_Release(m_pShaderCom);
	Engine::Safe_Release(m_pStaticMeshCom);
	Engine::CGameObject::Free();
}
