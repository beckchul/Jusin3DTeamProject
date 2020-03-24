#ifndef Macro_h__
#define Macro_h__

#ifdef Demonstration
#define WINCX 800 // 시연회
#define WINCY 600

#define SCALE_WINCX WINCX / 1024.f // 현재 화면에 따른 비율(작업 환경이 1024 X 768 이므로 이 값으로 고정할 것)
#define SCALE_WINCY WINCY / 768.f

#else 

	#ifdef BadComputer
	#define WINCX 1024 // 테스트
	#define WINCY 768
	#else
	#define WINCX 1024 // 테스트
	#define WINCY 768
	#endif


#define SCALE_WINCX WINCX / 1024.f // 현재 화면에 따른 비율(작업 환경이 1024 X 768 이므로 이 값으로 고정할 것)
#define SCALE_WINCY WINCY / 768.f
#endif

#define DEFAULT_FPS 60.f

#define PLANE_NEAR 0.2f
#define PLANE_FAR 10000.f

//#ifdef Demonstration
#define LIMIT_FPS
//#endif

#define MAX_CLIENT_NUM 5 // 최대 클라이언트 수 (실제론 -1명)
#define MAX_CLIENT_NUM2 31 // 최대 클라이언트 수 (실제론 -1명)

#define LoadDynamicMesh(PATH, NAME, COM_NAME, SCENE)					\
pComponent = Engine::CDynamicMesh::Create(m_pGraphicDev, PATH, NAME);   \
if (nullptr == pComponent)												\
return E_FAIL;															\
m_pComponentMgr->Add_Component(SCENE, COM_NAME, pComponent);	

#define LoadStaticMesh(PATH, NAME, COM_NAME, SCENE)						\
pComponent = Engine::CStaticMesh::Create(m_pGraphicDev, PATH, NAME);    \
if (nullptr == pComponent)												\
return E_FAIL;															\
m_pComponentMgr->Add_Component(SCENE, COM_NAME, pComponent);		

#define LoadTexture(PATH, COM_NAME, TYPE, NUM, SCENE)									 \
pComponent = Engine::CTexture::Create(m_pGraphicDev, PATH, Engine::CTexture::TYPE, NUM); \
if (nullptr == pComponent)																 \
return E_FAIL;																			 \
m_pComponentMgr->Add_Component(SCENE, COM_NAME, pComponent);							 \

#define AddCollisionSphere(POINTER, SCENEID, CLONE_KEY, COMTYPE, KEY, RADIUS, BONE_MATRIX, SCALE)			\
pComponent = POINTER = ((Engine::CCollisionSphere*)m_pComponentMgr->Clone_Component(SCENEID, CLONE_KEY));	\
if (nullptr == pComponent)																					\
return E_FAIL;																								\
m_mapComponent[COMTYPE].insert(MAPCOMPONENT::value_type(KEY, pComponent));									\
POINTER->Create_Sphere(RADIUS, BONE_MATRIX, SCALE);															\
POINTER->Add_Ref();	

#define AddComponent(POINTER, CLASS, SCENEID, CLONE_KEY, COMTYPE, KEY)					   \
pComponent = POINTER = ((CLASS)m_pComponentMgr->Clone_Component(SCENEID, CLONE_KEY));	   \
if (nullptr == pComponent)																   \
return E_FAIL;																			   \
m_mapComponent[COMTYPE].insert(MAPCOMPONENT::value_type(KEY, pComponent));				   \
POINTER->Add_Ref();																		   

#define AddGameObject(CLASS, SCENEID, LAYERID)										\
pGameObject = CLASS::Create(m_pGraphicDev);											\
if (nullptr == pGameObject)															\
return E_FAIL;																		\
if (FAILED(m_pObjectMgr->Add_GameObject(SCENEID, LAYERID, pGameObject)))			\
return E_FAIL;																		\

#define AddCamera(CLASS, CAMTYPE)									\
pGameObject = CLASS::Create(m_pGraphicDev);							\
if (nullptr == pGameObject)											\
return E_FAIL;														\
m_pCameraMgr->Add_Camera(pGameObject, CAMTYPE);						\

#define Randomize() srand((unsigned)time(NULL))		// 난수 발생기 초기화
#define Random(n) (rand() % (n))					//0~n까지의 난수 발생
#define RandFloat ((FLOAT)rand()/(FLOAT)RAND_MAX)		

