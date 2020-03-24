#include "stdafx.h"
#include "Player.h"
#include "Renderer.h"
#include "ClientNetwork.h"
#include "Weapon.h"
#include "SoundMgr.h"
#include "NavMgr.h"
#include "CollisionSphere.h"
#include "Component_Manager.h"
#include "DataManager.h"
#include "CollisionManager.h"
#include "InputDev.h"
#include "InfoSubject.h"
#include "MyEffect.h"
#include "Object_Manager.h"
#include "Camera_Manager.h"
#include "TowerRadius.h"
#include "TowerRange.h"
#include "TowerRange2.h"
#include "Text_Tower.h"
#include "Mouse_Manager.h"
#include "MousePointer.h"
#include "LowHp.h"

// Tower
//#include "Effect_Mesh_AuraSphere.h"
#include "Effect_Mesh_Shield.h"
#include "FireTower.h"
#include "MissileTower.h"
#include "StrikerTower.h"
#include "LightningTower.h"
#include "Blockade.h"

#include "EffectMgr.h"
#include "Effect_Particle.h"
#include "Effect_RectParticle.h"
#include "Aura.h"

#include "Font_PlayerDamage.h"

USING(Engine)

CPlayer::CPlayer(LPDIRECT3DDEVICE9 pGraphicDev)
: CDynamicObject(pGraphicDev)
, m_pInputDev(Engine::CInputDev::GetInstance())
, m_pInfoSubject(Engine::CInfoSubject::GetInstance())
, m_iAttackCount(0)
, m_iClinetNum(1)
, m_bFirstUpdate(false)
, m_iHighRenderState(0)
, m_iPreHighRenderState(0)
, m_pPlayerInfo(nullptr)
, m_pWeapon(nullptr)
, m_dwIndex(-1)
, m_pNavMgr(CNavMgr::GetInstance())
, m_pCamMgr(CCamera_Manager::GetInstance())
, m_pCollisionBody(nullptr)
, m_pCollisionManager(CCollisionManager::GetInstance())
, m_fTrunCount(0.f)
, m_bFirstFrame(false)
, m_bAbleToBuild(false)
, m_pPreviewTower(nullptr)
, m_pTowerRadius(nullptr)
, m_pTowerRange(nullptr)
, m_pTowerRange2(nullptr)
, m_pText_Tower(nullptr)
, m_pMousePointer(dynamic_cast<CMousePointer*>(CMouse_Manager::GetInstance()->Get_MousePointer()))
, m_eBuildTower(TOWER_TYPE_END)
, m_fWaitingTime(0.f)
, m_fSummoningTime(-1.f)
, m_bExitServer(false)
, m_vecColor(0.f, 0.f, 0.f, 0.f)
, m_vecTransColor(0.f, 0.f, 0.f, 0.f)
{
	m_pInputDev->Add_Ref();
	m_pInfoSubject->Add_Ref();
	m_pNavMgr->Add_Ref();
	m_pCamMgr->Add_Ref();
	m_pCollisionManager->Add_Ref();
	m_pMousePointer->Add_Ref();
	ZeroMemory(&m_tLocalData, sizeof(Engine::NETDATA));

	m_fColorLimitTime = 0.15f;
}

CPlayer::~CPlayer()
{
}

void CPlayer::SetServerIndex(int iIndex)
{
	if (-1 == iIndex)
		cout << "인덱스 세팅이 제대로 안됐음!" << endl;

	m_iClinetNum = iIndex;
	cout << "m_iClinetNum" << m_iClinetNum << endl;

	if (m_iClinetNum == g_iClientIndex)
		m_bMyClient = true;
}

void CPlayer::Play_Animation(const float& fTimeDelta)
{
	if (m_pPlayerInfo->tUnitInfo.bDead)
		return;

	//////// Low Animation
	if (m_iLowPreRenderState != m_iLowRenderState)
	{
		m_iLowPreRenderState = m_iLowRenderState;
		m_pDynamicMeshCom->Set_LowAnimationSet(m_vecAnimationInfo[m_iLowRenderState]);
	}

	if (m_pDynamicMeshCom->GetPeriod() - m_vecAnimationInfo[m_iLowRenderState].dEndAniCount <= m_pDynamicMeshCom->GetTrackPosition())
	{
		m_iLowPreRenderState = -1;
	}

	/////// High Animation
	if (m_iPreHighRenderState != m_iHighRenderState)
	{
		m_iPreHighRenderState = m_iHighRenderState;
		if (PLAYER_MOVE == m_pPlayerInfo->eMainState)
			m_pPlayerInfo->eMainState = PLAYER_INPUT;
		m_pDynamicMeshCom->Set_AnimationSet(m_vecAnimationInfo[m_iHighRenderState]);
	}

	if(m_pDynamicMeshCom->GetPeriod() - m_vecAnimationInfo[m_iHighRenderState].dEndAniCount <= m_pDynamicMeshCom->GetTrackPosition())
	{
		m_iPreHighRenderState = -1;
		
		switch (m_pPlayerInfo->eMainState)
		{
		case PLAYER_JUMP:
		case PLAYER_JUMP_FALLING:
			m_pPlayerInfo->eMainState = PLAYER_JUMP_FALLING;
			break;
		case PLAYER_JUMP_ATTACK:
		case PLAYER_JUMP_FALLING_ATTACK:
			m_pPlayerInfo->eMainState = PLAYER_JUMP_FALLING_ATTACK;
			break;
		case PLAYER_JUMP_ATTACK_SECOND:
		case PLAYER_JUMP_FALLING_ATTACK_SECOND:
			m_pPlayerInfo->eMainState = PLAYER_JUMP_FALLING_ATTACK_SECOND;
			break;
		case PLAYER_JUMP_ATTACK_THIRD:
		case PLAYER_JUMP_FALLING_ATTACK_THIRD:
			m_pPlayerInfo->eMainState = PLAYER_JUMP_FALLING_ATTACK_THIRD;
			break;
		case PLAYER_JUMP_ATTACK_FOURTH:
		case PLAYER_JUMP_FALLING_ATTACK_FOURTH:
			m_pPlayerInfo->eMainState = PLAYER_JUMP_FALLING_ATTACK_FOURTH;
			break;
		case PLAYER_SKILL:
			m_pPlayerInfo->eMainState = PLAYER_SKILL_ATTACK;
			break;
		case PLAYER_TOWER_PREVIEW:			// 현재 상태 유지
		case PLAYER_TOWER_PREVIEW_SECOND:
		case PLAYER_TOWER_SUMMONING:
		case PLAYER_HEAL:
			break;
		case PLAYER_ATTACK_SECOND:
		case PLAYER_ATTACK_THIRD:
			if (m_pPlayerInfo->ePlayerType == PLAYER_MAGE)
			{
				m_pPlayerInfo->eMainState = PLAYER_ATTACK_THIRD;
				break;
			}
			else
			{
				m_pPlayerInfo->eMainState = PLAYER_NOT_INPUT;
				break;
			}
		case PLAYER_DEAD:
			m_pPlayerInfo->tUnitInfo.bDead = true;
			m_fDeadTrackPosition = (_float)m_pDynamicMeshCom->GetPeriod();
			m_iDeadTrackIndex = m_iHighRenderState;
			return;
		default:
			if (1 == m_iAttackCount && PLAYER_HURT != m_pPlayerInfo->eMainState)
			{
				m_pPlayerInfo->eMainState = PLAYER_ATTACK_SECOND;
				
				if (m_pPlayerInfo->ePlayerType == PLAYER_MONK)
				{
					_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
					float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
					if (fDistance < 250.f)
					{
						CSoundMgr::GetInstance()->Play_Sound(L"Monk_meleeswing_2.ogg", Engine::CHANNEL_MONK, 0.6f);
					}
				}
			}
			else
				m_pPlayerInfo->eMainState = PLAYER_NOT_INPUT;
			break;
		}

		m_pCollisionBody->Set_HitCheck(false);
		m_iAttackCount = 0;

		if (m_pPlayerInfo->eMainState != PLAYER_HEAL && m_pOwn_HealingEffect.size() > 0)
		{
			_uint iSize = 0;

			list<CEffect*>::iterator iter_begin = m_pOwn_HealingEffect.begin();
			list<CEffect*>::iterator iter_end = m_pOwn_HealingEffect.end();
			for (; iter_begin != iter_end; ++iter_begin)
			{
				if (iSize == 0)
				{
					++iSize;
					dynamic_cast<CEffect_Particle*>((*iter_begin))->Set_LoopFalse();
					continue;
				}
				(*iter_begin)->Set_InstantFadeOut(1.f);
				++iSize;
			}
			m_pOwn_HealingEffect.clear();
		}
	}

	m_pDynamicMeshCom->Play_AnimationSet(fTimeDelta, m_pPlayerInfo->fCameraAngle / 10.f, m_pLowFilterBone, m_pHighFilterBone);
}

