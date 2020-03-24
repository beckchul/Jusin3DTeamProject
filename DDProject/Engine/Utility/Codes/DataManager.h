#ifndef DataManager_h__
#define DataManager_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class CGameObject;
class ENGINE_DLL CDataManager : public CBase
{
	DECLARE_SINGLETON(CDataManager)
private:
	explicit CDataManager();
	virtual ~CDataManager();
public:
	// getter
	vector<OBJECT_NAME*>&	Get_ObjName(void) { return m_vecObjName; }
	vector<OBJECT_NAME*>&	Get_ObjName2(void) { return m_vecObjName2; }
	vector<OBJECT_NAME*>&	Get_ObjName3(void) { return m_vecObjName3; }
	vector<_vec3>& Get_LightPosition(Engine::OBJECTID eId);
	vector<_vec3>& Get_MonsterRoute(Engine::OBJECTID eId);
	vector<_int>& Get_RequiredMp(int iPlayerType) { return m_vecRequireMp[iPlayerType]; }

	vector< vector<int> >		Get_AI_Pattern(void) { return  m_vecAIPattern; }
	vector<_vec3>				Get_AI_Position(void) { return m_vecMonsterPosition; }

	_matrix& Get_ViewMatrix(void) { return m_matView; }
	_matrix& Get_ViewInverseMatrix(void) { return m_matViewInverse; }
	_matrix& Get_ProjMatrix(void) { return m_matProj; }
	_matrix& Get_LightViewMatrix(void) { return m_matLightView;  }
	_matrix& Get_LightProjMatrix(void) { return m_matLightProj;  }
	_vec3& Get_CamPoistion(void) { return m_vecCamEye; }
	_vec3& Get_PlayerPosition(void) { return m_vecPlayerPos;  }
	_vec4& Get_BackColor(void) { return m_vecBackColor;  }
	_vec4& Get_FadeColor(void) { return m_vecFadeColor;  }
	string&	Get_Player_ID(_int iIndex) { return m_vecPlayer_ID[iIndex]; }
	_int& Get_SceneID(void) { return m_iSceneID;  }

	vector<WAVECREATE_INFO*>& Get_StageWave(_int _iStage) { return m_vecWaveCreateInfo[_iStage]; }
	_vec3 Get_MagneticInitPos(_int iIndex) { return *m_vecMagneticSphereInitPos[iIndex]; }
	_float Get_MagneticRadius(void) { return m_fMagneticSphereRadius; }

	// Setter
	void Set_ViewMatrix(_matrix& matView) { m_matView = matView;}
	void Set_ViewInverseMatrix(_matrix& matViewInv) { m_matViewInverse = matViewInv; }
	void Set_ProjMatrix(_matrix& matProj) { m_matProj = matProj; }
	void Set_CamPoistion(_vec3& vecEye) { m_vecCamEye = vecEye; }
	void Set_PlayerPosition(_vec3& vecPos) { m_vecPlayerPos = vecPos;  }
	void Set_LightViewMatrix(_matrix& matLightView) { m_matLightView = matLightView; }
	void Set_LightProjMatrix(_matrix& matLightProj) { m_matLightProj = matLightProj; }
	void Set_BackColor(_vec4& color) { m_vecBackColor = color;  }
	void Set_FadeColor(_vec4& color) { m_vecFadeColor = color;  }
	void Set_Player_ID(_uint uIndex, string& s) { if(m_vecPlayer_ID.size() > uIndex) m_vecPlayer_ID[uIndex] = s; }
	void Set_Scene_ID(_int iSceneID) { m_iSceneID = iSceneID; }

	void Set_AI_Pattern(vector< vector<int> >& vecAIPattern) { m_vecAIPattern = vecAIPattern; }
	void Set_AI_Position(vector<_vec3> vecAIPos) { m_vecMonsterPosition = vecAIPos; }
	void Set_MageticRadius(_float& fRadius) { m_fMagneticSphereRadius = fRadius; }

	HRESULT Initialize_DataMgr(void);
	HRESULT Load_MapObjName(void);
	HRESULT Load_WaveCreateInfo(void);
public:
	void Add_PointLight(_vec3 vecLight, Engine::OBJECTID eId, int eLightFirst = OBJECT_POINT_LIGHT0);
	void Add_MonsterRoute(_vec3 vecRoute, Engine::OBJECTID eId, int eMonsterFirst = OBJECT_MONSTER_ROUTE0);

	void Add_EasterEgg(CGameObject* pGameObject, _vec3& vecPos);
	void Delete_EasterEgg(void);
	void Save_EasterEgg(void);

	void Reset_Data(void);

private:
	vector<OBJECT_NAME*>		m_vecObjName; // Stage Object 이름들
	vector<OBJECT_NAME*>		m_vecObjName2; // Stage2 Object 이름들
	vector<OBJECT_NAME*>		m_vecObjName3; // Lobby Object 이름들

	vector<_vec3>				m_vecPointLight[6]; // 점 조명
	vector<_vec3>				m_vecMonsterRoute[8]; // 몬스터 길

	_matrix						m_matView; // View Matrix
	_matrix						m_matViewInverse; // View Matrix의 Inverse
	_matrix						m_matProj; // Projection Matrix
	_matrix						m_matLightView;
	_matrix						m_matLightProj;
	_vec3						m_vecCamEye; // 카메라 위치
	_vec3						m_vecPlayerPos;
	_int						m_iSceneID = 0;

	vector<WAVECREATE_INFO*>	m_vecWaveCreateInfo[3];

	vector<pair<CGameObject*, _vec3>> m_vecEasterEgg;

	_vec4						m_vecBackColor;
	_vec4						m_vecFadeColor;

	vector<_int>				m_vecRequireMp[2]; // Mage, Monk 순서
	
	vector<string>				m_vecPlayer_ID; // 플레이어 ID 저장

	vector< vector<int> >		m_vecAIPattern;
	vector<_vec3>				m_vecMonsterPosition;

	vector<_vec3*>				m_vecMagneticSphereInitPos;	// Minkyeong
	_float								m_fMagneticSphereRadius;
private:
	virtual void Free(void);
};

END

#endif