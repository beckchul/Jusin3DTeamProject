#include "stdafx.h"
#include "Loading.h"
#include "Component_Manager.h"
#include "Management.h"
#include "tinyxml2.h"
#include "DataManager.h"
#include "EffectMgr.h"

// 이펙트 로드를 위해 . .
// 2D_Effect
#include "Effect_Particle.h"
#include "Effect_RectParticle.h"
#include "Effect_Frame.h"
#include "Effect_Single.h"
#include "Effect_Single_Fog.h"
#include "Effect_Decal.h"
// Effect_Mesh
#include "Effect_Mesh_Shield.h"
#include "Effect_Mesh_Bolt.h"
#include "Effect_Mesh_ElectricGroup1.h"
#include "Effect_Mesh_ElectricGroup2.h"
#include "Effect_Mesh_LightningTree.h"
#include "Effect_Mesh_Wave.h"
#include "Effect_Mesh_AuraSphere.h"
#include "Effect_Mesh_Spiral.h"
#include "Effect_Mesh_ManaBomb.h"
#include "Effect_Mesh_SplosionRings.h"
#include "Effect_Mesh_Swirly.h"
#include "Effect_Mesh_GlowBall.h"
#include "Effect_Mesh_Projectile1.h"
#include "Effect_Mesh_SkyBeam.h"

USING(Engine)
using namespace tinyxml2;

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGTYPE eLoadID)
	: m_pGraphicDev(pGraphicDev)
	, m_bComplete(false)
	, m_eLoadType(eLoadID)
	, m_pComponentMgr(Engine::CComponent_Manager::GetInstance())
	, m_pEffectMgr(Engine::CEffectMgr::GetInstance())
{
	m_pEffectMgr->Add_Ref();
	m_pComponentMgr->Add_Ref();
	m_pGraphicDev->AddRef();
}

CLoading::~CLoading()
{
}

HRESULT CLoading::Loading_Function()
{
	HRESULT		hr = 0;

	switch (m_eLoadType)
	{
	case CLoading::LOADING_LOGO:
		StaticLoading();
		Engine::CManagement::GetInstance()->Ready_Management(SCENE_STATIC, L"Com_Renderer");
		LogoLoading();
		break;

	case CLoading::LOADING_MENU:
#ifdef Demonstration
		StaticLoading();
		Engine::CManagement::GetInstance()->Ready_Management(SCENE_STATIC, L"Com_Renderer");
#endif
		MenuLoading();
		break;

	case CLoading::LOADING_STAGE1:
		Stage1Loading();
		break;

	case CLoading::LOADING_STAGE2:
#ifdef Lastman_Client
		StaticLoading();
		Engine::CManagement::GetInstance()->Ready_Management(SCENE_STATIC, L"Com_Renderer");
#endif
		Stage2Loading();
		break;

	case CLoading::LOADING_LOBBY:
		LobbyLoading();
		break;

		// TestScene
	case CLoading::LOADING_TESTSCENE_SEUNGHO:
		SeungHoLoading();
		break;
	case CLoading::LOADING_TESTSCENE_SEOCHAN:
		SeoChanLoading();
		break;
	case CLoading::LOADING_TESTSCENE_BACKCHUL:
		BackChulLoading();
		break;
	case CLoading::LOADING_TESTSCENE_MINKYEONG:
		MinKyeongLoading();
		break;

	case CLoading::LOADING_TESTSCENE_SEUNGHO2:
		//SeungHoLoading2();
		break;
	case CLoading::LOADING_TESTSCENE_SEOCHAN2:
		SeoChanLoading2();
		break;
	case CLoading::LOADING_TESTSCENE_BACKCHUL2:
		//BackChulLoading2();
		break;
	case CLoading::LOADING_TESTSCENE_MINKYEONG2:
		//MinKyeongLoading2();
		break;
	}

	if (FAILED(hr))
	{
		MSG_BOX(L"Loading Function call Failed");
		return 0;
	}

	return 0;
}

HRESULT CLoading::Ready_Loading(void)
{
#ifdef Demonstration
	std::packaged_task<void()> task(bind(&CLoading::Loading_Function, this));
	thread temp(move(task));
	temp.detach();
#else
	Loading_Function();
#endif

	return S_OK;
}

HRESULT CLoading::LogoLoading(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// ===== For.Texture ======================================================================================================================================================
	// For.BackGround
	pComponent = Engine::CTexture::Create(m_pGraphicDev, L"../Resources/Texture/Logo/Back/%d.tga", Engine::CTexture::TYPE_NORMAL, 18);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_LOGO, L"Com_Texture_Back", pComponent)))
		return E_FAIL;

	pComponent = Engine::CTexture::Create(m_pGraphicDev, L"../Resources/Texture/Logo/%d.tga", Engine::CTexture::TYPE_NORMAL, 1);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_LOGO, L"Com_Texture_Select", pComponent)))
		return E_FAIL;

	m_bComplete = true;

	return NOERROR;
}

HRESULT CLoading::StaticLoading(void)
{
	Engine::CComponent*			pComponent = nullptr;

	Randomize();		// 시드값 설정

						// For.Utilty 
						// For.Renderer
	pComponent = Engine::CRenderer::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Renderer", pComponent)))
		return E_FAIL;

	// For.Transform
	pComponent = Engine::CTransform::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Transform", pComponent)))
		return E_FAIL;

	// For.Buffer 
	// For.Rectangle_Texture Buffer
	pComponent = Engine::CRect_Texture::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Buffer_RcTex", pComponent)))
		return E_FAIL;

	// For.Cube
	pComponent = Engine::CCube_Texture::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Buffer_Cube", pComponent);

	//// For.CubeColor
	pComponent = Engine::CCubeColor::Create(m_pGraphicDev, &_vec3(0.f, 0.f, 0.f), &_vec3(1.f, 1.f, 1.f), D3DCOLOR_ARGB(255, 0, 255, 0));
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Buffer_CubeColor", pComponent)))
		return E_FAIL;

	// For.View_Texture
	pComponent = Engine::CView_Texture::Create(m_pGraphicDev, &_vec4(0.f, 0.f, 0.f, 0.f));
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Buffer_ViewTexture", pComponent);

	// For.Terrain
	pComponent = Engine::CTerrain_Texture::Create(m_pGraphicDev, 129, 129, 1);
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Buffer_Terrain", pComponent);

	// For.Trail_Texture
	pComponent = Engine::CTrail_Texture::Create(m_pGraphicDev);
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Buffer_TrailTexture", pComponent);

	// For.Shader
	pComponent = Engine::CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_BackLogo.hpp");
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Shader_Back", pComponent)))
		return E_FAIL;

	pComponent = Engine::CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_UI.hpp");
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Shader_UI", pComponent)))
		return E_FAIL;

	pComponent = Engine::CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_Effect.hpp");
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Shader_Effect", pComponent)))
		return E_FAIL;

	pComponent = Engine::CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_MeshEffect.hpp");
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Shader_MeshEffect", pComponent)))
		return E_FAIL;

	pComponent = Engine::CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_TrailEffect.hpp");
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Shader_TrailEffect", pComponent)))
		return E_FAIL;

	/*pComponent = Engine::CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_Terrain.hpp");
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Shader_Terrain", pComponent)))
	return E_FAIL;*/

	pComponent = Engine::CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_StaticMesh.hpp");
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Shader_StaticMesh", pComponent)))
		return E_FAIL;

	pComponent = Engine::CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_Sky.hpp");
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Shader_SkyBox", pComponent)))
		return E_FAIL;

	//// == 하드웨어 스키닝
	pComponent = Engine::CShader::Create(m_pGraphicDev, L"../Resources/Shaders/Shader_DynamicMesh.hpp");
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_Shader_DynamicMesh", pComponent)))
		return E_FAIL;

	//////////// CollisionSphere
	// Monster
	pComponent = Engine::CCollisionSphere::Create(m_pGraphicDev);
	if (FAILED(m_pComponentMgr->Add_Component(SCENE_STATIC, L"Com_CollisionSphere", pComponent)))
		return E_FAIL;

	// DynamicMesh
	Load_DynamicMesh();

	// StaticMesh
	Load_StaticMesh();

	// UI
	Load_UI();

	//Effect
	Load_Effect();

	// Effect_List
	Load_EffectList();
	return NOERROR;
}

HRESULT CLoading::MenuLoading(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// 메뉴
	LoadTexture(L"../Resources/Texture/UI/Text/PressStart/PressStart.png", L"Com_Texture_PressStart", TYPE_NORMAL, 1, SCENE_MENU);

	//// Map
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Menu/Map/", L"MapLast.x", L"Com_Mesh_MenuMap", SCENE_MENU);

	// Title
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Menu/Title/", L"Title.x", L"Com_Mesh_Title", SCENE_MENU);


	m_bComplete = true;
	return S_OK;
}

HRESULT CLoading::Stage1Loading(void)
{
	// 스테이에 필요한 컴포넌트를 생성하자.
	Engine::CComponent*			pComponent = nullptr;

	// --- Mesh ---

	// - Dynamic Mesh -

	// ETC
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Door/LeftDoor/", L"LeftDoor.x", L"Com_Mesh_LeftDoor", SCENE_STAGE);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Door/RightDoor/", L"RightDoor.x", L"Com_Mesh_RightDoor", SCENE_STAGE);

	// - Static Mesh -

	// Map
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/MagusQuarters/", L"MagusQuarters.x", L"Com_Mesh_Map", SCENE_STAGE);

	// Static Map Object
	vector<OBJECT_NAME*> vecObjName = CDataManager::GetInstance()->Get_ObjName();

	for (_uint i = 0; i < vecObjName.size(); ++i)
	{
		LoadStaticMesh(vecObjName[i]->szObjectPath,
			vecObjName[i]->szObjectX, vecObjName[i]->szObjectName, SCENE_STAGE);
	}

	LoadTexture(L"../Resources/Texture/Effect/Frame/MyEffect/%d.jpg", L"Com_Texture_MyEffect", TYPE_NORMAL, 64, SCENE_STAGE); // MyEffect(임시)

	m_bComplete = true;

	return NOERROR;
}

HRESULT CLoading::Stage2Loading(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// Static Map Object
	vector<OBJECT_NAME*> vecObjName = CDataManager::GetInstance()->Get_ObjName2();

	for (_uint i = 0; i < vecObjName.size(); ++i)
	{
		LoadStaticMesh(vecObjName[i]->szObjectPath,
			vecObjName[i]->szObjectX, vecObjName[i]->szObjectName, SCENE_STAGE2);
	}

	// SkyBoxBall
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/TheKingsGame/sky_Regular_SM/", L"sky_Regular_SM.x", L"Com_Mesh_SkyBoxBall", SCENE_STAGE2);
	// SkyBox
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/TheKingsGame/Jester_Skybox/", L"Jester_Skybox.x", L"Com_Mesh_SkyBox", SCENE_STAGE2);

	m_bComplete = true;

	return NOERROR;
}

HRESULT CLoading::LobbyLoading(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// Static Map Object
	vector<OBJECT_NAME*> vecObjName = CDataManager::GetInstance()->Get_ObjName3();

	for (_uint i = 0; i < vecObjName.size(); ++i)
	{
		LoadStaticMesh(vecObjName[i]->szObjectPath,
			vecObjName[i]->szObjectX, vecObjName[i]->szObjectName, SCENE_LOBBY);
	}

	m_bComplete = true;

	return NOERROR;
}

HRESULT CLoading::SeungHoLoading(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// Map
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/MagusQuarters/", L"MagusQuarters.x", L"Com_Mesh_Map", SCENE_STAGE);

	m_bComplete = true;

	return NOERROR;
}

