#include "DataManager.h"
#include "GameObject.h"
#include "Time.h"

USING(Engine)

IMPLEMENT_SINGLETON(CDataManager)

CDataManager::CDataManager()
	: m_vecBackColor(0.f, 0.f, 0.f, 0.f)
	, m_vecPlayer_ID(30)
{
}

CDataManager::~CDataManager()
{
}

vector<_vec3>& CDataManager::Get_LightPosition(Engine::OBJECTID eId)
{
	int iIndex = eId - Engine::OBJECT_POINT_LIGHT0; // 0, 1, 2, 3, 4

	return m_vecPointLight[iIndex];
}

vector<_vec3>& CDataManager::Get_MonsterRoute(Engine::OBJECTID eId)
{
	int iIndex = eId - Engine::OBJECT_MONSTER_ROUTE0; // 0, 1, 2, ... 7

	return m_vecMonsterRoute[iIndex];
}

HRESULT CDataManager::Initialize_DataMgr(void)
{
	if (FAILED(Load_MapObjName()))
		return E_FAIL;

	m_vecRequireMp[2];

	// Mage
	int iMageMpArray[10] = { 150, 0, 8, 200, 0, 0, 40, 20, 80, 120 };

	for (int i = 0; i < 10; ++i)
		m_vecRequireMp[0].push_back(iMageMpArray[i]);

	// Monk
	int iMonkMpArray[10] = { 100, 0, 15, 15, 0, 0, 30, 50, 40, 60 };

	for (int i = 0; i < 10; ++i)
		m_vecRequireMp[1].push_back(iMonkMpArray[i]);

	m_vecPlayer_ID;


	_vec3* pPos = new _vec3;

	*pPos = _vec3(-554.f, -203.f, -381.f);
	m_vecMagneticSphereInitPos.push_back(pPos);

	pPos = new _vec3;
	*pPos = _vec3(-755.f, 10.f, 735.f);
	m_vecMagneticSphereInitPos.push_back(pPos);

	pPos = new _vec3;
	*pPos = _vec3(-846.f, 113.f, 134.f);
	m_vecMagneticSphereInitPos.push_back(pPos);

	return NOERROR;
}

HRESULT CDataManager::Load_MapObjName(void)
{
	DWORD dwByte = 0;

	// Stage1
	HANDLE hFile = CreateFile(L"../Resources/Data/Object/Name.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	while (true)
	{
		OBJECT_NAME* objName = new OBJECT_NAME;

		ReadFile(hFile, objName, sizeof(OBJECT_NAME), &dwByte, NULL);

		if (dwByte == 0)
		{
			delete objName;
			break;
		}

		m_vecObjName.push_back(objName);
	}
	CloseHandle(hFile);

	// Stage2
	HANDLE hFile2 = CreateFile(L"../Resources/Data/Object/Stage2_Name.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	while (true)
	{
		OBJECT_NAME* objName2 = new OBJECT_NAME;

		ReadFile(hFile2, objName2, sizeof(OBJECT_NAME), &dwByte, NULL);

		if (dwByte == 0)
		{
			delete objName2;
			break;
		}

		m_vecObjName2.push_back(objName2);
	}
	CloseHandle(hFile2);

	// Stage3
	HANDLE hFile3 = CreateFile(L"../Resources/Data/Object/Lobby_Name.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	while (true)
	{
		OBJECT_NAME* objName3 = new OBJECT_NAME;

		ReadFile(hFile3, objName3, sizeof(OBJECT_NAME), &dwByte, NULL);

		if (dwByte == 0)
		{
			delete objName3;
			break;
		}

		m_vecObjName3.push_back(objName3);
	}
	CloseHandle(hFile3);

	return NOERROR;
}

HRESULT CDataManager::Load_WaveCreateInfo()
{
	DWORD dwByte = 0;
	HANDLE hFile;
	for (int i = 0; i < 3; ++i)
	{
		switch (i)
		{
		case 0:
			hFile = CreateFile(L"../Resources/Data/Wave/Wave.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			break;
		case 1:
			hFile = CreateFile(L"../Resources/Data/Wave/Wave2.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			break;
		case 2:
			hFile = CreateFile(L"../Resources/Data/Wave/Wave3.dat", GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			break;

			return NOERROR;
		}

		while (true)
		{
			WAVECREATE_INFO* pWaveInfo = new WAVECREATE_INFO;

			ReadFile(hFile, pWaveInfo, sizeof(WAVECREATE_INFO), &dwByte, NULL);

			if (dwByte == 0)
			{
				delete pWaveInfo;
				break;
			}

			m_vecWaveCreateInfo[i].push_back(pWaveInfo);
		}
		CloseHandle(hFile);
	}
	return NOERROR;
}

void CDataManager::Add_PointLight(_vec3 vecLight, Engine::OBJECTID eId, int eLightFirst)
{
	m_vecPointLight[eId - eLightFirst].push_back(vecLight);
}

void CDataManager::Add_MonsterRoute(_vec3 vecRoute, Engine::OBJECTID eId, int eMonsterFirst)
{
	m_vecMonsterRoute[eId - eMonsterFirst].push_back(vecRoute);
}

void CDataManager::Add_EasterEgg(CGameObject * pGameObject, _vec3 & vecPos)
{
	m_vecEasterEgg.push_back(make_pair(pGameObject, vecPos));
}

void CDataManager::Delete_EasterEgg(void)
{
	if (m_vecEasterEgg.size() == 0)
		return;

	m_vecEasterEgg.back().first->Set_Dead();
	m_vecEasterEgg.pop_back();
}

void CDataManager::Save_EasterEgg(void)
{
	if (m_vecEasterEgg.size() == 0)
		return;

	HANDLE hFile;
	DWORD dwByte = 0;

	time_t timer = time(NULL);
	tm t;
	localtime_s(&t, &timer);

	TCHAR szFileName[64];
	wsprintf(szFileName, L"../Resources/Data/EasterEgg/%d월%d일%d시%d분%d초.data", t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

	hFile = CreateFile(szFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	for (size_t i = 0; i < m_vecEasterEgg.size(); ++i)
		WriteFile(hFile, &m_vecEasterEgg[i].second, sizeof(_vec3), &dwByte, NULL);

	CloseHandle(hFile);
}

void CDataManager::Reset_Data(void)
{
	for(int i=0; i<6; ++i)
		m_vecPointLight[i].clear();

	for (int i = 0; i < 8; ++i)
		m_vecMonsterRoute[i].clear();
}

void CDataManager::Free(void)
{
	for (auto& objName : m_vecObjName)
		Engine::Safe_Delete(objName);

	for (auto& objName2 : m_vecObjName2)
		Engine::Safe_Delete(objName2);

	for (auto& objName3 : m_vecObjName3)
		Engine::Safe_Delete(objName3);

	for (int i = 0; i < 3; ++i)
	{
		for (_uint j = 0; j < m_vecWaveCreateInfo[i].size(); ++j)
			Engine::Safe_Delete(m_vecWaveCreateInfo[i][j]);

		m_vecWaveCreateInfo[i].clear();
	}
	
	Save_EasterEgg();
}
