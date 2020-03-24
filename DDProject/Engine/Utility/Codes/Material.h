#ifndef Material_h__
#define Material_h__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CMaterial : public CComponent
{
public:
	enum MATERIAL {MTRL_DIFFUSE, MTRL_AMBIENT, MTRL_SPECULAR, MTRL_EMISSIVE, MTRL_POWER};
private:
	explicit CMaterial(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CMaterial(const CMaterial& rhs);
	virtual ~CMaterial(void);
public:
	void Get_Material(D3DMATERIAL9* pMtrlInfo) {
		*pMtrlInfo = m_MtrlInfo;}
public:
	void Change_Material(MATERIAL eType, void* pValue);
public:
	HRESULT Ready_Component(D3DXCOLOR Diffuse, D3DXCOLOR Ambient, D3DXCOLOR Emissive, D3DXCOLOR Specular, const _float& fPower);
	void SetUp_OnGraphicDev(void) const {
		m_pGraphicDev->SetMaterial(&m_MtrlInfo);}
private:
	LPDIRECT3DDEVICE9		m_pGraphicDev;
	D3DMATERIAL9			m_MtrlInfo;
public:
	virtual CComponent* Clone(void);
	static CMaterial* Create(LPDIRECT3DDEVICE9 pGraphicDev, D3DXCOLOR Diffuse, D3DXCOLOR Ambient, D3DXCOLOR Emissive, D3DXCOLOR Specular, const _float& fPower);
private:
	virtual void Free(void);
};


END

#endif // Material_h__