void CPlayer::Player_Fall(const float& fTimeDelta)
{
	if (TRUE == m_pPlayerInfo->tUnitInfo.bDead && PLAYER_DEAD == m_pPlayerInfo->eMainState)
		return;

	m_pPlayerInfo->tUnitInfo.fSpeedY -= 4.f * fTimeDelta;

	m_pTransformCom->Go_Down(m_pPlayerInfo->tUnitInfo.fSpeedY);

	bool bDropKill = false;
	if (TRUE == m_pNavMgr->FallOnNavMesh(m_pTransformCom, m_dwIndex, bDropKill)) // Collide with NavMesh
	{
		m_pPlayerInfo->tUnitInfo.fSpeedY = 0.f;

		switch (m_pPlayerInfo->eMainState)
		{
		case PLAYER_JUMP_ATTACK:
		case PLAYER_JUMP_FALLING_ATTACK:
			m_pPlayerInfo->eMainState = PLAYER_ATTACK;
			break;
		case PLAYER_JUMP_ATTACK_SECOND:
		case PLAYER_JUMP_FALLING_ATTACK_SECOND:
			m_pPlayerInfo->eMainState = PLAYER_ATTACK_SECOND;
			break;
		case PLAYER_JUMP_ATTACK_THIRD:
		case PLAYER_JUMP_FALLING_ATTACK_THIRD:
			m_pPlayerInfo->eMainState = PLAYER_ATTACK_THIRD;
			break;
		case PLAYER_JUMP_ATTACK_FOURTH:
		case PLAYER_JUMP_FALLING_ATTACK_FOURTH:
			m_pPlayerInfo->eMainState = PLAYER_ATTACK_FOURTH;
			break;
		case PLAYER_JUMP:
		case PLAYER_JUMP_FALLING:
			m_pPlayerInfo->eMainState = PLAYER_NOT_INPUT;
			break;
		}
	}
	if (bDropKill) // 낙사
	{
		if (m_pPlayerInfo->eMainState != PLAYER_DEAD && m_bMyClient)
		{
			Hit_UI(4.f);
			CCamera_Manager::GetInstance()->Shake_Camera(8000.f, 0.9f, 3); // 강도, 시간, 횟수

			if (m_pPlayerInfo->ePlayerType == PLAYER_MONK)
				CSoundMgr::GetInstance()->Play_Sound(L"Monk_death2.ogg", CHANNEL_MONK, 0.9f);
			else if (m_pPlayerInfo->ePlayerType == PLAYER_MAGE)
				CSoundMgr::GetInstance()->Play_Sound(L"Apprentice_yell.ogg", CHANNEL_MAGE, 0.9f);
		}
			
		m_pPlayerInfo->eMainState = PLAYER_DEAD;
		m_pTransformCom->Go_Down(-30.f);
	}
		
}

void CPlayer::Player_Check_SecondFloor(void)
{
	if (-1 == m_dwIndex)
		return;

	if (FALSE == m_pNavMgr->IsAbleToUpStairs(m_dwIndex)) // 1->2층 갈 수 있는 내비메쉬인지
		return;

	m_dwIndex = m_pNavMgr->FindSecondFloor(m_pTransformCom->Get_Info(CTransform::INFO_POSITION), m_dwIndex);
}

void CPlayer::Player_Jump(const float& fTimeDelta)
{
	m_pPlayerInfo->tUnitInfo.fSpeedY = 2.f;
}

void CPlayer::Control_PlayerMove(const float& fTimeDelta)
{
	if (0.f != m_pPlayerInfo->tUnitInfo.fSpeedZ && 0.f == m_pPlayerInfo->tUnitInfo.fSpeedX)
		m_dwIndex = m_pTransformCom->Go_Straight(fTimeDelta * m_pPlayerInfo->tUnitInfo.fSpeedZ, m_dwIndex);
	if (0.f != m_pPlayerInfo->tUnitInfo.fSpeedX && 0.f == m_pPlayerInfo->tUnitInfo.fSpeedZ)
		m_dwIndex = m_pTransformCom->Go_Right(fTimeDelta * m_pPlayerInfo->tUnitInfo.fSpeedX, m_dwIndex);
	if (0.f != m_pPlayerInfo->tUnitInfo.fSpeedX && 0.f != m_pPlayerInfo->tUnitInfo.fSpeedZ)
	{
		m_dwIndex = m_pTransformCom->Go_Straight(fTimeDelta * m_pPlayerInfo->tUnitInfo.fSpeedZ * sqrtf(0.9f), m_dwIndex);
		m_dwIndex = m_pTransformCom->Go_Right(fTimeDelta * m_pPlayerInfo->tUnitInfo.fSpeedX * sqrtf(0.9f), m_dwIndex);
	}	
	
	if (3.f < m_pPlayerInfo->tUnitInfo.fSpeedX)
		m_pPlayerInfo->tUnitInfo.fSpeedX -= fTimeDelta * 200.f;
	else if (-3.f > m_pPlayerInfo->tUnitInfo.fSpeedX)
		m_pPlayerInfo->tUnitInfo.fSpeedX += fTimeDelta * 200.f;
	else
		m_pPlayerInfo->tUnitInfo.fSpeedX = 0.f;

	if (3.f < m_pPlayerInfo->tUnitInfo.fSpeedZ)
		m_pPlayerInfo->tUnitInfo.fSpeedZ -= fTimeDelta * 200.f;
	else if (-3.f > m_pPlayerInfo->tUnitInfo.fSpeedZ)
		m_pPlayerInfo->tUnitInfo.fSpeedZ += fTimeDelta * 200.f;
	else
		m_pPlayerInfo->tUnitInfo.fSpeedZ = 0.f;
}

