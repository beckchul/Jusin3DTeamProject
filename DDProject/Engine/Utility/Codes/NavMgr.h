
#ifndef NavMgr_h__
#define NavMgr_h__

#include "Engine_Defines.h"
#include "NaviCell.h"

BEGIN(Engine)

class CTransform;
class CGameObject;
class CNaviCell;

class ENGINE_DLL CNavMgr : public CBase
{
	enum eShowNavMesh { SHOW_NOTHING, SHOW_NAVMESH, SHOW_NAVMESH_FONT };

	DECLARE_SINGLETON(CNavMgr)
private:
	explicit CNavMgr(void);
	virtual ~CNavMgr(void);
public: // Getter
	vector<CNaviCell*> GetVecNavMesh(void) { return m_vecNaviMesh; }
public:
	void ReserveCellSize(LPDIRECT3DDEVICE9 pGraphicDev, const DWORD& dwSize);
	HRESULT AddCell(NAVMESH* pNavMesh, CGameObject* pFontNumber);
	void LinkCell(void);
	void Reset_Cell(void); // 스테이지 전환 시 기존 내비메쉬 리셋

	void Update_NavMesh(const float& fTimeDelta);
	void Render_NavMesh(void);

	DWORD MoveOnNavMesh(_vec3* pPos, const _vec3* pDir, const DWORD& dwCurrentIdx, bool bDropAble = true); // 메쉬 위에서 이동
	bool FallOnNavMesh(CTransform* ppTransform, const DWORD& dwCurrentIdx, bool& bDead); // NavMesh와 충돌

	DWORD FindNavMesh(_vec3& vPos); // 낙하용
	DWORD FindSecondFloor(_vec3& vPos, const DWORD dwCurrentIdx); // 2층 찾기용
	bool IsAbleToUpStairs(const DWORD dwCurrentIdx);
private:
	// 아래 2개는 안쓰는중
	POINTID Get_NearestPoint(CNaviCell* pNavCell, _vec3 vPoint);
	CNaviCell* Get_NearNeighbor(CNaviCell* pNavCell, POINTID ePoint);

	// 옵션에 따른 행동
	void Slide_Vector(_vec3* pPos, const _vec3* pDir, DWORD& dwIndex, NEIGHBORID eNeighborID);
	void Drop(_vec3* pPos, const _vec3* pDir, DWORD& dwIndex);
private:
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;
	DWORD					m_dwReserveSize;
	DWORD					m_dwIdxCnt;
	eShowNavMesh			m_eShowNavMesh = SHOW_NOTHING;
private:
	LPD3DXLINE				m_pLine = nullptr;
private:
	typedef vector<CNaviCell*>		VECCELL;
	VECCELL							m_vecNaviMesh; // 전체 내비메쉬
	VECCELL							m_vecNaviMeshSecondFloor; // 2층 내비메쉬
	
private:
	virtual void Free(void);
};

END

#endif // NavMgr_h__
