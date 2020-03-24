#ifndef GameObject_h__
#define GameObject_h__

#include "Engine_Defines.h"
#include "Component_Manager.h"
#include "GraphicDev.h"
#include "Component.h"
#include "Shader.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject : public CBase
{
protected:
	explicit CGameObject(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject(void);
	
public: // Setter
	void Set_Pos(const _vec3* pPos);
	void Set_Dir(const _vec3* pDir);
	void Set_Dead(void) { m_bDead = true; }

public: // Getter
	_vec3	Get_Pos(void);
	CComponent* Get_Component(CComponent::COMTYPE eComType, const TCHAR* pComponentKey);
	const float& Get_ViewZ(void) const { 
		return m_fViewZ; }
public:	
	virtual HRESULT Ready_GameObject(void);
	virtual int FirstUpdate_GameObject(const float & fTimeDelta) { return 0; }
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void FirstRender_GameObject(_bool bTrue = false) {}
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0) {}
protected:
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;
	CTransform*				m_pTransformCom = nullptr; // Transform
protected: // For.Manager
	CComponent_Manager*		m_pComponentMgr = nullptr;
protected:
	map<const TCHAR*, CComponent*>			m_mapComponent[CComponent::COM_END];
	typedef map<const TCHAR*, CComponent*>	MAPCOMPONENT;		
protected:
	float				m_fViewZ; // 카메라로부터의 거리
	bool				m_bDead = false; // 죽음..
protected:
	void Compute_ViewZ(const D3DXVECTOR3* pPos);
	virtual HRESULT Add_Component(void)PURE;
protected:
	virtual void Free(void) override;
};


END
#endif