void CPlayer::Observer_AddData(void)
{
	m_pInfoSubject->Add_Data(MESSAGE_PLAYER_INFO, m_pPlayerInfo);
}

void CPlayer::Observer_Update(void)
{
	m_pInfoSubject->Notify_Message(MESSAGE_PLAYER_INFO);
}

void CPlayer::Observer_RemoveData(void)
{
	m_pInfoSubject->Remove_Data(MESSAGE_PLAYER_INFO, m_pPlayerInfo);
}

void CPlayer::SendLocalData(void)
{
	Engine::NETDATA tData;
	ZeroMemory(&tData, sizeof(Engine::NETDATA));
	tData.chKey = 72;
	tData.eMessage = NETMSG_CLIENT_PLAYER_UPDATE;
	tData.iIndex = m_iClinetNum;
	tData.matWorld = *(m_pTransformCom->Get_WorldMatrix());
	tData.iHighAnimation = m_iHighRenderState;
	tData.iLowAnimation = m_iLowRenderState;
	tData.eMainState = m_pPlayerInfo->eMainState;
	tData.wSubMessage = m_eBuildTower;
	tData.vecPick = m_vecPick;

	CClientNetwork::GetInstance()->SendMsg(tData);
}

void CPlayer::Send_TowerData(_vec3 vecPos, DWORD dwOption)
{
	Engine::NETDATA tData;
	ZeroMemory(&tData, sizeof(Engine::NETDATA));
	tData.chKey = 72;
	tData.eMessage = NETMSG_CLINET_UPDATE_TOWER;
	tData.iIndex = m_iClinetNum;

	tData.wSubMessage = (WORD)dwOption; // 내비메쉬 옵션
	tData.vecPick = vecPos;

	CClientNetwork::GetInstance()->SendMsg(tData);
}

void CPlayer::Ready_Combat(void)
{
	Engine::NETDATA tData;
	ZeroMemory(&tData, sizeof(Engine::NETDATA));
	tData.chKey = 72;
	tData.eMessage = NETMSG_CLIENT_READY_COMBAT;
	tData.iIndex = m_iClinetNum;

	CClientNetwork::GetInstance()->SendMsg(tData);
}

void CPlayer::Leave_Player()
{
	m_bMyClient = false;
}

void CPlayer::Change_Player(int iAfterPlayer)
{
	cout << g_iClientIndex << "번 플레이어 ";

	m_bMyClient = true;
	g_iClientIndex = iAfterPlayer;

	cout << g_iClientIndex << "번으로 변경" << endl;;

	CCamera_Manager::GetInstance()->Change_Camera(CAMERA_STATIC, 0, 1.f, true);
}

void CPlayer::Set_TowerData(Engine::NETDATA tData)
{
	m_vecTowerPos = tData.vecPick;
	m_dwNavOption = tData.wSubMessage;
}

void CPlayer::Check_Collision(void)
{
	if (!m_bFirstFrame)
		return;

	if (PLAYER_DEAD == m_pPlayerInfo->eMainState ||
		TRUE == m_pPlayerInfo->tUnitInfo.bDead)
		return;

	// 평소 Hit되지 않았을 경우에는 색깔값이 원래 색깔.

	auto pCollisionlist = m_pCollisionManager->Find_Componentlist(L"Com_Col_MonWeapon");
	for (auto iter = pCollisionlist.begin(); iter != pCollisionlist.end(); ++iter)
	{
		if (m_pCollisionBody->Check_Collision((CCollisionSphere*)(*iter)) && !m_pCollisionBody->Get_HitCheck())
		{
			
			m_pPlayerInfo->tUnitInfo.iHp -= ((CCollisionSphere*)(*iter))->Get_Damage();

			// 데미지 폰트
			Engine::CGameObject* pGameObject = CFont_PlayerDamage::Create(m_pGraphicDev, ((CCollisionSphere*)(*iter))->Get_Damage());
			pGameObject->Set_Pos(&(m_pTransformCom->Get_Info(CTransform::INFO_POSITION) + _vec3(0.f, 22.f, 0.f)));
			CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", pGameObject);

			if (0 >= m_pPlayerInfo->tUnitInfo.iHp) // 죽었을때 
			{
				m_pPlayerInfo->eMainState = PLAYER_DEAD;
				m_pPlayerInfo->tUnitInfo.iHp = 0;
				if (m_bMyClient)
				{
					Hit_UI(3.f);
					CCamera_Manager::GetInstance()->Shake_Camera(2000.f, 0.8f, 6); // 강도, 시간, 횟수
				}
				Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_PlayerDeadWhiteBlood"
					, m_pDynamicMeshCom->Get_FrameMatrix("b_Root")
					, m_pTransformCom->Get_WorldMatrix());

				_vec3 vecEye = CDataManager::GetInstance()->Get_CamPoistion();
				float fDistance = D3DXVec3Length(&(vecEye - m_pTransformCom->Get_Info(CTransform::INFO_POSITION)));
				if (fDistance < 300.f)
				{
					switch (m_pPlayerInfo->ePlayerType)
					{
					case PLAYER_MAGE:
						CSoundMgr::GetInstance()->Play_Sound(L"Apprentice_yell.ogg", CHANNEL_MAGE, 0.9f);
						break;
					case PLAYER_MONK:
						CSoundMgr::GetInstance()->Play_Sound(L"Monk_death2.ogg", CHANNEL_MONK, 0.9f);
						break;
					}
				}
			}
			else if (PLAYER_SKILL != m_pPlayerInfo->eMainState &&
				PLAYER_TOWER_CANCEL != m_pPlayerInfo->eMainState &&
				PLAYER_TOWER_PREVIEW != m_pPlayerInfo->eMainState &&
				PLAYER_TOWER_PREVIEW_SECOND != m_pPlayerInfo->eMainState &&
				PLAYER_TOWER_SUMMONING != m_pPlayerInfo->eMainState &&
				PLAYER_ATTACK_FOURTH != m_pPlayerInfo->eMainState &&
				PLAYER_JUMP_ATTACK_FOURTH != m_pPlayerInfo->eMainState &&
				PLAYER_JUMP_FALLING_ATTACK_FOURTH != m_pPlayerInfo->eMainState &&
				PLAYER_SKILL_ATTACK != m_pPlayerInfo->eMainState) // Hit 됐을때 
			{

				m_pPlayerInfo->eMainState = PLAYER_HURT;
				if (m_bMyClient)
				{
					Hit_UI(0.3f);
					CCamera_Manager::GetInstance()->Shake_Camera(600.f, 0.3f, 1); // 강도, 시간, 횟수
				}
				if (m_pPlayerInfo->ePlayerType == PLAYER_MONK && m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"Monk_hurt1_2.ogg", Engine::CHANNEL_MONK, 0.7f);
				else if(m_pPlayerInfo->ePlayerType == PLAYER_MAGE && m_bMyClient)
					CSoundMgr::GetInstance()->Play_Sound(L"Monk_hurt2.ogg", Engine::CHANNEL_MAGE, 0.7f);
				

				// Effect--
				int iRand = Random(3);
				_matrix* matWorld = m_pTransformCom->Get_WorldMatrix();

				switch (iRand)
				{
				case 0:
					Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_PlayerWhiteBlood1"
						, m_pDynamicMeshCom->Get_FrameMatrix("b_Root")
						, m_pTransformCom->Get_WorldMatrix());
					break;
				case 1:
					Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_PlayerWhiteBlood2"
						, m_pDynamicMeshCom->Get_FrameMatrix("b_Root")
						, m_pTransformCom->Get_WorldMatrix());				
					break;
				case 2:
					Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_PlayerWhiteBlood3"
						, m_pDynamicMeshCom->Get_FrameMatrix("b_Root")
						, m_pTransformCom->Get_WorldMatrix());				
					break;
				}
			}

			// Hit됐을때 컬러값 변경.
			m_bColorAdd = true;
			m_fColorTime = 0.f;

			m_pCollisionBody->Set_HitCheck(true);

	}



	}
	auto pCollisionlist2 = m_pCollisionManager->Find_Componentlist(L"Com_Col_HealingAura");
	for (auto iter = pCollisionlist2.begin(); iter != pCollisionlist2.end(); ++iter)
	{
		if (m_pCollisionBody->Check_AuraCollision((CCollisionSphere*)(*iter)))
		{
			m_fHealingCount += m_fTimeDelta;
			if (m_fHealingCount >= 1.f)
			{
				m_fHealingCount = 0.f;
				
				m_pPlayerInfo->tUnitInfo.iHp += ((CCollisionSphere*)(*iter))->Get_Damage();
				if (m_pPlayerInfo->tUnitInfo.iHpMax < m_pPlayerInfo->tUnitInfo.iHp)
					m_pPlayerInfo->tUnitInfo.iHp = m_pPlayerInfo->tUnitInfo.iHpMax;
				((CCollisionSphere*)(*iter))->Set_Life(1.f);

				if(m_pHealingEffect.empty())
					m_pHealingEffect =	Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_HealingAura_Effected"
						, m_pDynamicMeshCom->Get_FrameMatrix("b_Root")
						, m_pTransformCom->Get_WorldMatrix());

			}
		}
		else // 충돌이 안되었을때
		{
			if (m_pHealingEffect.size() > 0)
			{
				list<CEffect*>::iterator iter_begin = m_pHealingEffect.begin();
				list<CEffect*>::iterator iter_end = m_pHealingEffect.end();

				for (; iter_begin != iter_end; ++iter_begin)
				{
					(*iter_begin)->Set_Dead();
					//(*iter_begin)->Set_InstantFadeOut(2.f);
				}
				m_pHealingEffect.clear();
			}
		}
	}
}

