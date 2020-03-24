#include "Target_Manager.h"
#include "Target.h"

USING(Engine)
IMPLEMENT_SINGLETON(CTarget_Manager)

CTarget_Manager::CTarget_Manager()
{
}


CTarget_Manager::~CTarget_Manager()
{
}

HRESULT CTarget_Manager::Add_RenderTarget(LPDIRECT3DDEVICE9 pGraphicDev, const _tchar * pTargetTag, const _uint & iSizeX, const _uint & iSizeY, D3DFORMAT Format, D3DXCOLOR Color, bool bShadowMap)
{
	auto pTarget = Find_Target(pTargetTag);

	if (nullptr != pTarget)
		return E_FAIL;

	pTarget = CTarget::Create(pGraphicDev, iSizeX, iSizeY, Format, Color, bShadowMap);

	if (nullptr == pTarget)
		return E_FAIL;

	m_mapTargets.insert(MAPTARGETS::value_type(pTargetTag, pTarget));

	return NOERROR;
}

HRESULT CTarget_Manager::SetUp_OnGraphicDev(const _tchar * pTargetTag, const _uint & iIndex)
{
	auto pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->SetUp_OnGraphicDev(iIndex);
}

HRESULT CTarget_Manager::SetUp_OnShader(LPD3DXEFFECT pEffect, const _tchar * pTargetTag, const char* pConstantName)
{
	auto pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->SetUp_OnShader(pEffect, pConstantName);
}

HRESULT CTarget_Manager::Add_MRT(const _tchar * pMRTTag, const _tchar * pTargetTag)
{
	auto pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
	{
		list<CTarget*>		MRTList;
		MRTList.push_back(pTarget);
		m_mapMRT.insert(MAPMRT::value_type(pMRTTag, MRTList));
	}
	else
		pMRTList->push_back(pTarget);

	pTarget->Add_Ref();

	return NOERROR;
}

HRESULT CTarget_Manager::Begin_MRT(const _tchar * pMRTTag)
{
	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	_uint			iIndex = 0;

	for (auto& pTarget : *pMRTList)
		pTarget->Clear_Target();

	for (auto& pTarget : *pMRTList)
		pTarget->SetUp_OnGraphicDev(iIndex++);
	
	return NOERROR;
}

HRESULT CTarget_Manager::End_MRT(const _tchar * pMRTTag)
{
	list<CTarget*>*	pMRTList = Find_MRT(pMRTTag);

	if (nullptr == pMRTList)
		return E_FAIL;

	for (auto& pTarget : *pMRTList)
		pTarget->Release_OnGraphicDev();

	return NOERROR;
}

HRESULT CTarget_Manager::Begin_SingleTarget(const _tchar * pTargetTag)
{
	CTarget* pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	pTarget->Clear_Target();

	pTarget->SetUp_OnGraphicDev(0);

	return NOERROR;
}

HRESULT CTarget_Manager::End_SingleTarget(const _tchar * pTargetTag)
{
	CTarget* pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	pTarget->Release_OnGraphicDev();

	return NOERROR;
}

HRESULT CTarget_Manager::Ready_DebugBuffer(const _tchar * pTargetTag, _vec4 * pBufferInfo)
{
	auto pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->Ready_DebugBuffer(pBufferInfo);
}

HRESULT CTarget_Manager::Render_DebugBuffer(const _tchar * pTargetTag)
{
	auto pTarget = Find_Target(pTargetTag);

	if (nullptr == pTarget)
		return E_FAIL;

	return pTarget->Render_DebugBuffer();
}

CTarget * CTarget_Manager::Find_Target(const _tchar * pTargetTag)
{
	//find_if(m_mapTargets.begin(), m_mapTargets.end(), CTag_Finder(pTargetTag));

	auto& iter = find_if(m_mapTargets.begin(), m_mapTargets.end()
		, [&](MAPTARGETS::value_type& Pair)/*->bool*/ { return !lstrcmp(Pair.first, pTargetTag); });

	if (iter == m_mapTargets.end())
		return nullptr;
	return iter->second;
}

list<CTarget*>* CTarget_Manager::Find_MRT(const _tchar * pMRTTag)
{
	auto& iter = find_if(m_mapMRT.begin(), m_mapMRT.end()
		, [&](MAPMRT::value_type& Pair)/*->bool*/ { return !lstrcmp(Pair.first, pMRTTag); });

	if (iter == m_mapMRT.end())
		return nullptr;

	return &iter->second;
}

void CTarget_Manager::Free(void)
{
	DWORD dwRefCnt = 0;

	for (auto& Pair : m_mapMRT)
		for (auto& Target : Pair.second)
			dwRefCnt = Safe_Release(Target);

	for (auto& Pair : m_mapTargets)
		dwRefCnt = Safe_Release(Pair.second);

	m_mapMRT.clear();
	m_mapTargets.clear();
}
