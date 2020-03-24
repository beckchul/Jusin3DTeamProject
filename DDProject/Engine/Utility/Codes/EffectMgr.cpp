#include "EffectMgr.h"
#include "Effect.h"

IMPLEMENT_SINGLETON(Engine::CEffectMgr)
USING(Engine)

Engine::CEffectMgr::CEffectMgr(void)
{

}

Engine::CEffectMgr::~CEffectMgr(void)
{

}


PARTICLE_INFO* Engine::CEffectMgr::Find_ParticleInfo(const _tchar* pParticleTag)
{
	auto& iter = find_if(m_mapParticleInfo.begin(), m_mapParticleInfo.end()
		, [&](MAP_PARTICLEINFO::value_type& Pair) { return !lstrcmp(Pair.first, pParticleTag); });

	if (iter == m_mapParticleInfo.end())
		return nullptr;

	return iter->second;
}


HRESULT Engine::CEffectMgr::Add_EffectList(const _tchar* pEffectTag, list<CEffect*> pEffectList)
{
	auto& iter = find_if(m_mapEffectKind.begin(), m_mapEffectKind.end()
		, [&](unordered_map<const _tchar*, list<CEffect*>>::value_type& Pair) { return !lstrcmp(Pair.first, pEffectTag); });

	if (iter == m_mapEffectKind.end()) // 없으면 추가해준다.
	{
		m_mapEffectKind.insert(unordered_map<const _tchar*, list<CEffect*>>::value_type(pEffectTag, pEffectList));
		return S_OK;
	}
	else
	{
		MSG_BOX(L"이미 존재하는 이펙트 리스트 입니다.");
		return E_FAIL;
	}
	return E_FAIL;
}



HRESULT Engine::CEffectMgr::Init_EffectData(void)
{
	DWORD dwByte = 0;

	//----------------------------------------------------------------------------------------------------
	HANDLE hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Shield.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	PARTICLE_INFO* pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Shield", pParticleInfo));
	//----------------------------------------------------------------------------------------------------


	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/GatherToPurpleCrystal.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_GatherToPurpleCrystal", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Gravity.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);	

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Gravity", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Torch.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);	

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Torch", pParticleInfo));
	//----------------------------------------------------------------------------------------------------


	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/PurpleStars.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_PurpleStars", pParticleInfo));
	//----------------------------------------------------------------------------------------------------
	
	
	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/ManaBomb.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_ManaBomb", pParticleInfo));
	//----------------------------------------------------------------------------------------------------


	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Red_Missile.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Red_MissileDel", pParticleInfo));
	//----------------------------------------------------------------------------------------------------


	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/TowerCreate.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_TowerCreate", pParticleInfo));
	//----------------------------------------------------------------------------------------------------


	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/TowerCreate_Gather.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_TowerCreate_Gather", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Green_Missile.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Green_Missile", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/RectParticle.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_RectParticle", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/FirePlace.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	_vec3 vDir = pParticleInfo->vDir;
	pParticleInfo->vDir = _vec3(vDir.x, vDir.y - 9.f, vDir.z + 5.f);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_FirePlace", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/GreenBlood.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_GreenBlood", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Flame.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Flame", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Under_Flame.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Under_Flame", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/ChargeAtt.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_ChargeAtt", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/GameStart_PurpleStar.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_GameStart_PurpleStar", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/FireTower_Smoke.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_FireTower_Smoke", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/SlowAura_Bubble.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_SlowAura_Bubble", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Small_Lava_Bubble.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Small_Lava_Bubble", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Lava_Bubble.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Lava_Bubble", pParticleInfo));
	//----------------------------------------------------------------------------------------------------


	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/MageManaBomb_Att.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_MageManaBomb_Att", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Tower_Bulid.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Tower_Bulid", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Healing_Beam.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Healing_Beam", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Healing_Star.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Healing_Star", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Mage_Pink_Missile.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Mage_Pink_Missile", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Brazier_MinusX.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Brazier_MinusX", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Brazier_Smoke.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Brazier_Smoke", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/TrackPath_Test.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_TrackPath_Test", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Demon_Fire.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Demon_Fire", pParticleInfo));
	//----------------------------------------------------------------------------------------------------
	
	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/MissileTower_Track.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_MissileTower_Track", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Alive_Star.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Alive_Star", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Monster_Dead_Star.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Monster_Dead_Star", pParticleInfo));
	//----------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Tower_Destruction_Smoke.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL

		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Tower_Destruction_Smoke", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Demon_Hand.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Demon_Hand", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/FireTower_Track.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_FireTower_Track", pParticleInfo));
	//----------------------------------------------------------------------------------------------------


	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Drain_Aura.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Drain_Aura", pParticleInfo));
	//----------------------------------------------------------------------------------------------------


	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Enrage_Aura.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Enrage_Aura", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Healing_Aura.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Healing_Aura", pParticleInfo));
	//----------------------------------------------------------------------------------------------------


	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Lighting_Aura.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Lighting_Aura", pParticleInfo));
	//----------------------------------------------------------------------------------------------------


	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Spotlight_Star.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Spotlight_Star", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Recall_Star.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Recall_Star", pParticleInfo));
	//----------------------------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------------------------
	hFile = CreateFile(L"../Resources/Data/EffectData/Particle_Data/Brazier_Fire.dat"
		, GENERIC_READ
		, NULL
		, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	pParticleInfo = new PARTICLE_INFO;
	ReadFile(hFile, pParticleInfo, sizeof(PARTICLE_INFO), &dwByte, NULL);

	m_mapParticleInfo.insert(MAP_PARTICLEINFO::value_type(L"Particle_Brazier_Fire", pParticleInfo));
	//----------------------------------------------------------------------------------------------------


	
	CloseHandle(hFile);

	return NOERROR;
}

