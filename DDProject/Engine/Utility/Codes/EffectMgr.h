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

public:	// ���ڷ� ����Ʈ����Ʈ �Ѱ��ָ� �׿����Ӥ�
	void Kill_TheseEffectlist(list<CEffect*> pEffectlist);

public:
	HRESULT Init_EffectData(void);
	HRESULT Add_EffectList(const _tchar* pEffectTag, list<CEffect*> pEffectList);		// Effect�� ������ Tchar��, Enum������ ��Ƴ���..
	void Update_Effect(const float& fTimeDelta);
	void Render_Effect(void); // ��ǻ� �Ⱦ��� ���ɼ��� ����

	void Release_Effect(void);	// Scene�� ������, ���� ���ư����ִ� ����Ʈ ����Ʈ���� ����ش�.
										// = ObjectMgr - Release_GameObject
	void Reset_Effect(void); // jsh

public:		// �� �� �ּҰ��� ��ȯ�ϰ� �߾� -> ���� ���ϴ� Ÿ�ֿ̹� IsDead�����ֱ� ���� .
	CEffect*			Add_Effect(CEffect* pEffect);	// �ܼ��� Create�� ��ü �־��ٶ�
	list<CEffect*> Add_Effect(const _tchar* pEffectTag, _vec3* vOriginPos);		// TagName���� �̸� �������� ����Ʈ ������
	list<CEffect*> Add_Effect(const _tchar* pEffectTag, _vec3* vOriginPos, _vec3 FinalPos, _float fSpeed);
	list<CEffect*> Add_Effect(const _tchar* pEffectTag, _vec3* vOriginPos, _vec3 vDir, _float fSpeed, _bool bIsDir);
	list<CEffect*> Add_Effect(const _tchar* pEffectTag, _matrix* pmatInfo);	// �׳� matrix�� �޾ƿ�. 
	list<CEffect*> Add_Effect(const _tchar* pEffectTag, _matrix* pmatInfo, _vec3 vTargetPos);	// matrix + vTargetPos ( ȸ�� ���� )
	list<CEffect*> Add_Effect(const _tchar* pEffectTag, _matrix* pmatInfo, _matrix* pmatParent);	// TagName + Matrix +parent <- �� �ٿ��ߵǴ� ����Ʈ�� ����Ұ���


private:		// ���� ����Ʈ 
	list<CEffect*>				m_listEffect;

private:
	unordered_map<const _tchar* , list<CEffect*>>	m_mapEffectKind;

private:		// ����Ʈ�� ������
	unordered_map<const _tchar*, PARTICLE_INFO*>					m_mapParticleInfo;
	typedef unordered_map<const _tchar*, PARTICLE_INFO*>		MAP_PARTICLEINFO;

private:
	virtual void Free(void);
};

END

#endif // EffectMgr_h__