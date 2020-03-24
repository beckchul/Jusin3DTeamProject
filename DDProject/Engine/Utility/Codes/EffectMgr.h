#ifndef EffectMgr_h__
#define EffectMgr_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class CEffect;
class ENGINE_DLL CEffectMgr : public CBase
{
	DECLARE_SINGLETON(CEffectMgr)
private:
	explicit CEffectMgr(void);
	virtual ~CEffectMgr(void);
	
public:		// Getter
	PARTICLE_INFO* Find_ParticleInfo(const _tchar* pParticleTag);

public:	// 인자로 이펙트리스트 넘겨주면 죽엽러ㅣㅁ
	void Kill_TheseEffectlist(list<CEffect*> pEffectlist);

public:
	HRESULT Init_EffectData(void);
	HRESULT Add_EffectList(const _tchar* pEffectTag, list<CEffect*> pEffectList);		// Effect의 종류를 Tchar나, Enum문으로 잡아놓고..
	void Update_Effect(const float& fTimeDelta);
	void Render_Effect(void); // 사실상 안쓰일 가능성이 높다

	void Release_Effect(void);	// Scene이 끝나고, 실제 돌아가고있는 이펙트 리스트들을 비워준다.
										// = ObjectMgr - Release_GameObject
	void Reset_Effect(void); // jsh

public:		// 왜 다 주소값을 반환하게 했어 -> 내가 원하는 타이밍에 IsDead시켜주기 위함 .
	CEffect*			Add_Effect(CEffect* pEffect);	// 단순히 Create한 객체 넣어줄때
	list<CEffect*> Add_Effect(const _tchar* pEffectTag, _vec3* vOriginPos);		// TagName으로 미리 만들어놓은 리스트 생성시
	list<CEffect*> Add_Effect(const _tchar* pEffectTag, _vec3* vOriginPos, _vec3 FinalPos, _float fSpeed);
	list<CEffect*> Add_Effect(const _tchar* pEffectTag, _vec3* vOriginPos, _vec3 vDir, _float fSpeed, _bool bIsDir);
	list<CEffect*> Add_Effect(const _tchar* pEffectTag, _matrix* pmatInfo);	// 그냥 matrix만 받아옴. 
	list<CEffect*> Add_Effect(const _tchar* pEffectTag, _matrix* pmatInfo, _vec3 vTargetPos);	// matrix + vTargetPos ( 회전 위함 )
	list<CEffect*> Add_Effect(const _tchar* pEffectTag, _matrix* pmatInfo, _matrix* pmatParent);	// TagName + Matrix +parent <- 딱 붙여야되는 이펙트시 사용할거임


private:		// 실제 이펙트 
	list<CEffect*>				m_listEffect;

private:
	unordered_map<const _tchar* , list<CEffect*>>	m_mapEffectKind;

private:		// 이펙트의 데이터
	unordered_map<const _tchar*, PARTICLE_INFO*>					m_mapParticleInfo;
	typedef unordered_map<const _tchar*, PARTICLE_INFO*>		MAP_PARTICLEINFO;

private:
	virtual void Free(void);
};

END

#endif // EffectMgr_h__