void Engine::CEffectMgr::Update_Effect(const float& fTimeDelta)
{
	list<CEffect*>::iterator iter_begin = m_listEffect.begin();
	list<CEffect*>::iterator iter_end = m_listEffect.end();

	for (; iter_begin != iter_end;)
	{
		if (1 == (*iter_begin)->Update_GameObject(fTimeDelta))
		{
			Engine::Safe_Release(*iter_begin);
			iter_begin = m_listEffect.erase(iter_begin);
		}
		else
			iter_begin++;
	}
}

void Engine::CEffectMgr::Render_Effect(void)
{
	list<CEffect*>::iterator iter_begin = m_listEffect.begin();
	list<CEffect*>::iterator iter_end = m_listEffect.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		(*iter_begin)->Render_GameObject();
	}
}

void Engine::CEffectMgr::Release_Effect(void)
{
	for_each(m_listEffect.begin(), m_listEffect.end(), CRelease_Single());

	m_listEffect.clear();
}

void Engine::CEffectMgr::Kill_TheseEffectlist(list<CEffect*> pEffectlist)
{
	if (pEffectlist.size() == 0)
		return;

	list<CEffect*>::iterator iter_begin = pEffectlist.begin();
	list<CEffect*>::iterator  iter_end = pEffectlist.end();

	for (; iter_begin != iter_end; ++iter_begin)
		(*iter_begin)->Set_Dead();
	
	pEffectlist.clear();
}

void CEffectMgr::Reset_Effect(void)
{
	DWORD dwRefCnt = 0;

	for_each(m_listEffect.begin(), m_listEffect.end(), CRelease_Single());

	for (auto& Pair : m_mapEffectKind)
		for (auto& pEffect : Pair.second)
			dwRefCnt = Safe_Release(pEffect);

	for (auto& Pair : m_mapParticleInfo)
		Safe_Delete(Pair.second);

	m_listEffect.clear();
	m_mapEffectKind.clear();
	m_mapParticleInfo.clear();
}


CEffect* Engine::CEffectMgr::Add_Effect(CEffect* pEffect)
{
	m_listEffect.push_back(pEffect);

	return pEffect;		// 주소 리턴해서 가질려면 가지고..
							// 자기가 원하는 타이밍에 Dead시키기 위함 ?
}


