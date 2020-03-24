#include "Object_Manager.h"
#include "Layer.h"
#include "GameObject.h"

USING(Engine)
IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
	: m_pMapLayer(nullptr)
	, m_iMaxContainerSize(0)
{
}


CObject_Manager::~CObject_Manager()
{
}

const CComponent * CObject_Manager::Get_Component(const _uint& iSceneID, const _tchar* pLayerTag, CComponent::COMTYPE eComType, const TCHAR * pComponentKey, _uint uIndex)
{
	auto iter = m_pMapLayer[iSceneID].find(pLayerTag);

	if (iter == m_pMapLayer[iSceneID].end())
		return nullptr;

	return iter->second->Get_Component(eComType, pComponentKey, uIndex);
}

HRESULT CObject_Manager::Reserve_ObjectMgr(const int & iContainerSize)
{
	if (nullptr != m_pMapLayer)
		return E_FAIL;

	m_pMapLayer = new MAPLAYER[iContainerSize];

	m_iMaxContainerSize = iContainerSize;

	return NOERROR;
}

HRESULT CObject_Manager::Add_GameObject(int eID, const TCHAR * pLayerTag, CGameObject * pGameObject)
{
	if (nullptr == m_pMapLayer)
	{
		MSG_BOX(L"Not Reserved Container");
		return E_FAIL;
	}

	auto iter = m_pMapLayer[eID].find(pLayerTag);

	if (iter == m_pMapLayer[eID].end()) // ������ü�� �߰��Ϸ����� ���̾ ����.
	{
		CLayer*	pLayer = CLayer::Create(); // �׷��� ���̾ �����.
		if (nullptr == pLayer)
			return E_FAIL;
		pLayer->Add_GameObject(pGameObject); // ���� ���̾ �߰��Ϸ��ߴ� ��ü�� �߰��Ѵ�.
		m_pMapLayer[eID].insert(MAPLAYER::value_type(pLayerTag, pLayer)); // �̷��� ���� ���̾ �ʿ� �����ؼ� CObject_Manager�� ������ ���� �� �ֵ��� �Ѵ�.
	}
	else // ������ü�� �߰��Ϸ����� ���̾ �ִ�.
	{
		// iter->seoncd : CLayer*
		iter->second->Add_GameObject(pGameObject);
	}

	return NOERROR;
}

int CObject_Manager::Update_ObjMgr(const float & fTimeDelta)
{
	if (nullptr == m_pMapLayer)
	{
		MSG_BOX(L"Not Reserved Container");
		return -1;
	}

	int		iExitCode = 0;

	for (_uint i = 0; i < m_iMaxContainerSize; ++i)
	{
		for (auto& Pair : m_pMapLayer[i])
		{
			// Pair.second : CLayer*
			iExitCode = Pair.second->Update_Layer(fTimeDelta);

			if (iExitCode & 0x80000000)
				return iExitCode;
		}		
	}
	return iExitCode;
}

void CObject_Manager::Release_GameObject(const _uint & iSceneIdx)
{
	if (nullptr == m_pMapLayer)
		return;

	for (auto& Pair : m_pMapLayer[iSceneIdx])
	{
		Engine::Safe_Release(Pair.second);
	}
	m_pMapLayer[iSceneIdx].clear();
}

void CObject_Manager::Free(void)
{
	if (nullptr == m_pMapLayer)
	{
		MSG_BOX(L"Not Reserved Container");
		return;
	}

	for (_uint i = 0; i < m_iMaxContainerSize; ++i)
	{
		for_each(m_pMapLayer[i].begin(), m_pMapLayer[i].end(), CRelease_Pair());

		m_pMapLayer[i].clear();
	}		
	
	Engine::Safe_Delete_Array(m_pMapLayer);
}
