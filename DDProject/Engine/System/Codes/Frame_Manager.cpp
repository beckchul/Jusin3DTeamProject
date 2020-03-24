#include "Frame_Manager.h"
#include "Frame.h"

USING(Engine)

IMPLEMENT_SINGLETON(CFrame_Manager)

CFrame_Manager::CFrame_Manager()
{
}


CFrame_Manager::~CFrame_Manager()
{
}

HRESULT CFrame_Manager::Ready_Frame(const TCHAR * pFrameTag, CFrame * pFrame)
{
	CFrame* pFrame_Find = Find_Frame(pFrameTag);

	if (nullptr != pFrame_Find)
		return E_FAIL;

	m_mapFrame.insert(MAPFrame::value_type(pFrameTag, pFrame));		

	return NOERROR;
}

bool CFrame_Manager::Permit_Call(const TCHAR * pFrameTag, const float& fTimeDelta)
{
	CFrame* pFrame_Find = Find_Frame(pFrameTag);

	if (nullptr == pFrame_Find)
		return false;

	return pFrame_Find->Permit_Call(fTimeDelta);	
}

CFrame * CFrame_Manager::Find_Frame(const TCHAR * pFrameTag) const
{
	auto iter = m_mapFrame.find(pFrameTag);

	if(iter == m_mapFrame.end())
		return nullptr;

	return iter->second;
}

void CFrame_Manager::Free(void)
{
	/*for_each(m_mapFrame.begin(), m_mapFrame.end(), Engine::CRelease_Pair());*/

	unsigned long dwRefCnt = 0;

	for (auto& Pair : m_mapFrame)
	{
		dwRefCnt = Safe_Release(Pair.second);	
	}

	m_mapFrame.clear();
}
