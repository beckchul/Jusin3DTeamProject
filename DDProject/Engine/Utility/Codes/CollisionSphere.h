#ifndef CollisionSphere_h__
#define CollisionSphere_h__

#include "Collision.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CCollisionSphere : public CCollision
{
private:
	explicit CCollisionSphere(LPDIRECT3DDEVICE9 pGraphicDev);
	explicit CCollisionSphere(const CCollisionSphere& rhs);
	virtual ~CCollisionSphere(void);
public:
	void Set_CheckCollsion(const _bool& bCheck){
		m_bCheckCollision = bCheck; }
	void Set_MatrixWorldCollision(const _matrix* pMatrix) {
		m_matWorldCollision = (*pMatrix); }
	void Set_Damage(const _int& iDamage) {
		m_iDamage = iDamage; }
	void Set_HitCheck(const _bool& bCheck) {
		m_bHitCheck = bCheck; }
	void SetRender(const _bool& bRender) {
		m_bRender = bRender; }
	void Set_Life(const _float& fLife) {
		m_fLife = fLife; }
	void Set_Scale(const _float& fScale) {
		m_fScale = fScale; }
public:
	_bool Get_CheckCollision(void) {
		return m_bCheckCollision; }
	_float Get_Radius(void) {
		return m_fRadius; }
	_float Get_Scale(void) {
		return m_fScale; }
	_float Get_Life(void) {
		return m_fLife; }
	_matrix Get_WorldMatrix(void) { 
		return m_matWorldCollision; }
	_int Get_Damage(void) { 
		return m_iDamage; }
	_bool Get_HitCheck(void) {
		return m_bHitCheck; }
	_vec3 Get_Direction(void){
		return m_vDir; }
	_bool Get_Render(void) {
		return m_bRender; }
	
	
public:
	void Create_Sphere(_float fRadius, const _matrix* pBoneMatrix, _float fScale);
public:
	HRESULT Ready_CollisionSphere(void) { return NOERROR; }
	virtual int Update_CollisionSphere(const _matrix* pWorldMatrix);
	void Render_CollisionSphere(void);
public:
	_bool	Check_Collision(CCollisionSphere* pTarget);
	_bool	Check_AuraCollision(CCollisionSphere* pTarget);
	_bool	Push_Collision(CCollisionSphere* pTarget);
public:
	virtual CComponent* Clone(void);
	static CCollisionSphere* Create(LPDIRECT3DDEVICE9 pGraphicDev);
private:
	ID3DXMesh*					m_pCollisionSphere		= nullptr;	// �浹 ��ü
	const _matrix*				m_pCollisionBoneMatrix	= nullptr;	// �浹 �� matrix
	_matrix						m_matWorldCollision;				// �浹 ��ü�� world matrix
private:
	_float						m_fRadius			= 20.f;			// ��ü�� ������(�浹 ����)
	_float						m_fScale			= 1.f;			// ũ��
	_int						m_iDamage			= 0;			// �浹 ��������
	_bool						m_bCheckCollision	= true;			// �浹 ����
	_bool						m_bHitCheck			= false;		// �¾Ҵ��� ����
	_float						m_fLife = 0.f;
private:
	_vec3						m_vDir;
	_bool						m_bRender			= true;
private:
	virtual void Free(void);
};

END

#endif//CollisionSphere_h__