HRESULT CLoading::SeoChanLoading(void)
{
	// 스테이에 필요한 컴포넌트를 생성하자.
	Engine::CComponent*			pComponent = nullptr;

	// --- Mesh ---

	// - Dynamic Mesh -

	//// Player
	//LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Mage/", L"Mage.x", L"Com_Mesh_Mage", SCENE_STAGE);
	//LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Monk/", L"Monk.x", L"Com_Mesh_Monk", SCENE_STAGE);
	//LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Huntress/", L"Huntress.x", L"Com_Mesh_Huntress", SCENE_STAGE);

	//// Tower
	//LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/FireTower/", L"FireTower.x", L"Com_Mesh_FireTower", SCENE_STAGE);

	////Monster
	//LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Goblin/", L"Goblin.x", L"Com_Mesh_Goblin", SCENE_STAGE);
	//LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/OrcBruiser/", L"OrcBruiser.x", L"Com_Mesh_OrcBruiser", SCENE_STAGE);
	//LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Kobold/", L"Kobold.x", L"Com_Mesh_Kobold", SCENE_STAGE);
	//LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/DarkElfArcher/", L"DarkElfArcher.x", L"Com_Mesh_DarkElfArcher", SCENE_STAGE);

	// ETC
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Door/LeftDoor/", L"LeftDoor.x", L"Com_Mesh_LeftDoor", SCENE_STAGE);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Door/RightDoor/", L"RightDoor.x", L"Com_Mesh_RightDoor", SCENE_STAGE);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/TresureChest/", L"TresureChest.x", L"Com_Mesh_TresureChest", SCENE_STAGE);

	// - Static Mesh -

	//// Weapon
	//LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Staff/", L"Staff.x", L"Com_Mesh_Staff", SCENE_STAGE);
	//LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Spear/", L"Spear.x", L"Com_Mesh_Spear", SCENE_STAGE);
	//LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Arrow/", L"Arrow.x", L"Com_Mesh_Arrow", SCENE_STAGE);
	//LoadStaticMesh(L"../Resources/Mesh/StaticMesh/CrossBow/", L"CrossBow.x", L"Com_Mesh_CrossBow", SCENE_STAGE);
	//LoadStaticMesh(L"../Resources/Mesh/StaticMesh/CrossBowArrow/", L"CrossBowArrow.x", L"Com_Mesh_CrossBowArrow", SCENE_STAGE);


	// Crystal
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Crystal/", L"Crystal.x", L"Com_Mesh_Crystal", SCENE_STAGE);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Crystal/Accessory1/", L"Accessory1.x", L"Com_Mesh_Accessory1", SCENE_STAGE);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Crystal/Accessory2/", L"Accessory2.x", L"Com_Mesh_Accessory2", SCENE_STAGE);

	//// MeshEffect
	//LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/Shield/", L"Shield.x", L"Com_Mesh_Shield", SCENE_STAGE);
	//LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/Bolt/", L"Bolt.x", L"Com_Mesh_Bolt", SCENE_STAGE);
	//LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/ElectricityGroup01/", L"ElectricityGroup01.x", L"Com_Mesh_ElectricityGroup01", SCENE_STAGE);
	//LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/ElectricityGroup02/", L"ElectricityGroup02.x", L"Com_Mesh_ElectricityGroup02", SCENE_STAGE);
	//LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/LightningTreeShape/", L"LightningTreeShape.x", L"Com_Mesh_LightningTree", SCENE_STAGE);
	//LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/Wave/", L"Wave.x", L"Com_Mesh_Wave", SCENE_STAGE);
	//LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/ManaBomb/", L"ManaBomb.x", L"Com_Mesh_ManaBomb", SCENE_STAGE);
	//LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/AuraSphere/", L"AuraSphere.x", L"Com_Mesh_AuraSphere", SCENE_STAGE);
	//LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/AuraSphere/AuraSphere_Alt/", L"AuraSphere_Alt.x", L"Com_Mesh_AuraSphere_Alt", SCENE_STAGE);
	//LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/Spiral/", L"Spiral.x", L"Com_Mesh_Spiral", SCENE_STAGE);



	// Map
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/MagusQuarters/", L"MagusQuarters.x", L"Com_Mesh_Map", SCENE_STAGE);

	// Static Map Object
	vector<OBJECT_NAME*> vecObjName = CDataManager::GetInstance()->Get_ObjName();

	for (_uint i = 0; i < vecObjName.size(); ++i)
	{
		LoadStaticMesh(vecObjName[i]->szObjectPath,
			vecObjName[i]->szObjectX, vecObjName[i]->szObjectName, SCENE_STAGE);
	}

	// --- Texture ---

	//// - Effect -

	//LoadTexture(L"../Resources/Texture/Effect/Explode/%d.png", L"Com_Texture_Explode", TYPE_NORMAL, 4, SCENE_STAGE);
	//LoadTexture(L"../Resources/Texture/Effect/Particle/Particle_%d.png", L"Com_Texture_Particle", TYPE_NORMAL, 21, SCENE_STAGE);
	//LoadTexture(L"../Resources/Texture/Effect/Frame/CandleLight/CandleLight_%d.png", L"Com_Texture_CandleLight", TYPE_NORMAL, 16, SCENE_STAGE);
	//LoadTexture(L"../Resources/Texture/Effect/Frame/Flame1/Flame1_%d.png", L"Com_Texture_Flame1", TYPE_NORMAL, 16, SCENE_STAGE);
	//LoadTexture(L"../Resources/Texture/Effect/Frame/Flame2/Flame2_%d.png", L"Com_Texture_Flame2", TYPE_NORMAL, 16, SCENE_STAGE);
	//LoadTexture(L"../Resources/Texture/Effect/Trail/Trail_%d.tga", L"Com_Texture_Trail", TYPE_NORMAL, 1, SCENE_STAGE); // Trail

	//																												   // SingleEffect
	//LoadTexture(L"../Resources/Texture/Effect/Single/BS/staffFlare_%d.tga", L"Com_Texture_Flare", TYPE_NORMAL, 1, SCENE_STAGE);
	//LoadTexture(L"../Resources/Texture/Effect/Single/BS/stars_%d.tga", L"Com_Texture_stars", TYPE_NORMAL, 1, SCENE_STAGE);
	//LoadTexture(L"../Resources/Texture/Effect/Single/BS/Specular_%d.tga", L"Com_Texture_EffectSpecular", TYPE_NORMAL, 1, SCENE_STAGE);


	//LoadTexture(L"../Resources/Texture/Effect/Frame/MyEffect/%d.jpg", L"Com_Texture_MyEffect", TYPE_NORMAL, 64, SCENE_STAGE); // MyEffect(임시)

	// SkyBox
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/TheKingsGame/sky_Regular_SM/", L"sky_Regular_SM.x", L"Com_Mesh_SkyBoxBall", SCENE_STAGE2);

	m_bComplete = true;

	return NOERROR;
}

HRESULT CLoading::BackChulLoading(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// For.Mesh	=================================================================================================================================

	// Mesh_Dynamic ================================
	// Mage
	//LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Mage/", L"Mage.x", L"Com_Mesh_Mage", SCENE_STAGE);

	// Monk
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Monk/", L"Monk.x", L"Com_Mesh_Monk", SCENE_STAGE);

	// Huntress
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Huntress/", L"Huntress.x", L"Com_Mesh_Huntress", SCENE_STAGE);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/CrossBow/", L"CrossBow.x", L"Com_Mesh_CrossBow", SCENE_STAGE);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/CrossBowArrow/", L"CrossBowArrow.x", L"Com_Mesh_CrossBowArrow", SCENE_STAGE);

	//// Goblin
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Goblin/", L"Goblin.x", L"Com_Mesh_Goblin", SCENE_STAGE);

	//// OrcBruiser
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/OrcBruiser/", L"OrcBruiser.x", L"Com_Mesh_OrcBruiser", SCENE_STAGE);

	// Kobold
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Kobold/", L"Kobold.x", L"Com_Mesh_Kobold", SCENE_STAGE);

	// 9.20~ //===============
	// DarkElfArcher
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/DarkElfArcher/", L"DarkElfArcher.x", L"Com_Mesh_DarkElfArcher", SCENE_STAGE);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Arrow/", L"Arrow.x", L"Com_Mesh_Arrow", SCENE_STAGE);

	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/FireTower/", L"FireTower.x", L"Com_Mesh_FireTower", SCENE_STAGE);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/MissleTower/", L"MissleTower.x", L"Com_Mesh_MissileTower", SCENE_STAGE);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/StrikerTower/", L"StrikerTower.x", L"Com_Mesh_StrikerTower", SCENE_STAGE);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/LightningTower/", L"LightningTower.x", L"Com_Mesh_LightningTower", SCENE_STAGE);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/MageBlockade/", L"MageBlockade.x", L"Com_Mesh_Blockade", SCENE_STAGE);

	// =======================

	// Mesh_Static =================================
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Staff/", L"Staff.x", L"Com_Mesh_Staff", SCENE_STAGE);

	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Spear/", L"Spear.x", L"Com_Mesh_Spear", SCENE_STAGE);


	// Map ======================================================================================================================================
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/MagusQuarters/", L"MagusQuarters.x", L"Com_Mesh_Map", SCENE_STAGE);


	// ETC
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Door/LeftDoor/", L"LeftDoor.x", L"Com_Mesh_LeftDoor", SCENE_STAGE);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Door/RightDoor/", L"RightDoor.x", L"Com_Mesh_RightDoor", SCENE_STAGE);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/TresureChest/", L"TresureChest.x", L"Com_Mesh_TresureChest", SCENE_STAGE);

	// Crystal
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Crystal/", L"Crystal.x", L"Com_Mesh_Crystal", SCENE_STAGE);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Crystal/Accessory1/", L"Accessory1.x", L"Com_Mesh_Accessory1", SCENE_STAGE);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Crystal/Accessory2/", L"Accessory2.x", L"Com_Mesh_Accessory2", SCENE_STAGE);

	// Static Map Object
	vector<OBJECT_NAME*> vecObjName = CDataManager::GetInstance()->Get_ObjName();

	for (_uint i = 0; i < vecObjName.size(); ++i)
	{
		LoadStaticMesh(vecObjName[i]->szObjectPath,
			vecObjName[i]->szObjectX, vecObjName[i]->szObjectName, SCENE_STAGE);
	}

	m_bComplete = true;

	return NOERROR;
}

HRESULT CLoading::MinKyeongLoading(void)
{
	Engine::CComponent*			pComponent = nullptr;


	// ETC
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Door/LeftDoor/", L"LeftDoor.x", L"Com_Mesh_LeftDoor", SCENE_STAGE);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Door/RightDoor/", L"RightDoor.x", L"Com_Mesh_RightDoor", SCENE_STAGE);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/TresureChest/", L"TresureChest.x", L"Com_Mesh_TresureChest", SCENE_STAGE);

	// Map
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/MagusQuarters/", L"MagusQuarters.x", L"Com_Mesh_Map", SCENE_STAGE);

	// Static Map Object
	vector<OBJECT_NAME*> vecObjName = CDataManager::GetInstance()->Get_ObjName();

	for (_uint i = 0; i < vecObjName.size(); ++i)
	{
		LoadStaticMesh(vecObjName[i]->szObjectPath,
			vecObjName[i]->szObjectX, vecObjName[i]->szObjectName, SCENE_STAGE);
	}


	// Crystal
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Crystal/", L"Crystal.x", L"Com_Mesh_Crystal", SCENE_STAGE);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Crystal/Accessory1/", L"Accessory1.x", L"Com_Mesh_Accessory1", SCENE_STAGE);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Crystal/Accessory2/", L"Accessory2.x", L"Com_Mesh_Accessory2", SCENE_STAGE);

	// Tower
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/FireTower/", L"FireTower.x", L"Com_Mesh_FireTower", SCENE_STAGE);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/MissleTower/", L"MissleTower.x", L"Com_Mesh_MissileTower", SCENE_STAGE);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/StrikerTower/", L"StrikerTower.x", L"Com_Mesh_StrikerTower", SCENE_STAGE);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/LightningTower/", L"LightningTower.x", L"Com_Mesh_LightningTower", SCENE_STAGE);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/MageBlockade/", L"MageBlockade.x", L"Com_Mesh_Blockade", SCENE_STAGE);

	// 플레이어-------------------------------------------
	// Monk
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Monk/", L"Monk.x", L"Com_Mesh_Monk", SCENE_STAGE);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Spear/", L"Spear.x", L"Com_Mesh_Spear", SCENE_STAGE);


	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Mage/", L"Mage.x", L"Com_Mesh_Mage", SCENE_STAGE);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Staff/", L"Staff.x", L"Com_Mesh_Staff", SCENE_STATIC);
	// -------------------------------------------
	//Monster
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Goblin/", L"Goblin.x", L"Com_Mesh_Goblin", SCENE_STAGE);
	m_bComplete = true;

	return NOERROR;
}

HRESULT CLoading::SeoChanLoading2(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// Static Map Object
	vector<OBJECT_NAME*> vecObjName = CDataManager::GetInstance()->Get_ObjName2();

	for (_uint i = 0; i < vecObjName.size(); ++i)
	{
		LoadStaticMesh(vecObjName[i]->szObjectPath,
			vecObjName[i]->szObjectX, vecObjName[i]->szObjectName, SCENE_STAGE2);
	}

	// SkyBoxBall
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/TheKingsGame/sky_Regular_SM/", L"sky_Regular_SM.x", L"Com_Mesh_SkyBoxBall", SCENE_STAGE2);

	// SkyBox
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/TheKingsGame/Jester_Skybox/", L"Jester_Skybox.x", L"Com_Mesh_SkyBox", SCENE_STAGE2);

	m_bComplete = true;

	return NOERROR;
}

