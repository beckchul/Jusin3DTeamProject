#include "Layer.h"


USING(Engine)

CLayer::CLayer(void)
	:m_bFirstUpdate(FALSE)
{
}

CLayer::~CLayer(void)
{

}

const CComponent* CLayer::Get_Component(CComponent::COMTYPE eComType, const TCHAR* pComponentKey, const _uint& uIndex)
{
	auto iter = m_ObjectList.begin();

	for (_uint i = 0; i < uIndex; ++i)
		++iter;

	return (*iter)->Get_Component(eComType, pComponentKey);
}

HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_ObjectList.push_back(pGameObject);

	return S_OK;
}

HRESULT CLayer::Ready_Layer(void)
{
	m_bFirstUpdate = FALSE;

	return NOERROR;
}

int CLayer::Update_Layer(const float& fTimeDelta)
{
	int		iExitCode = 0;

	list<CGameObject*>::iterator iter_begin = m_ObjectList.begin();
	list<CGameObject*>::iterator iter_end = m_ObjectList.end();

	if (FALSE == m_bFirstUpdate) // First Update
	{
		m_bFirstUpdate = TRUE;

		for (; iter_begin != iter_end; ++iter_begin)
			(*iter_begin)->FirstUpdate_GameObject(fTimeDelta);

		iter_begin = m_ObjectList.begin();
	}

	for (; iter_begin != iter_end;) // ÀÏ¹Ý Update
	{
		iExitCode = (*iter_begin)->Update_GameObject(fTimeDelta);

		if (1 == iExitCode)
		{
			Engine::Safe_Release(*iter_begin);
			iter_begin = m_ObjectList.erase(iter_begin);
		}
		else
			iter_begin++;
	}

	return iExitCode;
}

CLayer* CLayer::Create(void)
{
	CLayer*		pInstance = new CLayer;

	if (FAILED(pInstance->Ready_Layer()))	
	{
		MSG_BOX(L"CLayer Created Failed");
		Engine::Safe_Release(pInstance);
	}
	return pInstance;
}

void CLayer::Free(void)
{
	DWORD dwRefCnt = 0;

	for_each(m_ObjectList.begin(), m_ObjectList.end(), CRelease_Single());

	m_ObjectList.clear();
}