#define GRAVITY 9.8f

#define ATTACK_HURT_CHECK m_pPlayerInfo->eMainState != PLAYER_ATTACK && m_pPlayerInfo->eMainState != PLAYER_ATTACK_SECOND && m_pPlayerInfo->eMainState != PLAYER_ATTACK_THIRD && m_pPlayerInfo->eMainState != PLAYER_ATTACK_FOURTH && m_pPlayerInfo->eMainState != PLAYER_HURT	\
&& m_pPlayerInfo->eMainState != PLAYER_JUMP_ATTACK && m_pPlayerInfo->eMainState != PLAYER_JUMP_ATTACK_SECOND && m_pPlayerInfo->eMainState != PLAYER_JUMP_ATTACK_THIRD && m_pPlayerInfo->eMainState != PLAYER_JUMP_ATTACK_FOURTH && m_pPlayerInfo->eMainState != PLAYER_JUMP_FALLING_ATTACK \
&& m_pPlayerInfo->eMainState != PLAYER_JUMP_FALLING_ATTACK_SECOND && m_pPlayerInfo->eMainState != PLAYER_JUMP_FALLING_ATTACK_THIRD && m_pPlayerInfo->eMainState != PLAYER_JUMP_FALLING_ATTACK_FOURTH && m_pPlayerInfo->eMainState != PLAYER_SKILL && m_pPlayerInfo->eMainState != PLAYER_SKILL_ATTACK

#define ATTACK_CHECK m_pPlayerInfo->eMainState != PLAYER_ATTACK && m_pPlayerInfo->eMainState != PLAYER_ATTACK_SECOND && m_pPlayerInfo->eMainState != PLAYER_ATTACK_THIRD	\
&& m_pPlayerInfo->eMainState != PLAYER_JUMP_ATTACK && m_pPlayerInfo->eMainState != PLAYER_JUMP_ATTACK_SECOND && m_pPlayerInfo->eMainState != PLAYER_JUMP_ATTACK_THIRD && m_pPlayerInfo->eMainState != PLAYER_JUMP_FALLING_ATTACK \
&& m_pPlayerInfo->eMainState != PLAYER_JUMP_FALLING_ATTACK_SECOND && m_pPlayerInfo->eMainState != PLAYER_JUMP_FALLING_ATTACK_THIRD && m_pPlayerInfo->eMainState != PLAYER_SKILL && m_pPlayerInfo->eMainState != PLAYER_SKILL_ATTACK

#define JUMP_CHECK m_pPlayerInfo->eMainState != PLAYER_JUMP && m_pPlayerInfo->eMainState != PLAYER_JUMP_ATTACK && m_pPlayerInfo->eMainState != PLAYER_JUMP_ATTACK_SECOND && m_pPlayerInfo->eMainState != PLAYER_JUMP_ATTACK_THIRD \
 && m_pPlayerInfo->eMainState != PLAYER_JUMP_FALLING && m_pPlayerInfo->eMainState != PLAYER_JUMP_FALLING_ATTACK && m_pPlayerInfo->eMainState != PLAYER_JUMP_FALLING_ATTACK_SECOND && m_pPlayerInfo->eMainState != PLAYER_JUMP_FALLING_ATTACK_THIRD && m_pPlayerInfo->eMainState != PLAYER_SKILL	\
 && m_pPlayerInfo->eMainState != PLAYER_SKILL_ATTACK

#define  JUMP_STATE_CHECK PLAYER_JUMP == m_pPlayerInfo->eMainState || PLAYER_JUMP_FALLING == m_pPlayerInfo->eMainState \
|| PLAYER_JUMP_ATTACK == m_pPlayerInfo->eMainState || PLAYER_JUMP_ATTACK_SECOND == m_pPlayerInfo->eMainState || PLAYER_JUMP_ATTACK_THIRD == m_pPlayerInfo->eMainState	\
|| PLAYER_JUMP_FALLING_ATTACK == m_pPlayerInfo->eMainState || PLAYER_JUMP_FALLING_ATTACK_SECOND == m_pPlayerInfo->eMainState || PLAYER_JUMP_FALLING_ATTACK_THIRD == m_pPlayerInfo->eMainState

#endif