#ifndef VIBuffer_h__
#define VIBuffer_h__

#include "Resources.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer : public CResources
{	
protected:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer(void);
public: // Getter
	_ulong Get_TriCnt() const {
		return m_dwTriCnt;
	}
	void GetVtxInfo(void* pVtxInfo);

public:	// Setter
	void SetVtxInfo(void* pVtxInfo);
	void SetIdxInfo(void* pIdxInfo, const DWORD* pTriCnt);
public:
	virtual HRESULT Ready_Buffer(void);
	virtual void Render_Buffer(void) {}
protected:
	LPDIRECT3DVERTEXBUFFER9				m_pVB;
	_ulong								m_dwVtxCnt;
	_ulong								m_dwVtxFVF;
	_ulong								m_dwVtxSize;
	
protected:
	LPDIRECT3DINDEXBUFFER9				m_pIB;
	_ulong								m_dwIdxSize;
	_ulong								m_dwTriCnt;
	D3DFORMAT							m_IndexFmt;

public:
	virtual CComponent* Clone(void)PURE;
public:
	virtual void Free(void);
};

END

#endif // VIBuffer_h__