void CPlayer::Move_MouseAngle(void)
{
	int iDistance = 0;
	if (iDistance = m_pInputDev->GetDIMouseMove(Engine::CInputDev::DIM_Y))
	{
		if (m_pPlayerInfo->fCameraAngle + iDistance / 20.f <= 30.f && // 각도 제한
			m_pPlayerInfo->fCameraAngle + iDistance / 20.f >= -15.f)
			m_pPlayerInfo->fCameraAngle += iDistance / 20.f;
	}
}

void CPlayer::Healing(void)
{
	if (m_pPlayerInfo->eMainState == PLAYER_HEAL)
	{

		if (m_pOwn_HealingEffect.size() <= 0)
			m_pOwn_HealingEffect = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_Own_Healing", m_pDynamicMeshCom->Get_FrameMatrix("b_Root"), m_pTransformCom->Get_WorldMatrix());

		m_fHealingHp += m_fTimeDelta * 100.f;
		m_fHealingMp += m_fTimeDelta * 10.f;
		
		if (m_fHealingHp >= 1.f)
		{
			++m_pPlayerInfo->tUnitInfo.iHp;
			m_fHealingHp = 0.f;
		}
		if (m_fHealingMp >= 1.f)
		{
			--m_pPlayerInfo->tUnitInfo.iMp;
			m_fHealingMp = 0.f;
		}

		if (m_pPlayerInfo->tUnitInfo.iHp >= m_pPlayerInfo->tUnitInfo.iHpMax)
		{
			m_pPlayerInfo->tUnitInfo.iHp = m_pPlayerInfo->tUnitInfo.iHpMax;
			m_pPlayerInfo->eMainState = PLAYER_NOT_INPUT;

			if (m_pOwn_HealingEffect.size() > 0)
			{
				_uint iSize = 0;

				list<CEffect*>::iterator iter_begin = m_pOwn_HealingEffect.begin();
				list<CEffect*>::iterator iter_end = m_pOwn_HealingEffect.end();
				for (; iter_begin != iter_end; ++iter_begin)
				{
					if (iSize == 0)
					{
						++iSize;
						dynamic_cast<CEffect_Particle*>((*iter_begin))->Set_LoopFalse();
						continue;
					}
					(*iter_begin)->Set_InstantFadeOut(1.f);
					++iSize;
				}
				m_pOwn_HealingEffect.clear();
			}

		}
		else if(0 >= m_pPlayerInfo->tUnitInfo.iMp)
		{
			m_pPlayerInfo->tUnitInfo.iMp = 0;
			m_pPlayerInfo->eMainState = PLAYER_NOT_INPUT;

			if (m_pOwn_HealingEffect.size() > 0)
			{
				_uint iSize = 0;

				list<CEffect*>::iterator iter_begin = m_pOwn_HealingEffect.begin();
				list<CEffect*>::iterator iter_end = m_pOwn_HealingEffect.end();
				for (; iter_begin != iter_end; ++iter_begin)
				{
					if (iSize == 0)
					{
						++iSize;
						dynamic_cast<CEffect_Particle*>((*iter_begin))->Set_LoopFalse();
						continue;
					}
					(*iter_begin)->Set_InstantFadeOut(1.f);
					++iSize;
				}
				m_pOwn_HealingEffect.clear();
			}
		}
	}
}

void CPlayer::Hit_UI(const _float & fExistTime)
{
	Engine::CGameObject* pGameObject = CLowHp::Create(m_pGraphicDev, fExistTime);
	if (nullptr == pGameObject)
		return;

	CObject_Manager::GetInstance()->Add_GameObject(g_eCurrentScene, L"Layer_UI", pGameObject);
}

