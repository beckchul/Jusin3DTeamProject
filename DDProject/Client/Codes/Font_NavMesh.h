#pragma once

#include "Font_Perspective.h"

namespace Engine
{
	class CDataManager;
}
class CFont_NavMesh : public CFont_Perspective
{
public:
	explicit CFont_NavMesh(LPDIRECT3DDEVICE9 pGraphicDev, _uint uIndex);
	virtual ~CFont_NavMesh();
public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
private:
	_uint		m_uIndex = 0;
	CDataManager*		m_pDataMgr = nullptr;
private:
	virtual HRESULT Add_Component(void);
	HRESULT Set_ConstantTable(LPD3DXEFFECT pEffect);
public:
	static CFont_NavMesh* Create(LPDIRECT3DDEVICE9 pGraphicDev, _uint uIndex);
private:
	virtual void Free(void);
};

