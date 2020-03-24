#ifndef CollisionManager_h__
#define CollisionManager_h__

#include "Defines.h"
#include "Engine_Defines.h"

namespace Engine
{
	class CDataManager;
	class CMouse_Manager;
}

class CMouseCol;
class CCollisionManager : public CBase
{
	DECLARE_SINGLETON(CCollisionManager)
private:
	explicit CCollisionManager();
	virtual ~CCollisionManager();
public:
	void Initialize_CollisionMgr(void);

	// Setter
	void Set_Render(_bool bRender) { 
		m_bRender = bRender; }
	// Getter
	_bool Get_Render(void) {
		return m_bRender; }
public:
	HRESULT Add_Component(const _tchar* pComponentTag, CComponent* pComponent);
	HRESULT Delete_Component(const _tchar* pComponentTag, CComponent* pComponent);
	void Render_CollisionSphere(void);
public:
	list<CComponent*> Find_Componentlist(const _tchar* pComponentTag);
private:
	_bool					m_bRender = false;
private:
	unordered_map<const _tchar*, list<CComponent*>>	m_MapCollisionComponent;

	// Picking
public:
	void Add_MouseCol(LPDIRECT3DDEVICE9 pGraphicDev, LPD3DXMESH pMesh, _matrix& matWorld); // 메쉬 정보를 MouseCol에 저장하고 m_listMouseCol에 MouseCol을 저장
	
	void Translation_ViewSpace(void);
	void Translation_Local(const _matrix* pmatWorld);
	
	bool Pick_MouseCol(_vec3& vecPick); // 마우스 위치를 로컬 좌표계로 변환시켜 피킹한다
	bool Pick_Terrain(const _vec3* vecPos, const _vec3* vecDir, _vec3& vecPick); // Input : 위치 & 방향 , Output : 피킹된 위치
	DWORD Pick_NavMesh(_vec3& vecPick);
	
	bool Picking(_vec3& vecPick); // MouseCol list를 순회하며 Mesh의 Picking된 지점을 찾는다
private:
	Engine::CDataManager*		m_pDataMgr = nullptr;
	Engine::CMouse_Manager*		m_pMouseMgr = nullptr;

	list<CMouseCol*>		m_listMousecol;
	_vec3					m_vPivotPos;
	_vec3					m_vRayDir;

	vector<Engine::NAVMESH> m_vecNavMesh; // 전체 내비메쉬
	vector<Engine::NAVMESH> m_vecTowerNavMesh; // 타워 내비메쉬
private:
	virtual void Free(void);
};

#endif