void CPlayer::Mouse_Pick(void)
{
	_vec3 vecPick;

	if (FALSE == m_pCollisionManager->Pick_MouseCol(vecPick))
		return;

	CMyEffect* pMyEffect = CMyEffect::Create(m_pGraphicDev);
	if (nullptr == pMyEffect)
		return;

	pMyEffect->Set_Pos(&(vecPick + _vec3(0.f, 0.0f, 0.f)));

	// 임시 이펙트
	CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pMyEffect);
}

void CPlayer::Preview_Tower(const float& fTimeDelta) // 최초 위치 잡기
{
	if (PLAYER_TOWER_PREVIEW != m_pPlayerInfo->eMainState && PLAYER_TOWER_PREVIEW_SECOND != m_pPlayerInfo->eMainState)
		return;

	if (nullptr == m_pPreviewTower && nullptr == m_pTowerRadius && nullptr == m_pTowerRange) // 타워 + 반지름
	{
		m_fSummoningTime = 0.f;
		if (-1.f == m_fWaitingTime)
			m_fWaitingTime = 1 / 1.8f;

		if (m_bMyClient && m_pCamMgr->Get_CamType() != CAMERA_TOWER)
		{
			m_pCamMgr->Change_Camera(CAMERA_TOWER, 0, 1.8f); // 시점 변환
			m_pMousePointer->Set_ShowMousePointer(false);
		}
			
		if (FALSE == m_pCamMgr->Get_IsCamAttach()) // 카메라가 완전히 붙은 다음에 타워를 생성한다.
			return;

		if (m_bMyClient)
		{
			m_pMousePointer->Change_MousePointer(CMouse_Manager::POINTER_BUILD); // 마우스 변경
			m_pMousePointer->Set_ShowMousePointer(true);
		}
		else // 다른 플레이어가 CamAttach되고 타워를 생성하기를 기다려주자
		{
			m_fWaitingTime -= fTimeDelta;
			if (m_fWaitingTime > 0.f)
				return;
			else
				m_fWaitingTime = -1.f;
		}

		// --------------------------- Test
		switch (m_pPlayerInfo->ePlayerType)
		{
		case PLAYER_MONK:
			cout << m_iClinetNum << "번 Monk : ";
			break;
		case PLAYER_MAGE:
			cout << m_iClinetNum << "번 Mage : ";
			break;
		}
		cout << "Preview" << endl;
		// --------------------------- Test
			
		Create_PreviewTower(m_eBuildTower); // CTower::Create

		Create_TowerUI(); // Tower Radius, Range 등
	}

	_vec3 vecPick, vecDir;
	DWORD dwOption = 0;

	if (m_bMyClient) // 내 클라이언트면 피킹을 통해 타워의 위치를 정한다
	{
		dwOption = m_pCollisionManager->Pick_NavMesh(vecPick); // 내비메쉬를 피킹

		if (0 == dwOption) // 피킹 실패
			return;

		Send_TowerData(vecPick, dwOption);
	}
	else // 아니라면 서버에서 보내준 정보로 이동시킨다
	{
		vecPick = m_vecTowerPos;
		dwOption = m_dwNavOption;
	}

	if (PLAYER_TOWER_PREVIEW == m_pPlayerInfo->eMainState) // 첫 번째 프리뷰 (타워 위치 잡기)
	{
		vecDir = vecPick - m_pTransformCom->Get_Info(CTransform::INFO_POSITION);

		float fAngle = -atan2f(vecDir.z, vecDir.x) + D3DXToRadian(90.f);

		m_pPreviewTower->Set_Pos(&vecPick); // 타워 위치가 마우스를 따라다니게
		m_pPreviewTower->Set_Dir(&_vec3(0.f, fAngle, 0.f)); // 타워 방향
		m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, (&_vec3(0.f, fAngle, 0.f))); // 플레이어 방향

		if (nullptr != m_pTowerRange)
		{
			m_pTowerRange->Set_Pos(&(vecPick + _vec3(0.f, 2.f, 0.f)));
			m_pTowerRange->Set_Dir(&_vec3(D3DXToRadian(90.f), fAngle, 0.f));
			m_pTowerRange2->Set_Pos(&(vecPick + _vec3(0.f, 2.1f, 0.f)));
			m_pTowerRange2->Set_Dir(&_vec3(D3DXToRadian(90.f), fAngle, 0.f));
		}
		AbleToBuildTower(dwOption); // 타워를 지을 수 있는지 없는지(에 따른 Text UI, Color 등 변경)
	}
	else if (PLAYER_TOWER_PREVIEW_SECOND == m_pPlayerInfo->eMainState) // 두 번째 프리뷰 (타워 방향 잡기)
	{
		vecDir = vecPick - m_pPreviewTower->Get_Pos();
		float fAngle = -atan2f(vecDir.z, vecDir.x) + D3DXToRadian(90.f);

		m_pPreviewTower->Set_Dir(&_vec3(0.f, fAngle, 0.f)); // 타워 방향

		if (nullptr != m_pTowerRange)
		{
			m_pTowerRange->Set_Dir(&_vec3(D3DXToRadian(90.f), fAngle, 0.f));
			m_pTowerRange2->Set_Dir(&_vec3(D3DXToRadian(90.f), fAngle, 0.f));
		}
	}
}

void CPlayer::Preview_Tower2(void)
{
	if (PLAYER_TOWER_PREVIEW_SECOND != m_pPlayerInfo->eMainState)
		return;

	if (nullptr == m_pTowerRange || nullptr == m_pPreviewTower)
		return;

	if (_vec4(1.f, 1.f, 0.f, 1.f) == m_pTowerRange->Get_Color()) // 컬러 색으로 Preview2 상태일 때 한 번만 들어오게 한다
		return;

	// --------------------------- Test
	switch (m_pPlayerInfo->ePlayerType)
	{
	case PLAYER_MONK:
		cout << m_iClinetNum << "번 Monk : ";
		break;
	case PLAYER_MAGE:
		cout << m_iClinetNum << "번 Mage : ";
		break;
	}
	cout << "Preview2" << endl;
	// --------------------------- Test

	if (m_bMyClient)
	{
		m_pCamMgr->Fix_MouseCursor(false);
		m_pMousePointer->Change_MousePointer(CMouse_Manager::POINTER_ARROW);

		POINT PtTower = WorldToScreen(m_pPreviewTower->Get_Pos());
		POINT PtPlayer = WorldToScreen(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));
		POINT PtDir = { PtTower.x - PtPlayer.x, PtTower.y - PtPlayer.y };
		WorldToScreen(m_pTransformCom->Get_Info(CTransform::INFO_POSITION));
		m_pMousePointer->Set_TowerPosition(CMouse_Manager::GetInstance()->GetMousePos(), PtDir); // 타워의 위치와 방향을 보내서 마우스 커서 위치도 바꿔주자
		m_pMousePointer->Change_Color(_vec4(1.f, 1.f, 0.f, 1.f));
	}
	
	if (nullptr != m_pTowerRange)
	{
		m_pTowerRange->Change_Color(_vec4(1.f, 1.f, 0.f, 1.f));
		m_pTowerRange2->Change_Color(_vec4(1.f, 1.f, 0.f, 1.f));
	}
}

