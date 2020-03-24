#ifndef CubeColor_h__
#define CubeColor_h__

#include "VIBuffer.h"
BEGIN(Engine)

class CMesh;
class ENGINE_DLL CCubeColor : public CVIBuffer
{
private:
	explicit CCubeColor(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCubeColor(const CCubeColor& rhs);
	virtual ~CCubeColor(void);

public:
	_vec3 Get_MinPoint(void){
		return m_vMin; }
	_vec3 Get_MaxPoint(void) {
		return m_vMax; }
public:
	void Set_ColInfo(CMesh* pMesh);
	void Set_ColInfo(const _matrix* pWorld, const _vec3* pMin, const _vec3* pMax) {
		m_pmatWorld = pWorld; m_vMin = *pMin; m_vMax = *pMax;	}

	void SetBoundingBox(const _vec3* pMin
		, const _vec3* pMax
		, const DWORD& dwColor);
public:
	HRESULT Ready_Buffer(const _vec3* pMin, const _vec3* pMax, const DWORD& dwColor);
	virtual void Render_Buffer(const _matrix* pmatWorld, const DWORD& dwColor);

private:
	const _matrix*			m_pmatWorld = nullptr;
	_vec3					m_vMin;
	_vec3					m_vMax;

public:
	virtual CComponent* Clone(void);
	static CCubeColor* Create(LPDIRECT3DDEVICE9 pGraphicDev, const _vec3* pMin, const _vec3* pMax
		, const DWORD& dwColor);
	virtual void Free(void);
};

END
#endif

