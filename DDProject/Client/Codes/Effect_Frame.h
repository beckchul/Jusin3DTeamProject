
#ifndef Effect_Frame_h__
#define Effect_Frame_h__

#include "Defines.h"
#include "Effect_2D.h"

class CEffect_Frame : public CEffect_2D
{
protected:
	explicit CEffect_Frame(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect_Frame(const CEffect_Frame& rhs);
	virtual ~CEffect_Frame(void);

public:
	virtual CEffect_Frame* Clone(void);

public:		// Setter
	void Set_FrameInfo(FRAME tInfo) { m_tFrame = tInfo; }
	void Set_LoopCount(_int iLimit = 1) { m_bIsLoop = FALSE; m_iLoopLimit = iLimit; }
	void Set_FrameMixOp(_bool bOption) { m_bMixFrame = bOption; }
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
	virtual HRESULT Add_Component(void);

private:
	void Set_TexTag(const _tchar* Tag, SCENEID eID);

private:
	FRAME							m_tFrame;
	const _tchar*					m_pTexTag;
	SCENEID						m_eSceneID;

	_bool								m_bMixFrame = TRUE;	// 기본적으로 다 쓰까놓음
	_bool								m_bIsLoop = TRUE;		// 기본적으로 Effect_Frame은 계속 돌게함.
	_uint								m_iLoopCount = 0;
	_uint								m_iLoopLimit = 1;
private:
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CEffect_Frame* Create(LPDIRECT3DDEVICE9 pGraphicDev,	const _tchar* pTexTag, SCENEID eID);
private:
	virtual void Free(void);
};

#endif // Effect_Frame_h__