void CPlayer::Create_PreviewTower(TOWER_TYPE eType)
{
	switch (eType)
	{
	// EffectMgr에 넣는 경우엔 Add_Ref를 호출하지 말자
	case TOWER_TYPE_SLOWAURA:
	case TOWER_TYPE_LIGHTNINGAURA:
	case TOWER_TYPE_HEALINGAURA:
	case TOWER_TYPE_DRAINAURA:
	case TOWER_TYPE_ENRAGEAURA:
		m_pPreviewTower = CEffect_Mesh_Shield::Create(m_pGraphicDev);
		dynamic_cast<CEffect*>(m_pPreviewTower)->Set_Information(Engine::CTransform::INFORMATION::INFO_SCALE, &_vec3(0.4f, 0.4f, 0.4f));
		dynamic_cast<CEffect*>(m_pPreviewTower)->Set_EffectColor(_vec4(0.1f, 0.3f, 0.1f, 0.f));
		dynamic_cast<CEffect_Mesh*>(m_pPreviewTower)->Set_UV(_vec2(5.5f, 5.5f)/*_vec2(20.f, 20.f)*/);
		dynamic_cast<CEffect_Mesh*>(m_pPreviewTower)->Set_UVAnimation(_vec2(0.25f, 0.f), 0.1f);
		Engine::CEffectMgr::GetInstance()->Add_Effect(dynamic_cast<CEffect*>(m_pPreviewTower));
		break;
	case TOWER_TYPE_FIRE:
		m_pPreviewTower = CFireTower::Create(m_pGraphicDev);
		CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", m_pPreviewTower);
		m_pPreviewTower->Add_Ref();
		break;
	case TOWER_TYPE_MISSILE:
		m_pPreviewTower = CMissileTower::Create(m_pGraphicDev);
		CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", m_pPreviewTower);
		m_pPreviewTower->Add_Ref();
		break;
	case TOWER_TYPE_STRIKER:
		m_pPreviewTower = CStrikerTower::Create(m_pGraphicDev);
		CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", m_pPreviewTower);
		m_pPreviewTower->Add_Ref();
		break;
	case TOWER_TYPE_LIGHTNING:
		m_pPreviewTower = CLightningTower::Create(m_pGraphicDev);
		CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", m_pPreviewTower);
		m_pPreviewTower->Add_Ref();
		break;
	case TOWER_TYPE_BLOCKADE:
		m_pPreviewTower = CBlockade::Create(m_pGraphicDev);
		CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", m_pPreviewTower);
		m_pPreviewTower->Add_Ref();
		break;
	}
}

void CPlayer::Create_TowerUI(void)
{
	m_pTowerRadius = CTowerRadius::Create(m_pGraphicDev); // 플레이어 주위에 원처럼 생기는거
	CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", m_pTowerRadius);
	m_pTowerRadius->Set_Pos(&(m_pTransformCom->Get_Info(CTransform::INFO_POSITION) + _vec3(0.f, 3.f, 0.f)));
	m_pTowerRadius->Add_Ref();

	int iTowerRangeType = -1;
	switch (m_eBuildTower)
	{
	case TOWER_TYPE_SLOWAURA:
	case TOWER_TYPE_HEALINGAURA:
	case TOWER_TYPE_LIGHTNINGAURA:
	case TOWER_TYPE_DRAINAURA:
	case TOWER_TYPE_ENRAGEAURA:
		iTowerRangeType = -1;
		break;
	case TOWER_TYPE_FIRE:
	case TOWER_TYPE_MISSILE:
	case TOWER_TYPE_STRIKER:
		iTowerRangeType = 2;
		break;
	case TOWER_TYPE_BLOCKADE:
		iTowerRangeType = 3;
		break;
	case TOWER_TYPE_LIGHTNING:
		iTowerRangeType = 4;
		break;
	}

	if (-1 == iTowerRangeType)
		return;

	m_pTowerRange = CTowerRange::Create(m_pGraphicDev, iTowerRangeType); // 타워 범위(부채꼴)
	CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", m_pTowerRange);
	m_pTowerRange->Add_Ref();
	m_pTowerRange2 = CTowerRange2::Create(m_pGraphicDev, iTowerRangeType); // 타워 범위(화살표)
	CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_UI", m_pTowerRange2);
	m_pTowerRange2->Add_Ref();
}

void CPlayer::Change_TowerColor(_vec4& vecColor)
{
	switch (m_eBuildTower)
	{
	case TOWER_TYPE_SLOWAURA:
	case TOWER_TYPE_LIGHTNINGAURA:
	case TOWER_TYPE_HEALINGAURA:
	case TOWER_TYPE_DRAINAURA:
	case TOWER_TYPE_ENRAGEAURA:
		dynamic_cast<CEffect*>(m_pPreviewTower)->Set_EffectColor(vecColor);
		break;
	case TOWER_TYPE_FIRE:
	case TOWER_TYPE_MISSILE:
	case TOWER_TYPE_STRIKER:
	case TOWER_TYPE_LIGHTNING:
	case TOWER_TYPE_BLOCKADE:
		dynamic_cast<CTower*>(m_pPreviewTower)->SetColor(vecColor);
		break;
	}
}

void CPlayer::AbleToBuildTower(const DWORD dwOption)
{
	if (dwOption & Engine::NAV_OPTION_TOWER_ENABLE) // 타워 짓기 가능
	{
		m_bAbleToBuild = true;

		// 색 변경
		Change_TowerColor(_vec4(0.4f, 1.f, 0.4f, 1.f));
		m_pMousePointer->Change_Color(_vec4(0.f, 1.f, 0.f, 1.f)); 
		if (nullptr != m_pTowerRange)
		{
			m_pTowerRange->Change_Color(_vec4(0.f, 1.f, 0.f, 1.f));
			m_pTowerRange2->Change_Color(_vec4(0.f, 1.f, 0.f, 1.f));
		}

		if (m_bMyClient)
		{
			if (nullptr == m_pText_Tower) // 텍스트 변경 혹은 유지
			{
				m_pText_Tower = CText_Tower::Create(m_pGraphicDev, 0);
				CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", m_pText_Tower);
				m_pText_Tower->Add_Ref();
			}
			else
			{
				if (0 != m_pText_Tower->Get_TextureNum()) // 다른 상태로 바뀐다
				{
					m_pText_Tower->Set_Dead();
					Engine::Safe_Release(m_pText_Tower);

					m_pText_Tower = CText_Tower::Create(m_pGraphicDev, 0);
					CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", m_pText_Tower);
					m_pText_Tower->Add_Ref();
				}
			}
		}
	}
	else // 타워 짓기 불가능
	{
		m_bAbleToBuild = false;

		// 색 변경
		Change_TowerColor(_vec4(1.f, 0.4f, 0.4f, 1.f));
		m_pMousePointer->Change_Color(_vec4(1.f, 0.f, 0.f, 1.f));
		if (nullptr != m_pTowerRange)
		{
			m_pTowerRange->Change_Color(_vec4(1.f, 0.f, 0.f, 1.f));
			m_pTowerRange2->Change_Color(_vec4(1.f, 0.f, 0.f, 1.f));
		}

		if (m_bMyClient)
		{
			if (nullptr == m_pText_Tower) // 텍스트 변경 혹은 유지
			{
				m_pText_Tower = CText_Tower::Create(m_pGraphicDev, 1);
				CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", m_pText_Tower);
				m_pText_Tower->Add_Ref();
			}
			else
			{
				if (1 != m_pText_Tower->Get_TextureNum()) // 다른 상태로 바뀐다
				{
					m_pText_Tower->Set_Dead();
					Engine::Safe_Release(m_pText_Tower);

					m_pText_Tower = CText_Tower::Create(m_pGraphicDev, 1);
					CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", m_pText_Tower);
					m_pText_Tower->Add_Ref();
				}
			}
		}
	}
}

