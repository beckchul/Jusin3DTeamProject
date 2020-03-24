#ifndef Component_Manager_h__
#define Component_Manager_h__

#include "Engine_Defines.h"
#include "Rect_Texture.h"
#include "Terrain_Texture.h"
#include "Cube_Texture.h"
#include "View_Texture.h"
#include "Trail_Texture.h"
#include "DynamicMesh.h"
#include "Static_Mesh.h"
#include "Texture.h"
#include "Renderer.h"
#include "Transform.h"
#include "Shader.h"
#include "CollisionSphere.h"
#include "CubeColor.h"

BEGIN(Engine)

class CComponent;
class ENGINE_DLL CComponent_Manager : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)	
private:
	explicit CComponent_Manager(void);
	virtual ~CComponent_Manager(void);
public:
	HRESULT Reserve_ComponentMgr(const _uint& iSize);
	HRESULT Add_Component(const _uint& uIndex, const _tchar* pComponentTag, CComponent* pComponent);
	CComponent* Clone_Component(const _uint& uIndex, const _tchar* pComponentTag);
	void Release_Component(const _uint& uIndex);
private:
	unordered_map<const _tchar*, CComponent*>*		m_pMapComponent; // 동적할당할려고.
	typedef unordered_map<const _tchar*, CComponent*>	MAPCOMPONENT;
private:
	_uint									m_iContainerSize;
private:
	CComponent* Find_Component(const _uint& uIndex, const _tchar* pComponentTag);

private:
	virtual void Free(void);

};

END

#endif // Component_Manager_h__
