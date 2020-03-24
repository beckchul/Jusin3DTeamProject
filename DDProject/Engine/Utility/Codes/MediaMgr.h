#ifndef MediaMgr_h__
#define MediaMgr_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class CMediaObj;
class CRect_Texture;

class ENGINE_DLL CMediaMgr : public CBase
{
	DECLARE_SINGLETON(CMediaMgr)
private:
	explicit CMediaMgr(void);
	virtual ~CMediaMgr(void);

public:
	void Ready_MediaMgr(LPDIRECT3DDEVICE9 pGraphicDev, float fWidth, float fHeight);
	void Progress(void);
	void Render(void);
	
public:
	HRESULT VideoPlay(WCHAR* wFileName);
	HRESULT StopPlay(void);

private:
	void InitMoviePos();
	void ProjVector(_vec3& Vector, const _vec3& VenPoint);
	void ConvertVector(_vec3& Vector, int iWidth, int iHeight);
	void VertexRenewal(void);

private:
	CMediaObj*				m_pMediaObj;
	LPDIRECT3DDEVICE9		m_pGraphicDev;

private:
	Engine::VTXTEX*			m_pVtxPos;
	Engine::VTXTEX*			m_pVtxConPos;
	_vec3				m_vVanPoint;
	_vec3				m_vPos;
	float					m_fHalfWidth;
	float					m_fHalfHeight;
	CRect_Texture*			m_pRcTexBuffer;
	float					m_fWidth;
	float					m_fHeight;

private:
	virtual void Free(void);
};

END


#endif // !MediaMgr_h__