void CPlayer::Cancel_Tower(void)
{
	if (PLAYER_TOWER_CANCEL != m_pPlayerInfo->eMainState)
		return;

	if (m_pPreviewTower == nullptr)
		return;

	// --------------------------- Test
	switch (m_pPlayerInfo->ePlayerType)
	{
	case PLAYER_MONK:
		cout << m_iClinetNum << "번 Monk : ";
		break;
	case PLAYER_MAGE:
		cout << m_iClinetNum << "번 Mage : ";
		break;
	}
	cout << "Cancel" << endl;
	// --------------------------- Test

	if (m_bMyClient)
	{
		_matrix matViewInv = CDataManager::GetInstance()->Get_ViewInverseMatrix();
		_vec3 vecDir;
		memcpy(&vecDir, matViewInv.m[2], sizeof(_vec3));
		m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, (&_vec3(0.f, -atan2f(vecDir.z, vecDir.x) + D3DXToRadian(90.f), 0.f)), true); // 플레이어 방향
		 
		m_pCamMgr->Change_Camera(CAMERA_STATIC, 0, 1.5f);
		m_pMousePointer->Change_MousePointer(CMouse_Manager::POINTER_AIM); // 마우스 커서 변경
		m_pMousePointer->Set_ShowMousePointer(true);
	}

	if(nullptr != m_pPreviewTower)
		m_pPreviewTower->Set_Dead();

	if (m_eBuildTower != TOWER_TYPE_SLOWAURA &&
		m_eBuildTower != TOWER_TYPE_LIGHTNINGAURA &&
		m_eBuildTower != TOWER_TYPE_HEALINGAURA &&
		m_eBuildTower != TOWER_TYPE_DRAINAURA &&
		m_eBuildTower != TOWER_TYPE_ENRAGEAURA)	// Aura(-Effect)경우 bDead시 EffectMgr에서 Safe_Release돔
		Engine::Safe_Release(m_pPreviewTower);

	m_pPreviewTower = nullptr;

	Delete_TowerUI();

	// 소환 이펙트 삭제
	list<CEffect*>::iterator iter_begin = m_pSummoingTowerEffect.begin();
	list<CEffect*>::iterator iter_end = m_pSummoingTowerEffect.end();
	for (; iter_begin != iter_end; ++iter_begin)
	{
		(*iter_begin)->Set_Dead();
	}
	m_pSummoingTowerEffect.clear();
}

void CPlayer::Summoning_Tower(const float& fTimeDelta)
{
	if (PLAYER_TOWER_SUMMONING != m_pPlayerInfo->eMainState)
		return;

	if (nullptr == m_pPreviewTower)
		return;

	_vec3 vPos = m_pPreviewTower->Get_Pos();

	if (nullptr != m_pTowerRadius) // 파티클 이펙트를 한 번만 생성
	{
		if(m_pSummoingTowerEffect.size() <= 0)
			m_pSummoingTowerEffect = Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_SummoingTower", &_vec3(vPos.x, vPos.y + 15.f, vPos.z));

		Delete_TowerUI();

		if (m_bMyClient) // 시점 변환
		{
			m_pCamMgr->Change_Camera(CAMERA_SUMMON);
			m_pMousePointer->Set_ShowMousePointer(false); // 마우스 포인터 변환
		}

		if (TOWER_TYPE_FIRE == m_eBuildTower
			|| TOWER_TYPE_MISSILE == m_eBuildTower
			|| TOWER_TYPE_STRIKER == m_eBuildTower
			|| TOWER_TYPE_LIGHTNING == m_eBuildTower
			|| TOWER_TYPE_BLOCKADE == m_eBuildTower)
		{
			//cout << "Maze Set_Summon" << endl;
			dynamic_cast<CTower*>(m_pPreviewTower)->Set_Summon(true);
			dynamic_cast<CTower*>(m_pPreviewTower)->SetColor(_vec4(0.f, 0.f, 0.f, 0.f));
		}

		// --------------------------- Test
		switch (m_pPlayerInfo->ePlayerType)
		{
		case PLAYER_MONK:
			cout << m_iClinetNum << "번 Monk : ";
			break;
		case PLAYER_MAGE:
			cout << m_iClinetNum << "번 Mage : ";
			break;
		}
		cout << "Summon" << endl;
		// --------------------------- Test
	}

	if (m_bMyClient)
	{
		m_fSummoningTime += fTimeDelta;
		if (m_fSummoningTime > 2.f) // ToDo : 상수값 바꿔주자
			m_pPlayerInfo->eMainState = PLAYER_TOWER_BUILD;
	}
}

