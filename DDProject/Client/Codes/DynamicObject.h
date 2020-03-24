#pragma once

#include "Engine_Defines.h"
#include "Defines.h"
#include "GameObject.h"

class CDynamicObject : public Engine::CGameObject
{
protected:
	explicit CDynamicObject(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CDynamicObject(void);
public:
	bool Get_Dead(void) { return m_tInfo.bDead; }
public: // Setter
	void Set_Dead(void) { m_tInfo.bDead = true; }
protected:
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CShader*			m_pShaderCom = nullptr;
	Engine::CDynamicMesh*		m_pDynamicMeshCom = nullptr;
protected:
	_float						m_fTimeDelta = 0.f;	// UpdateTime ����
	INFO						m_tInfo;	// �⺻ ���� ���� ����ü(�÷��̾� ����)
	vector<ANI_INFO>			m_vecAnimationInfo;	// �ִϸ��̼� ����ü vector

protected:
	HRESULT Load_AnimationInfo(char* szFilePath);
protected:
	virtual HRESULT Add_Component(void)PURE;
protected:
	virtual void Free(void);
};