list<CEffect*> Engine::CEffectMgr::Add_Effect(const _tchar* pEffectTag, _vec3* vOriginPos)
{
	// 인덱스 접근

	auto& iter = find_if(m_mapEffectKind.begin(), m_mapEffectKind.end()
		, [&](unordered_map<const _tchar*, list<CEffect*>>::value_type& Pair) { return !lstrcmp(Pair.first, pEffectTag); });

	list<CEffect*>::iterator iter_begin = iter->second.begin();
	list<CEffect*>::iterator iter_end = iter->second.end();
	
	list<CEffect*> TempList;

	for (; iter_begin != iter_end; ++iter_begin)
	{
   		CEffect* pEffect = (*iter_begin)->Clone();

 		pEffect->Set_Information(CTransform::INFO_SCALE, &((*iter_begin)->Get_Information(CTransform::INFO_SCALE)));
		pEffect->Set_Information(CTransform::INFO_ANGLE, &((*iter_begin)->Get_Information(CTransform::INFO_ANGLE)));
		pEffect->Set_OriginPos(*vOriginPos);
		m_listEffect.push_back(pEffect);

		// 반환용
		TempList.push_back(pEffect);
	}

	return TempList;
}

list<CEffect*> Engine::CEffectMgr::Add_Effect(const _tchar* pEffectTag, _vec3* vOriginPos, _vec3 FinalPos, _float fSpeed)
{
	auto& iter = find_if(m_mapEffectKind.begin(), m_mapEffectKind.end()
		, [&](unordered_map<const _tchar*, list<CEffect*>>::value_type& Pair) { return !lstrcmp(Pair.first, pEffectTag); });

	list<CEffect*>::iterator iter_begin = iter->second.begin();
	list<CEffect*>::iterator iter_end = iter->second.end();

	list<CEffect*> TempList;

	for (; iter_begin != iter_end; ++iter_begin)
	{
		CEffect* pEffect = (*iter_begin)->Clone();

		pEffect->Set_Information(CTransform::INFO_SCALE, &((*iter_begin)->Get_Information(CTransform::INFO_SCALE)));
		pEffect->Set_Information(CTransform::INFO_ANGLE, &((*iter_begin)->Get_Information(CTransform::INFO_ANGLE)));
		pEffect->Set_OriginPos(*vOriginPos);
		pEffect->Set_FinalPos(FinalPos);
		pEffect->Set_EffectSpeed(fSpeed);
		m_listEffect.push_back(pEffect);
		// 반환용
		TempList.push_back(pEffect);
	}

	return TempList;
}


list<CEffect*> Engine::CEffectMgr::Add_Effect(const _tchar* pEffectTag, _vec3* vOriginPos, _vec3 vDir, _float fSpeed, _bool bIsDir)
{
	auto& iter = find_if(m_mapEffectKind.begin(), m_mapEffectKind.end()
		, [&](unordered_map<const _tchar*, list<CEffect*>>::value_type& Pair) { return !lstrcmp(Pair.first, pEffectTag); });

	list<CEffect*>::iterator iter_begin = iter->second.begin();
	list<CEffect*>::iterator iter_end = iter->second.end();

	list<CEffect*> TempList;

	for (; iter_begin != iter_end; ++iter_begin)
	{
		CEffect* pEffect = (*iter_begin)->Clone();

		pEffect->Set_Information(CTransform::INFO_SCALE, &((*iter_begin)->Get_Information(CTransform::INFO_SCALE)));
		pEffect->Set_Information(CTransform::INFO_ANGLE, &((*iter_begin)->Get_Information(CTransform::INFO_ANGLE)));
		pEffect->Set_OriginPos(*vOriginPos);
		pEffect->Set_UseDir(vDir);
		pEffect->Set_EffectSpeed(fSpeed);
		m_listEffect.push_back(pEffect);
		// 반환용
		TempList.push_back(pEffect);
	}
	return TempList;

}

list<CEffect*> Engine::CEffectMgr::Add_Effect(const _tchar* pEffectTag, _matrix* pmatInfo)
{
	// 그냥 행렬 하나만 받아오는 이펙트의 경우
	auto& iter = find_if(m_mapEffectKind.begin(), m_mapEffectKind.end()
		, [&](unordered_map<const _tchar*, list<CEffect*>>::value_type& Pair) { return !lstrcmp(Pair.first, pEffectTag); });

	list<CEffect*>::iterator iter_begin = iter->second.begin();
	list<CEffect*>::iterator iter_end = iter->second.end();

	list<CEffect*> TempList;

	for (; iter_begin != iter_end; ++iter_begin)
	{
		CEffect* pEffect = (*iter_begin)->Clone();

		pEffect->Set_Information(CTransform::INFO_SCALE, &((*iter_begin)->Get_Information(CTransform::INFO_SCALE)));
		pEffect->Set_Information(CTransform::INFO_ANGLE, &((*iter_begin)->Get_Information(CTransform::INFO_ANGLE)));

		_vec3 vRevisionPos(0.f, 0.f, 0.f);
		memcpy(&vRevisionPos, &pmatInfo->m[3][0], sizeof(_vec3));
		pEffect->Set_OriginPos(vRevisionPos);		// 첫 위치부터 잡아주고 FixMatrix

		pEffect->Set_FixMatrix(pmatInfo);
		m_listEffect.push_back(pEffect);
		// 반환용
		TempList.push_back(pEffect);
	}

	return TempList;

}