void CPlayer::Build_Tower(void)
{
	if (m_pPlayerInfo->eMainState != PLAYER_TOWER_BUILD)
		return;

	if (nullptr == m_pPreviewTower)
		return;

	//m_pPlayerInfo->eMainState = PLAYER_TOWER_CANCEL;

	Change_TowerColor(_vec4(1.f, 1.f, 1.f, 1.f)); // 원래 색상으로 변경

	if (TOWER_TYPE_FIRE == m_eBuildTower
		|| TOWER_TYPE_MISSILE == m_eBuildTower
		|| TOWER_TYPE_STRIKER == m_eBuildTower
		|| TOWER_TYPE_LIGHTNING == m_eBuildTower
		|| TOWER_TYPE_BLOCKADE == m_eBuildTower)
	{
		dynamic_cast<CTower*>(m_pPreviewTower)->Set_Summon(false);
	}

	if (m_bMyClient)
	{
		_matrix matViewInv = CDataManager::GetInstance()->Get_ViewInverseMatrix();
		_vec3 vecDir;
		memcpy(&vecDir, matViewInv.m[2], sizeof(_vec3));
		m_pTransformCom->Set_Information(CTransform::INFO_ANGLE, (&_vec3(0.f, -atan2f(vecDir.z, vecDir.x) + D3DXToRadian(90.f), 0.f)), true); // 플레이어 방향

		m_pCamMgr->Change_Camera(CAMERA_STATIC, 0, 1.5f); // 시점 변환

		m_pMousePointer->Set_ShowMousePointer(true); 
		m_pMousePointer->Change_MousePointer(CMouse_Manager::POINTER_AIM); // 마우스 커서 변경
	}

	// --------------------------- Test
	switch (m_pPlayerInfo->ePlayerType)
	{
	case PLAYER_MONK:
		cout << m_iClinetNum << "번 Monk : ";
		break;
	case PLAYER_MAGE:
		cout << m_iClinetNum << "번 Mage : ";
		break;
	}
	cout << "Build" << endl;
	// --------------------------- Test
	
	// 이펙트 및 Preview Tower 제거
	if (nullptr != m_pPreviewTower)
	{
		// 소환이 다 되고나면, 소환 이펙트를 지워준다.
		list<CEffect*>::iterator iter_begin = m_pSummoingTowerEffect.begin();
		list<CEffect*>::iterator iter_end = m_pSummoingTowerEffect.end();
		for (; iter_begin != iter_end; ++iter_begin)
		{
			(*iter_begin)->Set_Dead();
		}
		m_pSummoingTowerEffect.clear();

		_vec3 vPos = m_pPreviewTower->Get_Pos();

		if (m_eBuildTower == TOWER_TYPE_FIRE) 
		{
			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_BuildTower", &vPos);
			dynamic_cast<CTower*>(m_pPreviewTower)->Set_TowerEffect(m_eBuildTower);
			Engine::Safe_Release(m_pPreviewTower);
			m_pPlayerInfo->tUnitInfo.iMp -= 80;
		}
		else if (m_eBuildTower == TOWER_TYPE_MISSILE)
		{
			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_BuildTower", &vPos);
			dynamic_cast<CTower*>(m_pPreviewTower)->Set_TowerEffect(m_eBuildTower);
			Engine::Safe_Release(m_pPreviewTower);
			m_pPlayerInfo->tUnitInfo.iMp -= 40;
		}
		else if (m_eBuildTower == TOWER_TYPE_STRIKER)
		{
			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_BuildTower", &vPos);
			dynamic_cast<CTower*>(m_pPreviewTower)->Set_TowerEffect(m_eBuildTower);
			Engine::Safe_Release(m_pPreviewTower);
			m_pPlayerInfo->tUnitInfo.iMp -= 150;
		}
		else if (m_eBuildTower == TOWER_TYPE_LIGHTNING)
		{
			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_BuildTower", &vPos);
			dynamic_cast<CTower*>(m_pPreviewTower)->Set_TowerEffect(m_eBuildTower);
			Engine::Safe_Release(m_pPreviewTower);
			m_pPlayerInfo->tUnitInfo.iMp -= 120;
		}
		else if (m_eBuildTower == TOWER_TYPE_BLOCKADE)
		{
			Engine::CEffectMgr::GetInstance()->Add_Effect(L"Effect_BuildTower", &vPos);
			Engine::Safe_Release(m_pPreviewTower);
			m_pPlayerInfo->tUnitInfo.iMp -= 20;
		}
		else if (m_eBuildTower == TOWER_TYPE_SLOWAURA)
		{
			m_pPreviewTower->Set_Dead();
			CGameObject*  pAura = CAura::Create(m_pGraphicDev, vPos, TOWER_TYPE_SLOWAURA);
			CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pAura);
			m_pPlayerInfo->tUnitInfo.iMp -= 30;
		}
		else if (m_eBuildTower == TOWER_TYPE_LIGHTNINGAURA)
		{
			m_pPreviewTower->Set_Dead();
			CGameObject*  pAura = CAura::Create(m_pGraphicDev, vPos, TOWER_TYPE_LIGHTNINGAURA);
			CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pAura);
			m_pPlayerInfo->tUnitInfo.iMp -= 50;
		}
		else if (m_eBuildTower == TOWER_TYPE_HEALINGAURA)
		{
			m_pPreviewTower->Set_Dead();
			CGameObject*  pAura = CAura::Create(m_pGraphicDev, vPos, TOWER_TYPE_HEALINGAURA);
			CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pAura);
			m_pPlayerInfo->tUnitInfo.iMp -= 40;
		}
		else if (m_eBuildTower == TOWER_TYPE_DRAINAURA)
		{
			m_pPreviewTower->Set_Dead();
			CGameObject*  pAura = CAura::Create(m_pGraphicDev, vPos, TOWER_TYPE_DRAINAURA);
			CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pAura);
			m_pPlayerInfo->tUnitInfo.iMp -= 60;
		}
		else if (m_eBuildTower == TOWER_TYPE_ENRAGEAURA)
		{
			m_pPreviewTower->Set_Dead();
			CGameObject*  pAura = CAura::Create(m_pGraphicDev, vPos, TOWER_TYPE_ENRAGEAURA);
			CObject_Manager::GetInstance()->Add_GameObject(SCENE_STAGE, L"Layer_GameObject", pAura);
			m_pPlayerInfo->tUnitInfo.iMp -= 100;
		}

		m_pPreviewTower = nullptr;
	}
}

void CPlayer::Delete_TowerUI(void)
{
	if (nullptr != m_pTowerRadius)
		m_pTowerRadius->Set_Dead();
	Engine::Safe_Release(m_pTowerRadius);
	m_pTowerRadius = nullptr;

	if (nullptr != m_pTowerRange)
		m_pTowerRange->Set_Dead();
	Engine::Safe_Release(m_pTowerRange);
	m_pTowerRange = nullptr;

	if (nullptr != m_pTowerRange2)
		m_pTowerRange2->Set_Dead();
	Engine::Safe_Release(m_pTowerRange2);
	m_pTowerRange2 = nullptr;

	if (nullptr != m_pText_Tower)
		m_pText_Tower->Set_Dead();
	Engine::Safe_Release(m_pText_Tower);
	m_pText_Tower = nullptr;
}

POINT CPlayer::WorldToScreen(_vec3 vecPos)
{
	_matrix matView, matProj;
	
	matView = CDataManager::GetInstance()->Get_ViewMatrix();
	matProj = CDataManager::GetInstance()->Get_ProjMatrix();

	D3DXVec3TransformCoord(&vecPos, &vecPos, &matView);
	D3DXVec3TransformCoord(&vecPos, &vecPos, &matProj);

	POINT ScreenPoint;

	ScreenPoint.x = _long((vecPos.x + 1) * WINCX) / 2;
	ScreenPoint.y = _long((-vecPos.y + 1) * WINCY) / 2;

	return ScreenPoint;
}

void CPlayer::Free(void)
{
	Engine::Safe_Release(m_pPreviewTower);
	Engine::Safe_Release(m_pMousePointer);

	Delete_TowerUI();

	Engine::Safe_Release(m_pInputDev);
	Engine::Safe_Release(m_pInfoSubject);
	Engine::Safe_Release(m_pNavMgr);
	Engine::Safe_Release(m_pCamMgr);
	Engine::Safe_Release(m_pWeapon);
	Engine::Safe_Release(m_pCollisionBody);
	Engine::Safe_Release(m_pCollisionManager);
	CDynamicObject::Free();
}