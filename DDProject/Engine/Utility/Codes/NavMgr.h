
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
	void Reset_Cell(void); // �������� ��ȯ �� ���� ����޽� ����

	void Update_NavMesh(const float& fTimeDelta);
	void Render_NavMesh(void);

	DWORD MoveOnNavMesh(_vec3* pPos, const _vec3* pDir, const DWORD& dwCurrentIdx, bool bDropAble = true); // �޽� ������ �̵�
	bool FallOnNavMesh(CTransform* ppTransform, const DWORD& dwCurrentIdx, bool& bDead); // NavMesh�� �浹

	DWORD FindNavMesh(_vec3& vPos); // ���Ͽ�
	DWORD FindSecondFloor(_vec3& vPos, const DWORD dwCurrentIdx); // 2�� ã���
	bool IsAbleToUpStairs(const DWORD dwCurrentIdx);
private:
	// �Ʒ� 2���� �Ⱦ�����
	POINTID Get_NearestPoint(CNaviCell* pNavCell, _vec3 vPoint);
	CNaviCell* Get_NearNeighbor(CNaviCell* pNavCell, POINTID ePoint);

	// �ɼǿ� ���� �ൿ
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
	VECCELL							m_vecNaviMesh; // ��ü ����޽�
	VECCELL							m_vecNaviMeshSecondFloor; // 2�� ����޽�
	
private:
	virtual void Free(void);
};

END

#endif // NavMgr_h__
