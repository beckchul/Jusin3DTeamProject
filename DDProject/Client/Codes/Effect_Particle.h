#ifndef Effect_Particle_h__
#define Effect_Particle_h__

#include "Effect.h"
#include "Defines.h"

DWORD FtoDW(float f);

class CEffect_Particle : public Engine::CEffect
{
protected:
	explicit CEffect_Particle(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CEffect_Particle(const CEffect_Particle& rhs);
	virtual ~CEffect_Particle(void);

public:
	virtual CEffect_Particle* Clone(void);

public:		// Setter
	virtual void Set_OriginPos(_vec3 OriginPos);
	HRESULT Set_DestPos(_vec3 DestPos);
	void Set_Dir(_vec3 vDir);		// Blood_Particle시 사용 . 안쓸꺼임
	void Set_Speed(_float fSpeed);
	void Set_FireWork_YPositive(void) { m_bFireWorkYPositive = TRUE; }
	void Set_DestBlendInv(void) { m_bDestBlendInv = TRUE; }
	void Set_SpreadExceptY(void) { m_ParticleInfo.bSpreadExceptY = TRUE; }
	void Set_LoopFalse(void) { m_ParticleInfo.bLoop = FALSE; }
	void Set_FollowTrackingPath(void) { m_bFollowTrackingPath = TRUE; }
	void Set_FadeInfo(_vec2 vFadeInfo) { m_vFadeInfo = vFadeInfo; }
	void Set_FadeColor(_vec3 vColor) { m_bUseFadeColor = TRUE; m_vFadeColor = vColor; }

public:
	virtual HRESULT Ready_GameObject(void);
	virtual int Update_GameObject(const float & fTimeDelta);
	virtual void Render_GameObject(LPD3DXEFFECT pArgEffect = nullptr, _uint uPassIdx = 0);
	virtual HRESULT Add_Component(void);
	virtual void Reset_Effect(void);

private:
	HRESULT Initialize(Engine::PARTICLE_INFO ParticleInfo);
	void Add_Particle(void);
	void Pre_Render(void);
	void Post_Render(void);

private:	//Calc
	float Get_RandomFloat(float LowBound, float HighBound);
	void Get_RandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max);

private:
	bool			Is_Empty(void);
	HRESULT	Clean_Up(void);		// 버텍스와 이미지를 비운다.
	void Set_ID(SCENEID eID);

private:
	Engine::CRenderer*			m_pRendererCom = nullptr;
	Engine::CTexture*				m_pTextureCom = nullptr;

private:
	IDirect3DVertexBuffer9*				m_pVb;					// 버텍스 버퍼
	list<Engine::PARTICLE_ATT*>		m_vecParticle;			// 파티클 속성 리스트
	Engine::PARTICLE_INFO				m_ParticleInfo;			// 파티클 정보
	_float										m_fDelayCount;		// 생성 카운트
	_int										m_iCount;				// 루프 옵션을 위한 변수
	_float										m_fGravityStartTime;
	SCENEID								m_eSceneID =  SCENE_END;
	_vec2										m_vFadeInfo;			// Fade 조절하장.
	_vec3										m_vFadeColor;
	
private:
	DWORD								m_dwVbSize;	// 버텍스 버퍼가 보관할수있는 파티클 수
	DWORD								m_dwVbOffset;
	DWORD								m_dwVbBatchSize;
	_bool									m_bFireWorkYPositive = FALSE;
	_bool									m_bDestBlendInv = FALSE;
	_bool									m_bFollowTrackingPath = FALSE;
	_bool									m_bUseFadeColor = FALSE;
public:
	static CEffect_Particle* Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::PARTICLE_INFO ParticleInfo, SCENEID eID);

private:
	virtual void Free(void);

};

#endif // Effect_Particle_h__