HRESULT CLoading::Load_DynamicMesh(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// Player
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Mage/", L"Mage.x", L"Com_Mesh_Mage", SCENE_STATIC);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Monk/", L"Monk.x", L"Com_Mesh_Monk", SCENE_STATIC);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Huntress/", L"Huntress.x", L"Com_Mesh_Huntress", SCENE_STATIC);

	// Tower
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/FireTower/", L"FireTower.x", L"Com_Mesh_FireTower", SCENE_STATIC);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/MissleTower/", L"MissleTower.x", L"Com_Mesh_MissileTower", SCENE_STATIC);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/StrikerTower/", L"StrikerTower.x", L"Com_Mesh_StrikerTower", SCENE_STATIC);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/LightningTower/", L"LightningTower.x", L"Com_Mesh_LightningTower", SCENE_STATIC);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/MageBlockade/", L"MageBlockade.x", L"Com_Mesh_Blockade", SCENE_STATIC);

	//Monster
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Goblin/", L"Goblin.x", L"Com_Mesh_Goblin", SCENE_STATIC);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/OrcBruiser/", L"OrcBruiser.x", L"Com_Mesh_OrcBruiser", SCENE_STATIC);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Kobold/", L"Kobold.x", L"Com_Mesh_Kobold", SCENE_STATIC);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/DarkElfArcher/", L"DarkElfArcher.x", L"Com_Mesh_DarkElfArcher", SCENE_STATIC);
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/Demon/", L"Demon.x", L"Com_Mesh_Demon", SCENE_STATIC);

	// TresureBox
	LoadDynamicMesh(L"../Resources/Mesh/DynamicMesh/TresureChest/", L"TresureChest.x", L"Com_Mesh_TresureChest", SCENE_STATIC);
	return NOERROR;
}

HRESULT CLoading::Load_StaticMesh(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// Weapon
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Staff/", L"Staff.x", L"Com_Mesh_Staff", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Spear/", L"Spear.x", L"Com_Mesh_Spear", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Arrow/", L"Arrow.x", L"Com_Mesh_Arrow", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/CrossBow/", L"CrossBow.x", L"Com_Mesh_CrossBow", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/CrossBowArrow/", L"CrossBowArrow.x", L"Com_Mesh_CrossBowArrow", SCENE_STATIC);

	// Crystal
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Crystal/", L"Crystal.x", L"Com_Mesh_Crystal", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Crystal/Accessory1/", L"Accessory1.x", L"Com_Mesh_Accessory1", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Crystal/Accessory2/", L"Accessory2.x", L"Com_Mesh_Accessory2", SCENE_STATIC);

	// Present
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Present/", L"Present.x", L"Com_Mesh_Present", SCENE_STATIC);

	// ManaToken
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/ManaToken/", L"ManaToken.x", L"Com_Mesh_ManaToken", SCENE_STATIC);

	// EasterEgg
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/EasterEgg/", L"EasterEgg.x", L"Com_Mesh_EasterEgg", SCENE_STATIC);

	// MeshEffect
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/Shield/", L"Shield.x", L"Com_Mesh_Shield", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/Bolt/", L"Bolt.x", L"Com_Mesh_Bolt", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/ElectricityGroup01/", L"ElectricityGroup01.x", L"Com_Mesh_ElectricityGroup01", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/ElectricityGroup02/", L"ElectricityGroup02.x", L"Com_Mesh_ElectricityGroup02", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/LightningTreeShape/", L"LightningTreeShape.x", L"Com_Mesh_LightningTree", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/Wave/", L"Wave.x", L"Com_Mesh_Wave", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/ManaBomb/", L"ManaBomb.x", L"Com_Mesh_ManaBomb", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/AuraSphere/", L"AuraSphere.x", L"Com_Mesh_AuraSphere", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/AuraSphere/AuraSphere_Alt/", L"AuraSphere_Alt.x", L"Com_Mesh_AuraSphere_Alt", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/Spiral/", L"Spiral.x", L"Com_Mesh_Spiral", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/EnergyBall/", L"EnergyBall.x", L"Com_Mesh_EnergyBall", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/ChickenSplosionRings/", L"ChickenSplosionRings.x", L"Com_Mesh_SplosionRings", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/Swirly/", L"Swirly.x", L"Com_Mesh_Swirly", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/GlowBall/", L"GlowBall.x", L"Com_Mesh_GlowBall", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/SkyBeam/", L"SkyBeam.x", L"Com_Mesh_SkyBeam", SCENE_STATIC);

	//발사체..
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/Head_Projectile/", L"Head_Projectile.x", L"Com_Mesh_Head_Projectile", SCENE_STATIC);
	LoadStaticMesh(L"../Resources/Mesh/StaticMesh/Effect/Projectile1/", L"Projectile1.x", L"Com_Mesh_Projectile1", SCENE_STATIC);

	return NOERROR;
}

