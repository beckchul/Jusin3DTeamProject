
#ifndef NaviCell_h__
#define NaviCell_h__

#include "Engine_Defines.h"

BEGIN(Engine)

enum POINTID {POINT_A, POINT_B, POINT_C, POINT_END};
enum LINE {LINE_AB, LINE_BC, LINE_CA, LINE_END};
enum NEIGHBORID {NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END};

class CGameObject;
class CLine2D;
class ENGINE_DLL CNaviCell : public CBase
{
private:
	explicit CNaviCell(const _vec3* pPointA, const _vec3* pPointB, const _vec3* pPointC);
	virtual ~CNaviCell(void);
public:
	// Getter
	const _vec3* GetPoint(POINTID ePointID){ return &m_vPoint[ePointID];	}
	CNaviCell* GetNeighbor(NEIGHBORID eNeighborID){ return m_pNeighbor[eNeighborID];	}
	DWORD GetIndex(void) {return m_dwIndex;}
	const D3DXPLANE GetPlane(void) { return m_tPlane; }
	DWORD GetOption(void) { return m_dwOption; }
	CLine2D* GetLine(LINE eLine) { return m_pLine2D[eLine]; }
public:
	// Setter
	void SetNeighbor(NEIGHBORID eNeighborID, CNaviCell* pNeighbor){	m_pNeighbor[eNeighborID] = pNeighbor;}
	void SetPoint(POINTID ePointID, _vec3 vecPos)
	{ 
		m_vPoint[ePointID] = vecPos; 
		//InitCell(m_pGraphicDev, m_dwIndex);
	}
	void SetOption(DWORD dwOption) { m_dwOption = dwOption; }
	void Set_FontNumber(CGameObject* pFontNumber);
public:
	HRESULT InitCell(LPDIRECT3DDEVICE9 pGraphicDev, const DWORD& dwIdx);
	bool ComparePoint(const _vec3* pFristPoint, const _vec3* pSecondPoint, CNaviCell* pNeighbor);

	void Update_NavCell(const float& fTimeDelta);
	void Render_NavCell(LPD3DXLINE pLine); // Cell 그리기
public:
	bool CheckPass(const _vec3* pPos, const _vec3* pDir, NEIGHBORID* pNeighborID, bool& bTwoPass); // 현재 Cell을 벗어나는가?
	bool CheckNavPass(const _vec3* pPos, const _vec3* pDir, NEIGHBORID* pNeighborID, bool& bTwoPass);

	bool CheckIn(const _vec3* pPos); // 현재 Cell 안에 있는가?
	
private:
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	CLine2D*				m_pLine2D[LINE_END];
	CNaviCell*				m_pNeighbor[NEIGHBOR_END];
	CGameObject*			m_pFontNumber = nullptr;
private:
	_vec3					m_vPoint[POINT_END];
	DWORD					m_dwIndex;
	D3DXPLANE				m_tPlane;

	DWORD					m_dwOption; 
public:
	static CNaviCell* Create(LPDIRECT3DDEVICE9 pGraphicDev
		, const _vec3* pPointA
		, const _vec3* pPointB
		, const _vec3* pPointC
		, const DWORD& dwIdx);
private:
	virtual void Free(void);
};

END

#endif // NaviCell_h__