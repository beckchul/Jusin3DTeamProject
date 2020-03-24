#include "GameObject.h"
#include "Shader.h"
#include "Transform.h"

USING(Engine)

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphicDev)
: m_pGraphicDev(pGraphicDev)
, m_pTransformCom(nullptr)
, m_bDead(FALSE)
{
	m_pGraphicDev->AddRef();	
}

CGameObject::CGameObject(const CGameObject& rhs)
: m_pGraphicDev(rhs.m_pGraphicDev)
, m_pTransformCom(nullptr)
{
	// TransformCom은 할당 X <- 따로 생성 
	m_pGraphicDev->AddRef();
}

CGameObject::~CGameObject(void)
{
}

void CGameObject::Compute_ViewZ(const D3DXVECTOR3* pPos)
{
	if (nullptr == pPos)
		return;

	D3DXMATRIX		matView;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	D3DXVECTOR3		vCamPos = D3DXVECTOR3(matView._41, matView._42, matView._43);

	m_fViewZ = D3DXVec3Length(&(vCamPos - *pPos));
}

void CGameObject::Set_Pos(const _vec3 * pPos)
{
	m_pTransformCom->Set_Information(CTransform::INFO_POSITION, pPos);
}

void CGameObject::Set_Dir(const _vec3 * pDir)
{
	m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, pDir);
}

HRESULT CGameObject::Ready_GameObject(void)
{
	if (nullptr != m_pComponentMgr)
		return E_FAIL;

	m_pComponentMgr = Engine::CComponent_Manager::GetInstance();

	m_pComponentMgr->Add_Ref();

	return NOERROR;
}

int CGameObject::Update_GameObject(const float & fTimeDelta)
{
	int		iExitCode = 0;

	for (auto& pair : m_mapComponent[Engine::CComponent::COM_DYNAMIC])
	{
 		iExitCode = pair.second->Update_Component();
		if (iExitCode & 0x80000000)
			return iExitCode;
	}

	return iExitCode;
}

_vec3 CGameObject::Get_Pos(void)
{
	return m_pTransformCom->Get_Info(CTransform::INFO_POSITION);
}

CComponent* CGameObject::Get_Component(CComponent::COMTYPE eComType, const TCHAR* pComponentKey)
{
	auto iter = m_mapComponent[eComType].find(pComponentKey);

	if (iter == m_mapComponent[eComType].end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free(void)
{
	DWORD dwRefCnt = 0;

	for (_uint i = 0; i < Engine::CComponent::COM_END; ++i)
	{
		for (auto& pair : m_mapComponent[i])
			dwRefCnt = Safe_Release(pair.second);

		m_mapComponent[i].clear();
	}

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pComponentMgr);
	Safe_Release(m_pGraphicDev);
}