HRESULT CLoading::Load_Effect(void)
{
	Engine::CComponent*			pComponent = nullptr;

	// --- Texture ---
	// MeshEffect_MaskMap
	LoadTexture(L"../Resources/Mesh/StaticMesh/Effect/Shield/water_mask_%d.tga", L"Com_Texture_ShieldMask", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Mesh/StaticMesh/Effect/EnergyBall/energyCaustics_m%d.tga", L"Com_Texture_EnergyBall_Mask", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Mesh/StaticMesh/Effect/EnergyBall/energyCaustics_n%d.tga", L"Com_Texture_EnergyBall_Normal", TYPE_NORMAL, 1, SCENE_STATIC);

	// Frame_Effect
	LoadTexture(L"../Resources/Texture/Effect/Particle/Particle_%d.png", L"Com_Texture_Particle", TYPE_NORMAL, 24, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Frame/CandleLight/CandleLight_%d.png", L"Com_Texture_CandleLight", TYPE_NORMAL, 16, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Frame/Flame1/Flame1_%d.png", L"Com_Texture_Flame1", TYPE_NORMAL, 16, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Frame/Flame2/Flame2_%d.png", L"Com_Texture_Flame2", TYPE_NORMAL, 16, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Frame/Blood1/Blood_%d.png", L"Com_Texture_Blood", TYPE_NORMAL, 19, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Trail/Trail_%d.tga", L"Com_Texture_Trail", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Frame/Explosion/explosionFlipbook_%d.png", L"Com_Texture_Explosion", TYPE_NORMAL, 16, SCENE_STATIC);

	// Single_Effect
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/staffFlare_%d.tga", L"Com_Texture_Flare", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/stars_%d.tga", L"Com_Texture_stars", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/Specular_%d.tga", L"Com_Texture_EffectSpecular", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/splatter_%d.tga", L"Com_Texture_splatter", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/flowBeam_%d.tga", L"Com_Texture_FlowBeam", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/radialStars_%d.tga", L"Com_Texture_RadialStar", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/BloodSet_%d.tga", L"Com_Texture_BloodSet", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/UnderFire_%d.tga", L"Com_Texture_UnderFire", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/projectile_%d.tga", L"Com_Texture_projectile", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/fogSheet_%d.tga", L"Com_Texture_FogSheet", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/FloatRing_%d.tga", L"Com_Texture_FloatRing", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/energySwirl_%d.tga", L"Com_Texture_EnergySwirl", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/Stars_Black_%d.png", L"Com_Texture_BlackStars", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/circleHighlight_%d.tga", L"Com_Texture_HighlightCircle", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/BloodSet_Blue%d.tga", L"Com_Texture_BloodSet_Blue", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/BloodSet_Green%d.tga", L"Com_Texture_BloodSet_Green", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/BloodSet_Red%d.tga", L"Com_Texture_BloodSet_Red", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/LongSpecular_%d.tga", L"Com_Texture_EffectLongSpecular", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/BlueMagicCircle_%d.tga", L"Com_Texture_BlueMagicCircle", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/PinkMagicCircle_%d.tga", L"Com_Texture_PinkMagicCircle", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/Merge_Redchannel_%d.tga", L"Com_Texture_MergeRed", TYPE_NORMAL, 1, SCENE_STATIC);

	// Fog_Mask
	LoadTexture(L"../Resources/Mesh/StaticMesh/Effect/Fog/ringsMerge_4_5_6_%d.tga", L"Com_Texture_FogMerge", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Mesh/StaticMesh/Effect/Fog/FogFader_%d.tga", L"Com_Texture_FogFader", TYPE_NORMAL, 1, SCENE_STATIC);

	// Decal Texture
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/mage_Slam_%d.tga", L"Com_Texture_Decal", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/SH_Circle_%d.tga", L"Com_Texture_Circle", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Single/BS/cloud_%d.tga", L"Com_Texture_Cloud", TYPE_NORMAL, 1, SCENE_STATIC);

	// Decal_Spray
	LoadTexture(L"../Resources/Texture/Effect/Decal_Spray/Spray_Well_Played%d.png", L"Com_Texture_Spray_WellPlayed", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Decal_Spray/Spray_Sorry%d.png", L"Com_Texture_Spray_Sorry", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Decal_Spray/Spray_Oops%d.png", L"Com_Texture_Spray_Oops", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Decal_Spray/Spray_Noose%d.png", L"Com_Texture_Spray_Noose", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Decal_Spray/Spray_Goodpepe%d.png", L"Com_Texture_Spray_GoodPepe", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Decal_Spray/Spray_pepe%d.png", L"Com_Texture_Spray_Pepe", TYPE_NORMAL, 1, SCENE_STATIC);

	// Decal_Backchul
	LoadTexture(L"../Resources/Texture/Effect/Decal_Spray/Backchul_In_KyeongBokPalace%d.png", L"Com_Texture_Backchul", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Decal_Spray/Backchul_Confidence%d.png", L"Com_Texture_BackchulConfidence", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Decal_Spray/Backchul_Feel_Pain%d.png", L"Com_Texture_BackchulPain", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Decal_Spray/Backchul_In_Karaoke%d.png", L"Com_Texture_BackchulKaraoke", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/Effect/Decal_Spray/Backchul_With_Turtle%d.png", L"Com_Texture_BackchulTurtle", TYPE_NORMAL, 1, SCENE_STATIC);

	// Decal_Something
	LoadTexture(L"../Resources/Texture/Effect/Decal_Spray/Burger_Puppy%d.png", L"Com_Texture_BurgerPuppy", TYPE_NORMAL, 1, SCENE_STATIC);

	return NOERROR;
}

HRESULT CLoading::Load_EffectList(void)
{
	//// 이펙트 데이터 초기화
	//m_pEffectMgr->Init_EffectData();

	// 이펙트 리스트들을 추가해주자.

	list<Engine::CEffect*> pEffectList;

	Engine::CEffect*   pEffect = nullptr;

	//========================================================
	//========================================================
	//========================================================
	//========================================================
	// Menu
	// ---------------------------------------Effect_MenuTorch---------------------------------------
	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Flame2", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 24.f, 16.f));
	pEffect->Set_EffectColor(_vec4(2.f, 0.8f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE
		, &_vec3(1.3f, 1.f, 1.f));
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_MenuTorch", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_MenuTorch_Blue---------------------------------------
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_UnderFire", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.5f, 0.5f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE,
		&_vec3(0.5f, 0.4f, 1.f));
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_MenuTorch_Blue", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Upside_FirePlace---------------------------------------
	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Flame2", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 24.f, 16.f));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(2.f, 0.8f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(1.8f, 2.8f, 1.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_FirePlace"))
		, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Upside_FirePlace", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Downside_FirePlace---------------------------------------
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_UnderFire", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.37f, 0.37f, 1.5f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE,
		&_vec3(1.f, 0.7f, 1.f));
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Downside_FirePlace", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Lobby_Candle---------------------------------------
	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_CandleLight", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 21.f, 16.f));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(2.f, 1.5f, 1.5f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(2.5f, 4.f, 1.f));
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Lobby_Candle", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	//========================================================
	//========================================================
	//========================================================
	//========================================================

	// ---------------------------------------Effect_SlowAura---------------------------------------
	//Particle
	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Shield")), SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_DetailPos(_vec3(0.f, 23.f, 0.f));   
	pEffectList.push_back(pEffect);

	// Wave
	pEffect = CEffect_Mesh_Wave::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(0.f, 1.f), 0.6f);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.45f, 0.45f, 0.45f));
	pEffect->Set_EffectColor(_vec4(0.3f, 0.8f, 0.3f, 0.7f));
	pEffect->Set_DetailPos(_vec3(0.f, 0.5f, 0.f));
	pEffectList.push_back(pEffect);

	// AuraMesh
	pEffect = CEffect_Mesh_Shield::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.4f, 0.4f, 0.4f));
	pEffect->Set_EffectColor(_vec4(0.3f, 0.8f, 0.3f, 0.7f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UV(_vec2(5.f, 5.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(0.25f, 0.f), 0.15f);

	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_SlowAura", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_In_SlowAura---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 2.5f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(6.f, 6.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(1.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(6.5f, 6.f, TRUE);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_SlowAura_Bubble")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_BubbleState(0.8f, 5.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_In_SlowAura", pEffectList);
	//--------------------------------------------------------------------------------------------
	pEffectList.clear();

	// ---------------------------------------Effect_LightingAura---------------------------------------
	//Particle
	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Lighting_Aura")), SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_DetailPos(_vec3(0.f, 23.f, 0.f));
	pEffectList.push_back(pEffect);

	// Wave
	pEffect = CEffect_Mesh_Wave::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(0.f, 1.f), 0.6f);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.45f, 0.45f, 0.45f));
	pEffect->Set_EffectColor(_vec4(0.2f, 0.6f, 1.f, 0.7f));
	pEffect->Set_DetailPos(_vec3(0.f, 0.5f, 0.f));
	pEffectList.push_back(pEffect);

	// AuraMesh
	pEffect = CEffect_Mesh_Shield::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.4f, 0.4f, 0.4f));
	pEffect->Set_EffectColor(_vec4(0.2f, 0.6f, 1.f, 0.7f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UV(_vec2(5.f, 5.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(0.25f, 0.f), 0.1f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_LightingAura", pEffectList);
	////--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_In_LightingAura---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 2.5f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(6.f, 6.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(1.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(6.5f, 6.f, TRUE);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Mesh_ElectricGroup1::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 0.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.03f, 0.03f, 0.03f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_FlickerDegree(1.5f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Mesh_ElectricGroup2::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 0.f, 2.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.05f, 0.05f, 0.05f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_FlickerDegree(3.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_In_LightingAura", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_HealingAura---------------------------------------
	//Particle
	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Healing_Aura")), SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_DetailPos(_vec3(0.f, 23.f, 0.f));
	pEffectList.push_back(pEffect);

	// Wave
	pEffect = CEffect_Mesh_Wave::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(0.f, 1.f), 0.25f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UV(_vec2(1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.45f, 0.45f, 0.45f));
	pEffect->Set_EffectColor(_vec4(0.8f, 0.4f, 0.7f, 0.7f));
	pEffect->Set_DetailPos(_vec3(0.f, 0.5f, 0.f));
	pEffectList.push_back(pEffect);

	// AuraMesh
	pEffect = CEffect_Mesh_Shield::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.4f, 0.4f, 0.4f));
	pEffect->Set_EffectColor(_vec4(0.8f, 0.4f, 0.7f, 0.7f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UV(_vec2(5.5f, 5.5f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(0.25f, 0.f), 0.1f);
	pEffectList.push_back(pEffect);

	Engine::CEffectMgr::GetInstance()->Add_EffectList(L"Effect_HealingAura", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_In_HealingAura---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(5.f, 0.5f, 4.7f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(40.f, 40.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-20.f, 4.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(0.f, 40.f, TRUE);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(5.f, 0.5f, 4.7f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(40.f, 40.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(90.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-20.f, 4.5f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(0.f, 40.f, TRUE);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.5f, 0.5f, 2.2f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(6.f, 6.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(1.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(6.5f, 6.f, TRUE);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_In_HealingAura", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_HealingAura_Effected---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 2.5f, 4.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(20.f, 20.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-20.f, 4.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(0.f, 20.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.8f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 2.5f, 4.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(20.f, 20.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(90.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-20.f, 4.5f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(0.f, 20.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.8f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EnergySwirl", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(10.f, 10.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 2.f, 1.f));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 0.8f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(20.f, 10.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.8f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, 90.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EnergySwirl", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(5.f, 5.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 2.f, 1.f));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 0.8f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(20.f, 10.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.8f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, 60.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EnergySwirl", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 2.f, 1.f));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 0.8f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(20.f, 10.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.8f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, 30.f));
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_HealingAura_Effected", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_DrainAura---------------------------------------
	//Particle
	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Drain_Aura")), SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_DetailPos(_vec3(0.f, 23.f, 0.f));
	pEffectList.push_back(pEffect);

	// Wave
	pEffect = CEffect_Mesh_Wave::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(0.f, 1.f), 0.25f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UV(_vec2(1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.45f, 0.45f, 0.45f));
	pEffect->Set_EffectColor(_vec4(0.6f, 0.6f, 0.f, 0.7f));
	pEffect->Set_DetailPos(_vec3(0.f, 0.5f, 0.f));
	pEffectList.push_back(pEffect);

	// AuraMesh
	pEffect = CEffect_Mesh_Shield::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.4f, 0.4f, 0.4f));
	pEffect->Set_EffectColor(_vec4(0.6f, 0.6f, 0.f, 0.7f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UV(_vec2(5.5f, 5.5f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(0.25f, 0.f), 0.1f);
	pEffectList.push_back(pEffect);

	Engine::CEffectMgr::GetInstance()->Add_EffectList(L"Effect_DrainAura", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_In_DrainAura---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(3.f, 3.f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(40.f, 40.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-20.f, 4.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(0.f, 40.f, TRUE);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(3.f, 3.f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(40.f, 40.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(90.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-20.f, 4.5f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(0.f, 40.f, TRUE);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.5f, 2.5f, 2.5f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(7.f, 7.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(1.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(7.5f, 7.f, TRUE);
	pEffectList.push_back(pEffect);



	m_pEffectMgr->Add_EffectList(L"Effect_In_DrainAura", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_EnrageAura---------------------------------------
	//Particle
	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Enrage_Aura")), SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_DetailPos(_vec3(0.f, 23.f, 0.f));
	pEffectList.push_back(pEffect);

	// Wave
	pEffect = CEffect_Mesh_Wave::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(0.f, 1.f), 0.25f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UV(_vec2(1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.45f, 0.45f, 0.45f));
	pEffect->Set_EffectColor(_vec4(1.f, 0.4f, 0.4f, 0.7f));
	pEffect->Set_DetailPos(_vec3(0.f, 0.5f, 0.f));
	pEffectList.push_back(pEffect);

	// AuraMesh
	pEffect = CEffect_Mesh_Shield::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.4f, 0.4f, 0.4f));
	pEffect->Set_EffectColor(_vec4(1.f, 0.4f, 0.4f, 0.7f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UV(_vec2(5.5f, 5.5f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(0.25f, 0.f), 0.1f);
	pEffectList.push_back(pEffect);

	Engine::CEffectMgr::GetInstance()->Add_EffectList(L"Effect_EnrageAura", pEffectList);
	//--------------------------------------------------------------------------------------------


	pEffectList.clear();

	// ---------------------------------------Effect_Torch---------------------------------------
	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Flame2", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 24.f, 16.f));
	pEffect->Set_EffectColor(_vec4(2.f, 0.8f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(3.3f, 5.3f, 1.f));      // 이거 세부조정 어케함
	pEffectList.push_back(pEffect);

	///////// Effect_Particle
	//pEffect = CEffect_Particle::Create(m_pGraphicDev
	//	, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Torch")), SCENE_STATIC);
	//if (nullptr == pEffect)
	//	return E_FAIL;
	//pEffect->Set_DetailPos(_vec3(0.f, 2.f, 0.f));
	//pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Torch", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_BlueTorch---------------------------------------
	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Flame2", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 24.f, 16.f));
	pEffect->Set_EffectColor(_vec4(0.f, 0.6f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(5.5f, 7.6f, 1.f));
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_BlueTorch", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_PinkTorch---------------------------------------
	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Flame2", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 24.f, 16.f));
	pEffect->Set_EffectColor(_vec4(1.f, 0.f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(5.5f, 7.6f, 1.f));
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_PinkTorch", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_YellowTorch---------------------------------------
	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Flame2", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 24.f, 16.f));
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(5.5f, 7.6f, 1.f));
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_YellowTorch", pEffectList);
	//--------------------------------------------------------------------------------------------	

	pEffectList.clear();

	// ---------------------------------------Effect_GreenTorch---------------------------------------
	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Flame2", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 24.f, 16.f));
	pEffect->Set_EffectColor(_vec4(0.f, 1.f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(5.5f, 7.6f, 1.f));
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_GreenTorch", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Flare---------------------------------------
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_Flare", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(4.f, 4.f, 4.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 5.f);
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_EffectColor(_vec4(0.3f, 1.f, 0.3f, 0.7f));
	pEffect->Set_TimeToDead(0.7f);
	pEffect->Set_FadeOut(2.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Flare", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Missile---------------------------------------
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_Flare", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(3.5f, 3.5f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_EffectColor(_vec4(0.4f, 2.f, 0.4f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(6.5f, 6.5f, TRUE);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_stars", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(9.f, 9.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 0.f, 0.8f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(20.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, 180.f));
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(2.8f, 2.8f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Mesh_Projectile1::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.3f, 2.5f, 0.4f, 1.f));
	//pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.3f, 0.15f, 0.15f));
	pEffect->Set_UseDotProduct();
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_Missile", pEffectList);
	//--------------------------------------------------------------------------------------------


	pEffectList.clear();

	// ---------------------------------------Effect_MissileDel---------------------------------------
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_Flare", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 1.f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(4.f, 4.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(25.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(14.f, 4.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(2.7f);
	pEffect->Set_TimeToDead(4.f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Decal::Create(m_pGraphicDev, L"Com_Texture_Cloud", L"Com_Texture_splatter");
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.2f, 1.f, 0.2f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(15.f, 10.f, 15.f));
	pEffect->Set_DetailPos(_vec3(0.f, 0.f, 0.f));
	dynamic_cast<CEffect_Decal*>(pEffect)->Set_DecalOp(5.f, 5.f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Green_Missile")), SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_MissileDel", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Crystal_Radial---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(1.6f, 0.f, 3.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(70.f, 70.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-7.f, 24.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(0.f, 70.f, TRUE);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(1.6f, 0.f, 3.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(70.f, 70.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(90.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-7.f, 18.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(0.f, 70.f, TRUE);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(1.6f, 0.f, 3.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(70.f, 70.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(180.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-7.f, 12.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(0.f, 70.f, TRUE);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(1.6f, 0.f, 3.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(70.f, 70.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(270.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-7.f, 6.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(0.f, 70.f, TRUE);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Crystal_Radial", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_MonsterGreenBlood1---------------------------------------

	// MonsterBlood
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BloodSet_Blue", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 3.f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 5.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(35.f, 35.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(2.f);
	pEffect->Set_DetailPos(_vec3(0.f, 13.f, 0.f));
	pEffect->Set_TimeToDead(2.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_MonsterGreenBlood1", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_MonsterGreenBlood2---------------------------------------

	// MonsterBlood
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BloodSet_Green", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 3.f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(40.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 5.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(30.f, 30.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(2.f);
	pEffect->Set_DetailPos(_vec3(0.f, 13.f, 0.f));
	pEffect->Set_TimeToDead(2.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_MonsterGreenBlood2", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_MonsterGreenBlood3---------------------------------------

	// MonsterBlood
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BloodSet_Red", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 3.f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(90.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 5.5f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(30.f, 30.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(2.7f);
	pEffect->Set_DetailPos(_vec3(0.f, 13.f, 0.f));
	pEffect->Set_TimeToDead(2.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_MonsterGreenBlood3", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_MonsterGreenExplosion---------------------------------------


	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Explosion", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 28.f, 16.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_LoopCount(1);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_EffectColor(_vec4(1.5f, 5.f, 0.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(2.5f, 2.5f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(30.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(20.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut();
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Explosion", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 28.f, 16.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_LoopCount(1);
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(1.5f, 5.f, 0.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(2.5f, 2.5f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(180.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(30.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(20.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut();
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_MonsterGreenExplosion", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_MonsterRedBlood1---------------------------------------

	// MonsterBlood
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BloodSet_Blue", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.5f, 0.f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(10.f, 10.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(15.f, 5.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(45.f, 35.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(2.f);
	pEffect->Set_DetailPos(_vec3(0.f, 40.f, 0.f));
	pEffect->Set_TimeToDead(2.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_MonsterRedBlood1", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_MonsterRedBlood2---------------------------------------

	// MonsterBlood
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BloodSet_Green", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.5f, 0.f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(10.f, 10.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(40.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(15.f, 5.5f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(45.f, 30.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(2.f);
	pEffect->Set_DetailPos(_vec3(0.f, 40.f, 0.f));
	pEffect->Set_TimeToDead(2.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_MonsterRedBlood2", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_MonsterRedBlood3---------------------------------------

	// MonsterBlood
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BloodSet_Red", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.5f, 0.f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(10.f, 10.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(90.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(15.f, 5.5f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(45.f, 30.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(2.7f);
	pEffect->Set_DetailPos(_vec3(0.f, 40.f, 0.f));
	pEffect->Set_TimeToDead(2.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_MonsterRedBlood3", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_MonsterRedExplosion---------------------------------------


	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Explosion", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 28.f, 16.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_LoopCount(1);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_EffectColor(_vec4(2.5f, 0.6f, 0.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(20.f, 20.f, 1.f));
	pEffect->Set_DetailPos(_vec3(0.f, 55.f, 0.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(60.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(45.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut();
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Explosion", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 28.f, 16.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_LoopCount(1);
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(2.5f, 0.6f, 0.f, 1.f));
	pEffect->Set_DetailPos(_vec3(0.f, 55.f, 0.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(20.f, 20.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(180.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(60.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(45.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut();
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_MonsterRedExplosion", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_PlayerWhiteBlood1---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BloodSet_Red", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(90.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 6.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(25.f, 25.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.4f);
	pEffect->Set_TimeToDead(2.f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BloodSet_Blue", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 5.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(25.f, 25.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.4f);
	pEffect->Set_TimeToDead(2.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_PlayerWhiteBlood1", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_PlayerWhiteBlood2---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BloodSet_Blue", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 6.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(25.f, 25.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.4f);
	pEffect->Set_TimeToDead(2.f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BloodSet_Green", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(40.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 5.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(25.f, 25.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.4f);
	pEffect->Set_TimeToDead(2.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_PlayerWhiteBlood2", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_PlayerWhiteBlood3---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BloodSet_Green", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(40.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 5.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(25.f, 25.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.4f);
	pEffect->Set_TimeToDead(2.f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BloodSet_Red", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(90.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 5.5f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(25.f, 25.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.4f);
	pEffect->Set_TimeToDead(2.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_PlayerWhiteBlood3", pEffectList);

	//--------------------------------------------------------------------------------------------



	pEffectList.clear();

	// ---------------------------------------Effect_MonsterPinkBlood1---------------------------------------

	// MonsterBlood
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BloodSet_Blue", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.5f, 0.f, 2.5f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 5.5f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(35.f, 35.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(2.3f);
	pEffect->Set_DetailPos(_vec3(0.f, 13.f, 0.f));
	pEffect->Set_TimeToDead(2.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_MonsterPinkBlood1", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_MonsterPinkBlood2---------------------------------------

	// MonsterBlood
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BloodSet_Green", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.5f, 0.f, 2.5f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(40.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 5.5f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(35.f, 35.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(2.3f);
	pEffect->Set_DetailPos(_vec3(0.f, 13.f, 0.f));
	pEffect->Set_TimeToDead(2.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_MonsterPinkBlood2", pEffectList);

	//--------------------------------------------------------------------------------------------


	pEffectList.clear();

	// ---------------------------------------Effect_MonsterPinkBlood3---------------------------------------

	// MonsterBlood
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BloodSet_Red", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.5f, 0.f, 2.5f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(90.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 5.5f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(35.f, 35.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(2.3f);
	pEffect->Set_DetailPos(_vec3(0.f, 13.f, 0.f));
	pEffect->Set_TimeToDead(2.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_MonsterPinkBlood3", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_MonsterPinkExplosion---------------------------------------


	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Explosion", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 28.f, 16.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_LoopCount(1);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_EffectColor(_vec4(2.f, 0.f, 3.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(2.5f, 2.5f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(40.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(22.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut();
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Explosion", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 28.f, 16.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_LoopCount(1);
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(2.f, 0.f, 3.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(2.5f, 2.5f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(180.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(40.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(22.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut();
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_MonsterPinkExplosion", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_PlayerDeadWhiteBlood---------------------------------------

	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Blood", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 27.f, 19.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_LoopCount(1);
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameMixOp(FALSE);
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(5.f, 5.f, 5.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(30.f, 30.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(60.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(100.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(0.6f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_PlayerDeadWhiteBlood", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_MonsterDeadGreenBlood---------------------------------------

	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Blood", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 27.f, 19.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_LoopCount(1);
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameMixOp(FALSE);
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(0.f, 5.f, 0.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(30.f, 30.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(60.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(100.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(0.6f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_MonsterDeadGreenBlood", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_MonsterDeadPinkBlood---------------------------------------

	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Blood", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 27.f, 19.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_LoopCount(1);
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameMixOp(FALSE);
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(5.f, 0.f, 5.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(30.f, 30.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(60.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(100.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(0.6f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_MonsterDeadPinkBlood", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Mage_Charging---------------------------------------
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_projectile", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 0.7f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(75.f, 2.5f, 1.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_Flare", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.f, 1.3f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(30.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(15.f, 2.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(44.f, 9.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.7f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_Flare", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.f, 1.3f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(3.f, 3.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(180.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(15.f, 3.7f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(51.f, 12.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.7f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.5f, 1.f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(40.f, 40.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-7.f, 15.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(10.f, 40.f, TRUE);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.5f, 1.f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(40.f, 40.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(180.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-7.f, 10.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(10.f, 40.f, TRUE);
	pEffectList.push_back(pEffect);



	m_pEffectMgr->Add_EffectList(L"Effect_Mage_Charging", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Mage_Charging_Att---------------------------------------

	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_ChargeAtt")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_RotZ();
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Mesh_ManaBomb::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(3.f, 2.8f, 0.5f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.f, 0.f, 0.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_Scaling(2.5f, 1.f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_MaxScaling(1.4f, FALSE);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Mesh_Shield::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.5f, 2.f, 0.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.f, 0.f, 0.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UV(_vec2(100.f, 100.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_Scaling(4.f, 0.3f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_MaxScaling(0.9f, FALSE);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_ScalingFadeOut(2.6f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.03f);
	pEffect->Set_EffectColor(_vec4(100.f, 30.f, 3.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), D3DXToRadian(180.f), 0.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(160.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(180.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.f);
	pEffect->Set_DetailPos(_vec3(0.f, 3.f, 0.f));
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.03f);
	pEffect->Set_EffectColor(_vec4(200.f, 50.f, 3.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), 0.f, 0.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(180.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(200.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.f);
	pEffect->Set_DetailPos(_vec3(0.f, 2.5f, 0.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Decal::Create(m_pGraphicDev, L"Com_Texture_Decal");
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 0.8f, 0.5f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(50.f, 10.f, 50.f));
	dynamic_cast<CEffect_Decal*>(pEffect)->Set_DecalOp(1.2f, 0.3f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Mage_Charging_Att", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Mage_Casting_ManaBomb---------------------------------------
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_Flare", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_StartTimeControl(0.8f);
	pEffect->Set_EffectColor(_vec4(1.7f, 0.7f, 1.4f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(30.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 3.5f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(25.f, 4.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut();
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_Flare", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_StartTimeControl(0.8f);
	pEffect->Set_EffectColor(_vec4(2.f, 1.f, 1.7f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(3.f, 3.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(180.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 5.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(33.f, 8.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut();
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(3.f, 1.5f, 3.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(60.f, 60.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-7.f, 20.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(10.f, 60.f, TRUE);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.5f, 0.5f, 1.5f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(60.f, 60.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(180.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-7.f, 15.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(10.f, 60.f, TRUE);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Mage_Casting_ManaBomb", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Mage_ManaBomb_Att---------------------------------------

	pEffect = CEffect_Mesh_SplosionRings::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(3.f, 1.f, 3.3f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.f, 0.f, 0.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_Scaling(2.85f, 1.f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_MaxScaling(3.1f, FALSE);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_ScalingFadeOut(1.75f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Mesh_ManaBomb::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_StartTimeControl(0.3f);
	pEffect->Set_EffectColor(_vec4(1.7f, 0.f, 3.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.f, 0.f, 0.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(40.f), D3DXToRadian(60.f), 0.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_Scaling(2.75f, 1.f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_MaxScaling(1.5f, FALSE);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Mesh_Wave::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(3.7f, 0.6f, 5.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.f, 0.f, 0.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_Scaling(1.f, 1.f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_MaxScaling(0.7f, FALSE);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_ScalingFadeOut(1.6f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UV(_vec2(3.f, 1.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_MageManaBomb_Att")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_RotZ();
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_SpreadExceptY();
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.03f);
	pEffect->Set_EffectColor(_vec4(80.f, 5.f, 50.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), 0.f, 0.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(160.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(180.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.f);
	pEffect->Set_DetailPos(_vec3(0.f, 3.f, 0.f));
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Mage_ManaBomb_Att", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Get_GreenManaCrystal---------------------------------------
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BlackStars", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 1.3f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(5.f, 5.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(30.f)));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(12.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(14.f, 4.f, FALSE);
	//dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.2f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, 240.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MoveDir(_vec3(0.f, 38.f, 0.f));
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_Get_GreenManaCrystal", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Get_BlueManaCrystal---------------------------------------
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_BlackStars", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 0.f, 1.3f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(5.f, 5.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(-30.f)));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(12.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(14.f, 4.f, FALSE);
	//dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.2f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, -240.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MoveDir(_vec3(0.f, 38.f, 0.f));
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_Get_BlueManaCrystal", pEffectList);
	//--------------------------------------------------------------------------------------------


	pEffectList.clear();

	// ---------------------------------------Effect_SummoingTower---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 0.f, 3.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(40.f, 40.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-10.f, 4.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(0.f, 40.f, TRUE);
	pEffect->Set_TimeToDead(5.f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 0.f, 3.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(30.f, 30.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(90.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-10.f, 4.5f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(0.f, 30.f, TRUE);
	pEffect->Set_TimeToDead(5.f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.5f, 1.f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(5.f, 5.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(1.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(6.5f, 5.f, TRUE);
	pEffect->Set_TimeToDead(5.f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.5f, 2.5f, 2.5f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(1.5f, 1.5f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(1.f, 0.4f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(2.f, 1.5f, TRUE);
	pEffect->Set_TimeToDead(5.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_SummoingTower", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_BuildTower---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectLongSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.2f, 0.6f, 0.83f, 1.f));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(25.f, 37.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(20.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(50.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffect->Set_DetailPos(_vec3(0.f, 22.f, 0.f));
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_HighlightCircle", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.15f, 0.15f, 2.f, 1.f));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(20.f, 20.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), 0.f, 0.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(5.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(25.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut();
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MoveDir(_vec3(0.f, 40.f, 0.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_HighlightCircle", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_StartTimeControl(0.35f);
	pEffect->Set_EffectColor(_vec4(0.15f, 0.15f, 2.f, 1.f));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(20.f, 20.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), 0.f, 0.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(5.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(25.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut();
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MoveDir(_vec3(0.f, 40.f, 0.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Tower_Bulid")));
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_SpreadExceptY();
	pEffect->Set_DetailPos(_vec3(0.f, 24.f, 0.f));
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_BuildTower", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_UnderMissileTower---------------------------------------

	pEffect = CEffect_Mesh_ManaBomb::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_Scaling(10.f, 0.018f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(1.f, 0.f), 0.8f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_MaxScaling(0.175f, TRUE);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Mesh_ManaBomb::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(-1.5f, 0.f), 0.5f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_Scaling(8.f, 0.015f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_MaxScaling(0.175f, TRUE);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.2f);
	pEffect->Set_EffectColor(_vec4(0.9f, 0.7f, 3.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(20.f, 45.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_DetailPos(_vec3(0.f, 19.f, 0.f));
	pEffectList.push_back(pEffect);


	Engine::CEffectMgr::GetInstance()->Add_EffectList(L"Effect_UnderMissileTower", pEffectList);
	//--------------------------------------------------------------------------------------------


	pEffectList.clear();

	// ---------------------------------------Effect_MissileTower_Ball---------------------------------------

	pEffect = CEffect_Mesh_GlowBall::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.9f, 0.7f, 1.4f, 1.f));
	pEffect->Set_DetailPos(_vec3(0.f, -8.f, 0.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.045f, 0.04f, 0.045f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(1.f, 1.f), 0.4f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UV(_vec2(0.5f, 0.5f));
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_Flare", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.9f, 0.7f, 1.4f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_DetailPos(_vec3(0.f, -8.f, 0.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(9.f, 9.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(14.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(18.f, 9.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.8f);
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_MissileTower_Ball", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_UnderFireTower---------------------------------------

	pEffect = CEffect_Mesh_ManaBomb::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.3f, 0.2f, 0.2f, 1.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_Scaling(10.f, 0.018f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(1.f, 0.f), 0.8f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_MaxScaling(0.175f, TRUE);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Mesh_ManaBomb::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.3f, 0.2f, 0.2f, 1.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(-1.5f, 0.f), 0.5f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_Scaling(8.f, 0.015f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_MaxScaling(0.175f, TRUE);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.2f);
	pEffect->Set_EffectColor(_vec4(2.3f, 0.2f, 0.2f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(20.f, 45.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_DetailPos(_vec3(0.f, 19.f, 0.f));
	pEffectList.push_back(pEffect);


	Engine::CEffectMgr::GetInstance()->Add_EffectList(L"Effect_UnderFireTower", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_FireTower_Ball---------------------------------------

	pEffect = CEffect_Mesh_GlowBall::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.7f, 0.8f, 0.25f, 1.f));
	pEffect->Set_DetailPos(_vec3(0.f, -8.f, 0.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.055f, 0.044f, 0.055f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(1.f, 1.f), 0.4f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UV(_vec2(0.5f, 0.5f));
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Flame2", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 16.f, 16.f));
	pEffect->Set_DetailPos(_vec3(1.5f, -5.7f, 0.f));
	pEffect->Set_EffectColor(_vec4(1.5f, 0.8f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(11.5f, 13.5f, 1.f));
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Flame2", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 24.f, 16.f));
	pEffect->Set_DetailPos(_vec3(-2.f, -6.f, 0.f));
	pEffect->Set_EffectColor(_vec4(1.5f, 0.8f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(11.f, 13.f, 1.f));
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_FireTower_Ball", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_UnderLightingTower---------------------------------------

	pEffect = CEffect_Mesh_ManaBomb::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.4f, 0.4f, 2.3f, 1.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_Scaling(10.f, 0.018f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(1.f, 0.f), 0.8f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_MaxScaling(0.175f, TRUE);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Mesh_ManaBomb::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.4f, 0.4f, 2.3f, 1.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(-1.5f, 0.f), 0.5f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_Scaling(8.f, 0.015f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_MaxScaling(0.175f, TRUE);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.2f);
	pEffect->Set_EffectColor(_vec4(0.4f, 0.4f, 2.3f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(20.f, 45.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_DetailPos(_vec3(0.f, 19.f, 0.f));
	pEffectList.push_back(pEffect);


	Engine::CEffectMgr::GetInstance()->Add_EffectList(L"Effect_UnderLightingTower", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_LightingTower_Ball---------------------------------------

	pEffect = CEffect_Mesh_GlowBall::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.2f, 0.8f, 2.f, 1.f));
	pEffect->Set_DetailPos(_vec3(0.f, 6.5f, 0.f));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.05f, 0.05f, 0.05f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(1.f, 1.f), 0.4f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UV(_vec2(0.5f, 0.5f));
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_Flare", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.2f, 0.2f, 3.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_DetailPos(_vec3(0.f, 6.5f, 0.f));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(9.f, 9.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(14.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(18.f, 9.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.8f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Mesh_ElectricGroup1::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 0.f, 4.f, 1.f));
	pEffect->Set_DetailPos(_vec3(0.f, 6.5f, 0.f));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.03f, 0.03f, 0.03f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_Scaling(0.03f, 2.5f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_MaxScaling(0.04f, TRUE);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffectList.push_back(pEffect);


	Engine::CEffectMgr::GetInstance()->Add_EffectList(L"Effect_LightingTower_Ball", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_UnderStrikerTower---------------------------------------

	pEffect = CEffect_Mesh_ManaBomb::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.3f, 2.3f, 0.4f, 1.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_Scaling(10.f, 0.018f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(1.f, 0.f), 0.8f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_MaxScaling(0.175f, TRUE);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Mesh_ManaBomb::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.3f, 2.3f, 0.4f, 1.f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(-1.5f, 0.f), 0.5f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_Scaling(8.f, 0.015f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_MaxScaling(0.175f, TRUE);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.2f);
	pEffect->Set_EffectColor(_vec4(2.3f, 2.3f, 0.4f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(20.f, 45.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_Y);
	pEffect->Set_DetailPos(_vec3(0.f, 19.f, 0.f));
	pEffectList.push_back(pEffect);

	Engine::CEffectMgr::GetInstance()->Add_EffectList(L"Effect_UnderStrikerTower", pEffectList);
	//--------------------------------------------------------------------------------------------


	pEffectList.clear();

	// ---------------------------------------Effect_StrikerTower_Ball---------------------------------------

	pEffect = CEffect_Mesh_GlowBall::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.4f, 1.4f, 0.2f, 1.f));
	pEffect->Set_DetailPos(_vec3(0.f, 10.f, 0.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.057f, 0.057f, 0.057f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(1.f, 1.f), 0.4f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UV(_vec2(0.5f, 0.5f));
	pEffectList.push_back(pEffect);


	Engine::CEffectMgr::GetInstance()->Add_EffectList(L"Effect_StrikerTower_Ball", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Own_Healing---------------------------------------

	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Healing_Star")), SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Particle*>(pEffect)->Set_SpreadExceptY();
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(3.5f, 0.9f, 0.9f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(16.f, 16.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(12.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(24.f, 16.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.7f);
	pEffectList.push_back(pEffect);




	m_pEffectMgr->Add_EffectList(L"Effect_Own_Healing", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_MissileTower_Missile---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 2.5f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(12.f, 12.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(1.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(13.f, 12.f, TRUE);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.2f, 1.f, 2.5f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(3.f, 3.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(1.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(4.f, 3.f, TRUE);
	pEffectList.push_back(pEffect);



	pEffect = CEffect_Mesh_Projectile1::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.5f, 0.2f, 0.8f, 1.f));
	//pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.44f, 0.34f, 0.34f));
	pEffect->Set_UseDotProduct();
	pEffectList.push_back(pEffect);

	//pEffect = CEffect_RectParticle::Create(m_pGraphicDev
	//	, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_MissileTower_Track")));
	//if (nullptr == pEffect)
	//	return E_FAIL;
	//dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FollowTrackingPath();
	//dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FadeColor(_vec3(0.f, 0.f, 1.f), 3.f,  25.f);
	//dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FadeInfo(_vec2(3.f, 1.5f));
	//pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_MissileTower_Missile", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_MissileTower_Missile_Del---------------------------------------

	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Explosion", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 16.f, 16.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_LoopCount();
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MoveDir(_vec3(0.f, 41.f, 10.f));
	pEffect->Set_DetailPos(_vec3(1.f, -2.7f, 0.f));
	pEffect->Set_EffectColor(_vec4(0.f, 0.7f, 4.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(25.f, 25.f, 1.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 7.f, 25.f, 1.f));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(10.f, 10.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), 0.f, 0.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(166.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(65.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(2.35f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 7.f, 25.f, 1.f));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(10.f, 10.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), 0.f, 0.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(166.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(65.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(2.35f);
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_MissileTower_Missile_Del", pEffectList);
	//--------------------------------------------------------------------------------------------


	pEffectList.clear();

	// ---------------------------------------Effect_FireTower_Missile---------------------------------------

	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Explosion", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 24.f, 16.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_EffectColor(_vec4(3.f, 3.f, 3.f, 2.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(13.f, 13.f, 1.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Explosion", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 24.f, 16.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_EffectColor(_vec4(2.f, 2.f, 2.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(16.f, 16.f, 1.f));
	pEffectList.push_back(pEffect);



	pEffect = CEffect_Mesh_Projectile1::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 0.3f, 1.f));
	//pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.5f, 0.8f, 0.8f));
	pEffect->Set_UseDotProduct();
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_FireTower_Missile", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_FireTower_Missile_Del---------------------------------------

	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Explosion", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 16.f, 16.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_LoopCount(1);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MoveDir(_vec3(0.f, 10.f, 0.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameMixOp(FALSE);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_EffectColor(_vec4(3.f, 1.f, 1.f, 1.f));
	pEffect->Set_DetailPos(_vec3(0.f, 2.f, 0.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(50.f, 50.f, 1.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Explosion", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 24.f, 16.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_LoopCount(1);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MoveDir(_vec3(0.f, 10.f, 0.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameMixOp(FALSE);
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(3.f, 1.f, 1.f, 1.f));
	pEffect->Set_DetailPos(_vec3(0.f, 2.f, 0.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(50.f, 50.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(120.f)));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Explosion", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 16.f, 16.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_LoopCount(1);
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameMixOp(FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MoveDir(_vec3(10.f, 25.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(1.f, 0.f, 0.f, 1.f));
	pEffect->Set_DetailPos(_vec3(0.f, 7.f, 0.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(45.f, 45.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(5.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(55.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Decal::Create(m_pGraphicDev, L"Com_Texture_Cloud", L"Com_Texture_splatter");
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.f, 1.f, 0.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(44.f, 20.f, 44.f));
	pEffect->Set_DetailPos(_vec3(0.f, -10.f, 0.f));
	dynamic_cast<CEffect_Decal*>(pEffect)->Set_MaskTextureColor(_vec3(0.5f, 1.f, 1.f));
	dynamic_cast<CEffect_Decal*>(pEffect)->Set_DecalOp(6.f, 1.5f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_FireTower_Smoke")));
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_DetailPos(_vec3(0.f, -12.f, 0.f));
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_DestBlendInv();
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FadeInfo(_vec2(1.f, 1.2f));
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FadeColor(_vec3(0.f, 0.f, 0.f), 1.f, 9.f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_FireTower_Missile_Del", pEffectList);

	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_StrikerTower_Missile---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.5f, 2.5f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(20.f, 16.f, 1.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.5f, 2.5f, 2.5f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(5.f, 5.f, 1.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Mesh_Projectile1::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.5f, 2.5f, 0.f, 1.f));
	//pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.4f, 0.5f, 0.5f));
	pEffect->Set_UseDotProduct();
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_StrikerTower_Missile", pEffectList);
	//--------------------------------------------------------------------------------------------
	pEffectList.clear();

	// ---------------------------------------Effect_StrikerTower_Charging---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(3.f, 3.f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(60.f, 60.f, 1.f));
	pEffect->Set_TimeToDead(2.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-20.f, 4.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(0.f, 60.f, TRUE);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_RadialStar", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(3.f, 3.f, 0.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(60.f, 60.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(180.f)));
	pEffect->Set_TimeToDead(2.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-20.f, 4.5f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(0.f, 60.f, TRUE);
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_StrikerTower_Charging", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_LightningTower_Missile---------------------------------------


	pEffect = CEffect_Mesh_ElectricGroup2::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 0.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.3f, 0.3f, 0.3f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_FlickerDegree(10.f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_ScalingLikeSpring(_vec3(1.f, 1.f, 1.f), 0.35f, 0.3f);
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_InitRotationRand();
	pEffect->Set_TimeToDead(0.3f);
	pEffectList.push_back(pEffect);

	//pEffect = CEffect_Mesh_ElectricGroup2::Create(m_pGraphicDev);
	//if (nullptr == pEffect)
	//	return E_FAIL;
	//pEffect->Set_StartTimeControl(1.f);
	//pEffect->Set_EffectColor(_vec4(0.f, 0.f, 1.f, 1.f));
	//pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.3f, 0.3f, 0.3f));
	//pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), 0.f, D3DXToRadian(90.f)));
	//pEffect->Set_TimeToDead(0.5f);
	//pEffectList.push_back(pEffect);



	m_pEffectMgr->Add_EffectList(L"Effect_LightningTower_Missile", pEffectList);
	//--------------------------------------------------------------------------------------------


	pEffectList.clear();

	// ---------------------------------------Effect_Mage_CommonAtt_Flare---------------------------------------
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_Flare", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(5.f, 5.f, 4.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(40.f, 1.f);
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(2.f, 0.2f, 1.5f, 0.8f));	// 핑쿠
	pEffect->Set_TimeToDead(0.8f);
	pEffect->Set_FadeOut(1.7f);
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_Mage_CommonAtt_Flare", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Mage_CommonAtt---------------------------------------
	// 메이지 기본 평타

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_projectile", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 0.2f, 0.6f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(9.f, 1.5f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(38.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingLikeSpring(_vec2(25.f, 0.02f), 18.f, 6.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, 100.f));
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_projectile", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 0.2f, 0.6f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(6.f, 1.5f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(-38.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingLikeSpring(_vec2(25.f, 0.02f), 18.f, 6.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, 100.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(1.6f, 1.6f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(1.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(2.f, 1.6f, TRUE);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.7f, 0.7f, 1.3f, 1.f));	// 핑쿠
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(5.5f, 5.5f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(1.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(6.f, 5.5f, TRUE);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Mesh_Projectile1::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.2f, 0.2f, 1.2f, 1.f));
	//pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.35f, 0.2f, 0.2f));
	pEffect->Set_UseDotProduct();
	//dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(3.f, 0.f), 1.f);
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_Mage_CommonAtt", pEffectList);
	//--------------------------------------------------------------------------------------------


	pEffectList.clear();

	// ---------------------------------------Effect_Mage_CommonAtt_Del---------------------------------------
	// 메이지 기본 평타 사라짐
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_Flare", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(2.5f, 0.35f, 2.f, 1.f));	// 핑쿠
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(4.f, 4.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(30.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(18.f, 4.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(2.f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Decal::Create(m_pGraphicDev, L"Com_Texture_Circle", L"Com_Texture_Cloud");
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.9f, 0.3f, 1.5f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(10.f, 8.f, 10.f));
//	pEffect->Set_DetailPos(_vec3(0.f, -2.f, 0.f));
	dynamic_cast<CEffect_Decal*>(pEffect)->Set_MaskTextureColor(_vec3(0.5f, 1.f, 1.f));
	dynamic_cast<CEffect_Decal*>(pEffect)->Set_DecalOp(8.f, 6.f);
	pEffectList.push_back(pEffect);

	
	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Mage_Pink_Missile")), SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Mage_CommonAtt_Del", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_GameStart_Flare---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_Flare", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 0.5f, 0.9f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(180.f, 180.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-30.f, 15.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(0.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffect->Set_TimeToDead(4.f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_Flare", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_StartTimeControl(0.3f);
	pEffect->Set_EffectColor(_vec4(1.f, 0.5f, 0.9f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(3.f, 3.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(30.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(25.f, 16.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(200.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffect->Set_TimeToDead(4.f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_Flare", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_StartTimeControl(0.5f);
	pEffect->Set_EffectColor(_vec4(1.f, 0.5f, 0.9f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(3.f, 3.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(180.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(25.f, 16.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(200.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffect->Set_TimeToDead(4.f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_GameStart_PurpleStar")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_RotZ();
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_SpreadExceptY();
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_GameStart_Flare", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Lava_Smoke1---------------------------------------

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.06f);
	pEffect->Set_EffectColor(_vec4(2.f, 2.f, 2.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(30.f, 30.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(90.f)));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(15.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(230.f, 30.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.02f);
	pEffect->Set_EffectColor(_vec4(2.f, 2.f, 2.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(15.f, 15.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(20.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(230.f, 15.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(-0.04f);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(45.f, 45.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(180.f)));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_DetailPos(_vec3(6.f, 0.f, 10.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(5.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(230.f, 45.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(-0.04f);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(45.f, 45.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(120.f)));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_DetailPos(_vec3(6.f, 0.f, 10.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(7.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(230.f, 45.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.03f);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(45.f, 45.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_DetailPos(_vec3(-6.f, 0.f, -10.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(20.f, 1.f);
	 dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(230.f, 30.f, TRUE);
	 dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.03f);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(45.f, 45.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(30.f)));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_DetailPos(_vec3(-6.f, 0.f, -10.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(13.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(230.f, 30.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffectList.push_back(pEffect);



	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(-0.05f);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(60.f, 60.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(-60.f)));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(230.f, 100.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Lava_Smoke1", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Lava_Smoke2---------------------------------------

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(-0.03f);
	pEffect->Set_EffectColor(_vec4(2.f, 2.f, 2.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(30.f, 30.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(-90.f)));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(230.f, 30.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.06f);
	pEffect->Set_EffectColor(_vec4(2.f, 2.f, 2.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(15.f, 15.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(15.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(230.f, 30.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(-0.05f);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(45.f, 45.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(-180.f)));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_DetailPos(_vec3(6.f, 0.f, 10.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(13.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(230.f, 45.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(-0.05f);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(45.f, 45.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(-140.f)));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_DetailPos(_vec3(6.f, 0.f, 10.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(11.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(230.f, 45.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.06f);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(45.f, 45.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_DetailPos(_vec3(-6.f, 0.f, -10.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(230.f, 30.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(-0.06f);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(45.f, 45.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(-20.f)));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_DetailPos(_vec3(-6.f, 0.f, -10.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(12.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(230.f, 30.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.02f);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(60.f, 60.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(60.f)));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(20.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(230.f, 100.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Lava_Smoke2", pEffectList);
	//--------------------------------------------------------------------------------------------


	pEffectList.clear();

	// ---------------------------------------Effect_SmallLava_Smoke---------------------------------------

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.04f);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(80.f, 10.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.6f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.03f);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(80.f, 15.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.2f);
	pEffect->Set_DetailPos(_vec3(2.f, -5.f, 0.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.03f);
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(80.f, 15.f, TRUE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.4f);
	pEffect->Set_DetailPos(_vec3(-2.f, -5.f, 0.f));
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_SmallLava_Smoke", pEffectList);
	//--------------------------------------------------------------------------------------------


	pEffectList.clear();

	// ---------------------------------------Effect_brazier---------------------------------------

	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Brazier_MinusX")), SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Particle*>(pEffect)->Set_SpreadExceptY();
	pEffectList.push_back(pEffect);

	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Brazier_Smoke")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_SpreadExceptY();
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_BubbleState(0.5f, 4.f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_DefaultAlpha(0.4f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_Smoke(0.07f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_brazier", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Rightbrazier---------------------------------------

	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Brazier_MinusX")), SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Particle*>(pEffect)->Set_SpreadExceptY();
	dynamic_cast<CEffect_Particle*>(pEffect)->Set_Dir(_vec3(0.f, 35.f, 25.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Brazier_Smoke")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_SpreadExceptY();
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_BubbleState(0.5f, 4.f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_DefaultAlpha(0.4f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_Smoke(0.07f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_Dir(_vec3(0.f, 25.f, 20.f));
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Rightbrazier", pEffectList);
	//--------------------------------------------------------------------------------------------


	pEffectList.clear();

	// ---------------------------------------Effect_Leftbrazier---------------------------------------

	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Brazier_MinusX")), SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Particle*>(pEffect)->Set_SpreadExceptY();
	dynamic_cast<CEffect_Particle*>(pEffect)->Set_Dir(_vec3(0.f, 35.f, -25.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Brazier_Smoke")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_SpreadExceptY();
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_BubbleState(0.5f, 4.f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_DefaultAlpha(0.4f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_Smoke(0.07f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_Dir(_vec3(0.f, 25.f, -20.f));
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Leftbrazier", pEffectList);
	//--------------------------------------------------------------------------------------------


	pEffectList.clear();

	// ---------------------------------------Effect_TrackParticleTest---------------------------------------

	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_TrackPath_Test")), SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Particle*>(pEffect)->Set_FollowTrackingPath();
	dynamic_cast<CEffect_Particle*>(pEffect)->Set_FadeInfo(_vec2(1.f, 2.f));
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_TrackParticleTest", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Demon_Fire---------------------------------------

	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Demon_Fire")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FollowTrackingPath();
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FadeInfo(_vec2(2.f, 1.f));
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FadeColor(_vec3(1.5f, 0.f, 0.f), 0.8f, 8.f);
	pEffectList.push_back(pEffect);

	//pEffect = CEffect_Mesh_Projectile1::Create(m_pGraphicDev);
	//if (nullptr == pEffect)
	//	return E_FAIL;
	//pEffect->Set_EffectColor(_vec4(2.f, 2.f, 0.3f, 1.f));
	//pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	//pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(1.f, 0.7f, 0.7f));
	//pEffect->Set_UseDotProduct();
	//pEffectList.push_back(pEffect);
	// 수정 필요 .

	m_pEffectMgr->Add_EffectList(L"Effect_Demon_Fire", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Alive---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectLongSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.2f, 0.6f, 1.4f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(65.f, 50.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(-15.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(40.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Alive_Star")), SCENE_STATIC);
	pEffect->Set_DetailPos(_vec3(0.f, 13.f, 0.f));
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_Alive", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Monster_Dead---------------------------------------

	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Monster_Dead_Star")), SCENE_STATIC);
	dynamic_cast<CEffect_Particle*>(pEffect)->Set_SpreadExceptY();
	dynamic_cast<CEffect_Particle*>(pEffect)->Set_FadeInfo(_vec2(1.f, 1.3f));
	pEffect->Set_DetailPos(_vec3(0.f, 1.f, 0.f));
	pEffectList.push_back(pEffect);

	//pEffect = CEffect_Single_Fog::Create(m_pGraphicDev, SCENE_STAGE);
	//if (nullptr == pEffect)
	//	return E_FAIL;
	//dynamic_cast<CEffect_Single_Fog*>(pEffect)->Set_FogFadeSpeed(0.02f);
	//pEffect->Set_EffectColor(_vec4(2.f, 2.f, 2.f, 1.f));
	//pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 1.f, 1.f));
	//pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), 0.f, 0.f));
	//dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(90.f, 1.f);
	//dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(70.f, 70.f, FALSE);
	//dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.4f);
	//pEffect->Set_DetailPos(_vec3(0.f, 1.f, 0.f));
	//pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Monster_Dead", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_PinkMagicCircle---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_PinkMagicCircle", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.7f, 0.7f, 0.7f, 0.8f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(95.f, 95.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), 0.f, 0.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 10.f, 0.f));
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_PinkMagicCircle", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_BlueMagicCircle---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_PinkMagicCircle", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 0.7f, 0.7f, 0.8f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(95.f, 95.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), 0.f, 0.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, -10.f, 0.f));
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_BlueMagicCircle", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_RedMagicCircle---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_PinkMagicCircle", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.7f, 0.7f, 0.f, 0.8f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(95.f, 95.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), 0.f, 0.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 10.f, 0.f));
	pEffectList.push_back(pEffect);


	m_pEffectMgr->Add_EffectList(L"Effect_RedMagicCircle", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Tower_Destruction---------------------------------------

	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Explosion", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 24.f, 16.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_LoopCount(1);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MoveDir(_vec3(0.f, 10.f, 0.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameMixOp(FALSE);
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(2.f, 2.f, 2.f, 1.f));
	pEffect->Set_DetailPos(_vec3(0.f, 7.f, 0.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(25.f, 25.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(5.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(35.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(2.f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Frame::Create(m_pGraphicDev, L"Com_Texture_Explosion", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameInfo(FRAME(0.f, 24.f, 16.f));
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_LoopCount(1);
	dynamic_cast<CEffect_Frame*>(pEffect)->Set_FrameMixOp(FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MoveDir(_vec3(0.f, 10.f, 0.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_EffectColor(_vec4(1.f, 0.f, 0.f, 1.f));
	pEffect->Set_DetailPos(_vec3(0.f, 7.f, 0.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(35.f, 35.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(5.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(45.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(2.f);
	pEffectList.push_back(pEffect);


	pEffect = CEffect_Particle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Tower_Destruction_Smoke")), SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_DetailPos(_vec3(0.f, -3.f, 0.f));
	dynamic_cast<CEffect_Particle*>(pEffect)->Set_DestBlendInv();
	dynamic_cast<CEffect_Particle*>(pEffect)->Set_FireWork_YPositive();
	dynamic_cast<CEffect_Particle*>(pEffect)->Set_FadeInfo(_vec2(1.f, 1.1f));
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Tower_Destruction", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Attached_GreenManaCrystal1---------------------------------------
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_projectile", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 1.f, 0.f, 1.f));
	pEffect->Set_BillState(CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(15.f, 1.3f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(90.f)));
	//pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), 0.f, 0.f));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingLikeSpring(_vec2(-15.f, -0.01f), 15.f, 4.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, 30.f));
	pEffect->Set_DetailPos(_vec3(0.f, 0.2f, 0.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_projectile", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 1.f, 0.f, 1.f));
	pEffect->Set_BillState(CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(10.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(-30.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingLikeSpring(_vec2(18.f, 0.02f), 15.f, 3.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, 30.f));
	pEffect->Set_DetailPos(_vec3(0.f, 0.2f, 0.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_projectile", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 1.f, 0.f, 1.f));
	pEffect->Set_BillState(CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(15.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(60.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingLikeSpring(_vec2(-20.f, -0.02f), 16.f, 2.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, -30.f));
	pEffect->Set_DetailPos(_vec3(0.f, 0.2f, 0.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_projectile", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 1.f, 0.f, 1.f));
	pEffect->Set_BillState(CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(10.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(50.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingLikeSpring(_vec2(17.f, 0.01f), 16.f, 3.5f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, -50.f));
	pEffect->Set_DetailPos(_vec3(0.f, 0.2f, 0.f));
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Attached_GreenManaCrystal1", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Attached_GreenManaCrystal2---------------------------------------
	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_projectile", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 1.f, 0.f, 1.f));
	pEffect->Set_BillState(CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(15.f, 1.3f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(90.f)));
	//pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &_vec3(D3DXToRadian(90.f), 0.f, 0.f));
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingLikeSpring(_vec2(15.f, 0.01f), 15.f, 4.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, 30.f));
	pEffect->Set_DetailPos(_vec3(0.f, 0.2f, 0.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_projectile", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 1.f, 0.f, 1.f));
	pEffect->Set_BillState(CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(10.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(-30.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingLikeSpring(_vec2(-18.f, -0.02f), 15.f, 3.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, 30.f));
	pEffect->Set_DetailPos(_vec3(0.f, 0.2f, 0.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_projectile", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 1.f, 0.f, 1.f));
	pEffect->Set_BillState(CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(10.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(60.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingLikeSpring(_vec2(20.f, 0.02f), 16.f, 2.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, -30.f));
	pEffect->Set_DetailPos(_vec3(0.f, 0.2f, 0.f));
	pEffectList.push_back(pEffect);

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_projectile", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.f, 1.f, 0.f, 1.f));
	pEffect->Set_BillState(CEffect::BILL_ALL);
	pEffect->Set_ShaderPass(CEffect::EFFECT_ALPHABLEND_NO_Z);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(5.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(50.f)));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingLikeSpring(_vec2(-17.f, -0.01f), 16.f, 3.5f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Rotating(_vec3(0.f, 0.f, -50.f));
	pEffect->Set_DetailPos(_vec3(0.f, 0.2f, 0.f));
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Attached_GreenManaCrystal2", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Like_Spotlight---------------------------------------

	pEffect = CEffect_Mesh_SkyBeam::Create(m_pGraphicDev);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.8f, 0.8f, 0.6f, 1.f));
	pEffect->Set_DetailPos(_vec3(0.f, 85.f, 0.f));
	pEffect->Set_TimeToDead(5.f);
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(0.07f, 0.06f, 0.07f));
	dynamic_cast<CEffect_Mesh*>(pEffect)->Set_UVAnimation(_vec2(1.f, 0.f), 0.6f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Spotlight_Star")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_RotZ();
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FadeColor(_vec3(2.f, 0.8f, 1.f), 5.f, 2.f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FadeInfo(_vec2(2.f, 0.2f));
	pEffectList.push_back(pEffect);

	

	m_pEffectMgr->Add_EffectList(L"Effect_Like_Spotlight", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Demon_Hand---------------------------------------

	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Demon_Hand")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FollowTrackingPath();
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FadeColor(_vec3(2.f, 0.f, 0.f),1.f,  5.5f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FadeInfo(_vec2(2.f, 0.8f));
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Demon_Hand", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Recall---------------------------------------

	pEffect = CEffect_Single::Create(m_pGraphicDev, L"Com_Texture_EffectLongSpecular", SCENE_STATIC);
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(0.4f, 1.f, 2.8f, 1.f));
	pEffect->Set_BillState(Engine::CEffect::BILL_ALL);
	pEffect->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(25.f, 30.f, 1.f));
	dynamic_cast<CEffect_2D*>(pEffect)->Set_Scaling(10.f, 1.f);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_MaxScaling(40.f, 0.f, FALSE);
	dynamic_cast<CEffect_2D*>(pEffect)->Set_ScalingFadeOut(1.5f);
	pEffectList.push_back(pEffect);

	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Recall_Star")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FadeColor(_vec3(0.f, 0.f, 2.f), 1.f, 1.f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FadeInfo(_vec2(2.f, 0.5f));
	pEffect->Set_DetailPos(_vec3(0.f, 10.f, 0.f));
	pEffectList.push_back(pEffect);
	
	m_pEffectMgr->Add_EffectList(L"Effect_Recall", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Brazier_Fire---------------------------------------

	pEffect = CEffect_RectParticle::Create(m_pGraphicDev
		, *(Engine::CEffectMgr::GetInstance()->Find_ParticleInfo(L"Particle_Brazier_Fire")));
	if (nullptr == pEffect)
		return E_FAIL;
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FadeColor(_vec3(2.f, 0.f, 0.f), 1.f, 3.f);
	dynamic_cast<CEffect_RectParticle*>(pEffect)->Set_FadeInfo(_vec2(1.f, 1.f));
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Brazier_Fire", pEffectList);
	//--------------------------------------------------------------------------------------------
	


	/////////////////////Spray~~
	

	pEffectList.clear();

	// ---------------------------------------Effect_Spray_WellPlayed---------------------------------------

	pEffect = CEffect_Decal::Create(m_pGraphicDev, L"Com_Texture_Spray_WellPlayed");
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(30.f, 7.f, 13.f));
	//pEffect->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, D3DXToRadian(90.f), 0.f));
	pEffect->Set_DetailPos(_vec3(0.f, -3.f, 0.f));
	dynamic_cast<CEffect_Decal*>(pEffect)->Set_DecalOp(1.f, 0.05f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Spray_WellPlayed", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Spray_Sorry---------------------------------------

	pEffect = CEffect_Decal::Create(m_pGraphicDev, L"Com_Texture_Spray_Sorry");
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(30.f, 7.f, 13.f));
	pEffect->Set_DetailPos(_vec3(0.f, -5.f, 0.f));
	dynamic_cast<CEffect_Decal*>(pEffect)->Set_DecalOp(2.f, 0.1f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Spray_Sorry", pEffectList);
	//--------------------------------------------------------------------------------------------

	pEffectList.clear();

	// ---------------------------------------Effect_Spray_Oops---------------------------------------

	pEffect = CEffect_Decal::Create(m_pGraphicDev, L"Com_Texture_Spray_Oops");
	if (nullptr == pEffect)
		return E_FAIL;
	pEffect->Set_EffectColor(_vec4(1.f, 1.f, 1.f, 1.f));
	pEffect->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(30.f, 7.f, 13.f));
	pEffect->Set_DetailPos(_vec3(0.f, -5.f, 0.f));
	dynamic_cast<CEffect_Decal*>(pEffect)->Set_DecalOp(2.f, 0.1f);
	pEffectList.push_back(pEffect);

	m_pEffectMgr->Add_EffectList(L"Effect_Spray_Oops", pEffectList);
	//--------------------------------------------------------------------------------------------

	return NOERROR;
}

HRESULT CLoading::Load_UI()
{
	Engine::CComponent*			pComponent = nullptr;

	// Stage2
	LoadTexture(L"../Resources/Texture/UI/Login/%d.png", L"Com_Texture_Panel_Login", TYPE_NORMAL, 4, SCENE_STATIC);

	// --- Perspective UI ---

	// Tower UI
	LoadTexture(L"../Resources/Texture/UI/TowerRadius/towerBuildRange.tga", L"Com_Texture_TowerRaidus", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/TowerRange/%d.tga", L"Com_Texture_TowerRange", TYPE_NORMAL, 5, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/TowerRange2/%d.tga", L"Com_Texture_TowerRange2", TYPE_NORMAL, 5, SCENE_STATIC);

	// ObjectTool Texture (조명 위치, 몬스터 루트 등)
	LoadTexture(L"../Resources/Texture/MapTexture/Light%d.tga", L"Com_Texture_Light", TYPE_NORMAL, 6, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/MapTexture/Route%d.tga", L"Com_Texture_Route", TYPE_NORMAL, 8, SCENE_STATIC);

	// Skill UI
	LoadTexture(L"../Resources/Texture/UI/SkillUI/SlowAura/%d.png", L"Com_Texture_UI_SlowAura", TYPE_NORMAL, 4, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/SkillUI/HealAura/%d.png", L"Com_Texture_UI_HealAura", TYPE_NORMAL, 4, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/SkillUI/EnlargeAura/%d.png", L"Com_Texture_UI_EnlargeAura", TYPE_NORMAL, 4, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/SkillUI/DeathlyHallow/%d.png", L"Com_Texture_UI_DeathlyHallow", TYPE_NORMAL, 4, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/SkillUI/DrainAura/%d.png", L"Com_Texture_UI_DrainAura", TYPE_NORMAL, 4, SCENE_STATIC);

	// --- Orthographic UI ---

	// MeterTile
	LoadTexture(L"../Resources/Texture/UI/Panel/MeterTile.tga", L"Com_Texture_MeterTile", TYPE_NORMAL, 1, SCENE_STATIC);

	// Mouse
	LoadTexture(L"../Resources/Texture/UI/Mouse/%d.png", L"Com_Texture_Mouse", TYPE_NORMAL, 3, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Mouse/%d.tga", L"Com_Texture_Mouse2", TYPE_NORMAL, 1, SCENE_STATIC);

	// Minimap
	LoadTexture(L"../Resources/Texture/UI/Minimap/MiniMap_MagusQuarters.png", L"Com_Texture_MiniMap", TYPE_NORMAL, 1, SCENE_STATIC);

	// Chat
	LoadTexture(L"../Resources/Texture/UI/Chat/Chat.png", L"Com_Texture_Chat", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Font/Chat/%d.png", L"Com_Texture_Font_Chat", TYPE_NORMAL, 46, SCENE_STATIC); // Chat
	LoadTexture(L"../Resources/Texture/UI/Font/Chat2/%d.png", L"Com_Texture_Font_Chat2", TYPE_NORMAL, 46, SCENE_STATIC); // Chat2
	LoadTexture(L"../Resources/Texture/UI/Text/Player/Player%d.png", L"Com_Texture_Text_Player", TYPE_NORMAL, 6, SCENE_STATIC);

	// Panels
	LoadTexture(L"../Resources/Texture/UI/Panel/Panel_Level.tga", L"Com_Texture_Panel_Level", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Panel/Panel_SkillIcon2.tga", L"Com_Texture_Panel_SkillIcon", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Panel/Panel_Wave.tga", L"Com_Texture_Panel_Wave", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Panel/Panel_newBar.tga", L"Com_Texture_Panel_newBar", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Panel/newBarMask.tga", L"Com_Texture_newBarMask", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Panel/EnemyKillBar.tga", L"Com_Texture_Panel_EnemyKillBar", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Panel/EnemyKillBarMask.tga", L"Com_Texture_EnemyKillBarMask", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Panel/HeroInfo_ExpBar.tga", L"Com_Texture_Panel_HeroInfo_ExpBar", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Panel/HeroInfo_ExpBar_Mask.tga", L"Com_Texture_HeroInfo_ExpBar_Mask", TYPE_NORMAL, 1, SCENE_STATIC);

	// SkillIcon
	LoadTexture(L"../Resources/Texture/UI/SkillIcon/Mage/%d.tga", L"Com_Texture_SkillIcon_Mage", TYPE_NORMAL, 10, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/SkillIcon/Monk/%d.tga", L"Com_Texture_SkillIcon_Monk", TYPE_NORMAL, 10, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/SkillIcon/Huntress/%d.tga", L"Com_Texture_SkillIcon_Huntress", TYPE_NORMAL, 10, SCENE_STATIC);

	// Font
	LoadTexture(L"../Resources/Texture/UI/Font/Green/%d.png", L"Com_Texture_Font_Number_Green", TYPE_NORMAL, 10, SCENE_STATIC); // Score
	LoadTexture(L"../Resources/Texture/UI/Font/Skin/%d.png", L"Com_Texture_Font_Number_Skin", TYPE_NORMAL, 11, SCENE_STATIC); // Level
	LoadTexture(L"../Resources/Texture/UI/Font/Blue/%d.png", L"Com_Texture_Font_Number_Blue", TYPE_NORMAL, 11, SCENE_STATIC); // Mp
	LoadTexture(L"../Resources/Texture/UI/Font/Red/%d.png", L"Com_Texture_Font_Number_Red", TYPE_NORMAL, 11, SCENE_STATIC); // Hp
	LoadTexture(L"../Resources/Texture/UI/Font/Damage/%d.png", L"Com_Texture_Font_Damage", TYPE_NORMAL, 11, SCENE_STATIC); // Damage(0~9 ,)
	LoadTexture(L"../Resources/Texture/UI/Font/Damage/enchanted_land/%d.png", L"Com_Texture_Font_PlayerDamage", TYPE_NORMAL, 11, SCENE_STATIC); // PlayerDamage(0~9 ,)
	LoadTexture(L"../Resources/Texture/UI/Font/Respawn/%d.png", L"Com_Texture_Font_Respawn", TYPE_NORMAL, 4, SCENE_STATIC); // Respawn(0~3 ,)

	// Text
	LoadTexture(L"../Resources/Texture/UI/Text/Name_Level/Name_Level.png", L"Com_Texture_Text_Name_Level", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Text/Score/Score.png", L"Com_Texture_Text_Score", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Text/Wave/Build.png", L"Com_Texture_Text_Build", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Text/Wave/Combat.png", L"Com_Texture_Text_Combat", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Text/Wave/DefenseUnits.png", L"Com_Texture_Text_DefenseUnits", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Text/Wave/Wave.png", L"Com_Texture_Text_Wave", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Text/NextEnemies/NextEnemies.png", L"Com_Texture_Text_NextEnemies", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Text/Tower/%d.png", L"Com_Texture_Text_Tower", TYPE_NORMAL, 2, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Text/ActivateCrystal/ActivateCrystal.png", L"Com_Texture_Text_ActivateCrystal", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Text/Phase/%d.png", L"Com_Texture_Text_Phase", TYPE_NORMAL, 3, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Text/Title/%d.png", L"Com_Texture_Text_Title", TYPE_NORMAL, 3, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Text/Boss/%d.png", L"Com_Texture_Text_Boss", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Text/RespawnIn/%d.png", L"Com_Texture_Text_RespawnIn", TYPE_NORMAL, 1, SCENE_STATIC);
	LoadTexture(L"../Resources/Texture/UI/Text/ToLobby/%d.png", L"Com_Texture_Text_ToLobby", TYPE_NORMAL, 9, SCENE_STATIC);

	// Vertical
	LoadTexture(L"../Resources/Texture/UI/VerticalBar/%d.tga", L"Com_Texture_VerticalBar", TYPE_NORMAL, 5, SCENE_STATIC);

	// LowHp
	LoadTexture(L"../Resources/Texture/UI/HP_Low/%d.png", L"Com_Texture_LowHp", TYPE_NORMAL, 1, SCENE_STATIC);
	
	// Player_Left
	LoadTexture(L"../Resources/Texture/UI/PlayerLeft/%d.png", L"Com_Texture_PlayerLeft", TYPE_NORMAL, 31, SCENE_STATIC);

	// 자기장
	LoadTexture(L"../Resources/Texture/UI/Text/Electric/%d.png", L"Com_Texture_Electric", TYPE_NORMAL, 10, SCENE_STATIC);

	// 게임 시작 10.!
	LoadTexture(L"../Resources/Texture/UI/Text/Game_Start/%d.png", L"Com_Texture_Game_Start", TYPE_NORMAL, 20, SCENE_STATIC);

	return NOERROR;
}

CLoading* CLoading::Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGTYPE eLoadType)
{
	CLoading* pLoad = new CLoading(pGraphicDev, eLoadType);
	if (FAILED(pLoad->Ready_Loading()))
	{
		MSG_BOX(L"Loding Create Failed");
		Engine::Safe_Release(pLoad);
	}

	return pLoad;
}

void CLoading::Free(void)
{
	Engine::Safe_Release(m_pEffectMgr);
	Engine::Safe_Release(m_pComponentMgr);
	Engine::Safe_Release(m_pGraphicDev);
}