std::list<CEffect*> Engine::CEffectMgr::Add_Effect(const _tchar* pEffectTag, _matrix* pmatInfo, _vec3 vTargetPos)
{
	auto& iter = find_if(m_mapEffectKind.begin(), m_mapEffectKind.end()
		, [&](unordered_map<const _tchar*, list<CEffect*>>::value_type& Pair) { return !lstrcmp(Pair.first, pEffectTag); });

	list<CEffect*>::iterator iter_begin = iter->second.begin();
	list<CEffect*>::iterator iter_end = iter->second.end();

	list<CEffect*> TempList;

	for (; iter_begin != iter_end; ++iter_begin)
	{
		CEffect* pEffect = (*iter_begin)->Clone();

		pEffect->Set_Information(CTransform::INFO_SCALE, &((*iter_begin)->Get_Information(CTransform::INFO_SCALE)));
		pEffect->Set_Information(CTransform::INFO_ANGLE, &((*iter_begin)->Get_Information(CTransform::INFO_ANGLE)));

		_vec3 vRevisionPos(0.f, 0.f, 0.f);
		memcpy(&vRevisionPos, &pmatInfo->m[3][0], sizeof(_vec3));
		pEffect->Set_OriginPos(vRevisionPos);		// 첫 위치부터 잡아주고 FixMatrix

		pEffect->Set_TargetPos(vTargetPos);
		pEffect->Set_FixMatrix(pmatInfo);
		m_listEffect.push_back(pEffect);
		// 반환용
		TempList.push_back(pEffect);
	}

	return TempList;
}



list<CEffect*> Engine::CEffectMgr::Add_Effect(const _tchar* pEffectTag, _matrix* pmatInfo, _matrix* pmatParent)
{
		// 행렬 + 부모행렬까지 받아와야 되는 이펙트의 경우
	auto& iter = find_if(m_mapEffectKind.begin(), m_mapEffectKind.end()
		, [&](unordered_map<const _tchar*, list<CEffect*>>::value_type& Pair) { return !lstrcmp(Pair.first, pEffectTag); });

	list<CEffect*>::iterator iter_begin = iter->second.begin();
	list<CEffect*>::iterator iter_end = iter->second.end();

	list<CEffect*> TempList;

	for (; iter_begin != iter_end; ++iter_begin)
	{
		CEffect* pEffect = (*iter_begin)->Clone();

		pEffect->Set_Information(CTransform::INFO_SCALE, &((*iter_begin)->Get_Information(CTransform::INFO_SCALE)));
		pEffect->Set_Information(CTransform::INFO_ANGLE, &((*iter_begin)->Get_Information(CTransform::INFO_ANGLE)));
		
		_matrix TempMat = (*pmatInfo) *  (*pmatParent);
		_vec3 vRevisionPos(0.f, 0.f, 0.f);
		memcpy(&vRevisionPos, &TempMat.m[3][0], sizeof(_vec3));
		pEffect->Set_OriginPos(vRevisionPos);		// 첫 위치부터 잡아주고 FixMatrix

		pEffect->Set_FixMatrix(pmatInfo, pmatParent);
		m_listEffect.push_back(pEffect);
		// 반환용
		TempList.push_back(pEffect);
	}

	return TempList;
}

void Engine::CEffectMgr::Free(void)
{
	DWORD dwRefCnt = 0;

	for_each(m_listEffect.begin(), m_listEffect.end(), CRelease_Single());

	for (auto& Pair : m_mapEffectKind)
		for (auto& pEffect : Pair.second)
			dwRefCnt = Safe_Release(pEffect);

	for (auto& Pair : m_mapParticleInfo)
		Safe_Delete(Pair.second);

	m_listEffect.clear();
	m_mapEffectKind.clear();
	m_mapParticleInfo.clear();
}
