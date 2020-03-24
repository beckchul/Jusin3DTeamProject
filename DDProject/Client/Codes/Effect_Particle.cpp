#include "stdafx.h"
#include "Effect_Particle.h"

DWORD FtoDW(float f) { return *((DWORD*)&f); }


CEffect_Particle::CEffect_Particle(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CEffect(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pVb(nullptr)
	, m_fDelayCount(0.f)
	, m_dwVbSize(0)
	, m_dwVbOffset(0)
	, m_dwVbBatchSize(0)
	, m_fGravityStartTime(0.f)
	, m_eSceneID(SCENE_END)
	, m_bFireWorkYPositive(FALSE)
	, m_bDestBlendInv(FALSE)
	,m_bFollowTrackingPath(FALSE)
	, m_vFadeInfo(1.5f, 1.f)
	, m_bUseFadeColor(FALSE)
	, m_vFadeColor(0.f, 0.f, 0.f)
{
}

CEffect_Particle::CEffect_Particle(const CEffect_Particle& rhs)
	: Engine::CEffect(rhs)
	, m_pRendererCom(rhs.m_pRendererCom)
	, m_pTextureCom(rhs.m_pTextureCom)
	, m_pVb(rhs.m_pVb)
	, m_fDelayCount(rhs.m_fDelayCount)
	, m_dwVbSize(rhs.m_dwVbSize)
	, m_dwVbOffset(rhs.m_dwVbOffset)
	, m_dwVbBatchSize(rhs.m_dwVbBatchSize)
	, m_fGravityStartTime(rhs.m_fGravityStartTime)
	, m_ParticleInfo(rhs.m_ParticleInfo)
	, m_iCount(rhs.m_ParticleInfo.iMaxParticles)
	, m_eSceneID(rhs.m_eSceneID)
	, m_bFireWorkYPositive(rhs.m_bFireWorkYPositive)
	, m_bDestBlendInv(rhs.m_bDestBlendInv)
	, m_bFollowTrackingPath(rhs.m_bFollowTrackingPath)
	, m_vFadeInfo(rhs.m_vFadeInfo)
	, m_bUseFadeColor(rhs.m_bUseFadeColor)
	, m_vFadeColor(rhs.m_vFadeColor)
{
	m_pRendererCom->Add_Ref();
	m_pTextureCom->Add_Ref();

	m_pGraphicDev->CreateVertexBuffer(
		m_dwVbSize * sizeof(Engine::VTXPARTICLE)
		, D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY
		, Engine::D3DFVF_VTXPARTICLETEX
		, D3DPOOL_DEFAULT
		, &m_pVb
		, 0);

	/*
	//m_pVb->AddRef();
	// <- 새롭게 안만들면 개이상한데
	// ? m_pVB->AddRef해주면 23 남았는데
	// m_pVb(rhs.m_pVb) 해주면서 버퍼 만들면 또 괜찮네
	// 근데 이렇게 하면 안될거같음 

	//뻑나는 이유
	//	->파티클에는 TransformCom이 없는데,
	//	CEffect::Free()->CGameObejct::Free()를 부르게 되면서
	//	아래의 3개는 필수적으로지우게되는데
	//	Safe_Release(m_pTransformCom);
	//Safe_Release(m_pComponentMgr);
	//Safe_Release(m_pGraphicDev);
	//ComponentMgr, GraphicDev는 상관이 없는데
	//	TransformCom은 할당받지 않았는데 지워줌으로서 문제가 생기게 되어버림

	//	1) CGameObject::Free에 예외처리 하나 더 넣기
	//	2) 그냥 Particle에 TransformCom을 할당하도록 하기.
	//	< -1보단 2가 나을듯 ?
	*/

	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Render", m_pRendererCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", m_pTextureCom));
	m_pRendererCom->Add_Ref();
	m_pTextureCom->Add_Ref();

	// For.Transform
	m_pTransformCom = ((Engine::CTransform*)Engine::CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Com_Transform"));
	if (nullptr == m_pTransformCom)
		return;
	m_mapComponent[Engine::CComponent::COM_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", m_pTransformCom));
	m_pTransformCom->Add_Ref();
	// Transform 컴포넌트만 다른 주소값을 가지게 할당 


}

CEffect_Particle::~CEffect_Particle(void)
{
}


CEffect_Particle* CEffect_Particle::Clone(void)
{
	return new CEffect_Particle(*this);
}

void CEffect_Particle::Set_OriginPos(_vec3 OriginPos)
{

	_vec3 vAdjustPos = OriginPos + m_vDetailPos;
	m_ParticleInfo.vOriginPos = vAdjustPos;

	// bGartherToSpot을 사용하지않으면 , OriginPos에 생성된다.
}

HRESULT CEffect_Particle::Set_DestPos(_vec3 DestPos)
{
	m_ParticleInfo.vDestPos = DestPos;

	return NOERROR;
}

void CEffect_Particle::Set_Dir(_vec3 vDir)
{
	m_ParticleInfo.vDir = vDir;
}

void CEffect_Particle::Set_Speed(_float fSpeed)
{
	m_ParticleInfo.fVelocity = fSpeed;
}

HRESULT CEffect_Particle::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;
	
	return NOERROR;
}


HRESULT CEffect_Particle::Initialize(Engine::PARTICLE_INFO ParticleInfo)
{
	Clean_Up();
	//Randomize();		// <- 계속 호출해서 같은 시드값 나옴
								// 맨처음 Init에서만 한번 해주자.

	////////////////////////////////////////////////////////
	m_dwVbSize = 2048;
	m_dwVbOffset = 0;
	m_dwVbBatchSize = 512;

	m_pGraphicDev->CreateVertexBuffer(
		m_dwVbSize * sizeof(Engine::VTXPARTICLE)
		, D3DUSAGE_DYNAMIC | D3DUSAGE_POINTS | D3DUSAGE_WRITEONLY
		, Engine::D3DFVF_VTXPARTICLETEX
		, D3DPOOL_DEFAULT
		, &m_pVb
		, 0);
	//m_pVb->AddRef();

	m_ParticleInfo = ParticleInfo;
	m_ParticleInfo.bSpreadExceptY = FALSE;
	m_iCount = m_ParticleInfo.iMaxParticles;

	// Gravity
	m_ParticleInfo.bGravityTimeRand = TRUE;
	m_fGravityStartTime = 0.2f;

	return S_OK;
}

int CEffect_Particle::Update_GameObject(const float & fTimeDelta)
{
	//파티클 업데이트

	if (m_bDead)
		return 1;

	Engine::CGameObject::Update_GameObject(fTimeDelta);

	if (nullptr != m_pRendererCom)
	{
		if (m_ParticleInfo.bAlphaBlend)
			m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_ALPHA, this);
		else
			m_pRendererCom->Add_RenderList(Engine::CRenderer::RENDER_DEFAULT, this);
	}

	list<Engine::PARTICLE_ATT*>::iterator iter_begin = m_vecParticle.begin();

	while (iter_begin != m_vecParticle.end())
	{
		//파티클 
		(*iter_begin)->fAge += fTimeDelta;


		if (!m_ParticleInfo.bGatherToSpot)
		{
			if (m_ParticleInfo.bGravity && (*iter_begin)->fAge >= (*iter_begin)->fGravityStartTime)		// 이 예외처리를 안해주니까 두번 Pos에값이 들어가서 값이 튀었었음.
			{
				//중력도 시간이 지날수록 떨어지는 속도가 빨라지니까..
				(*iter_begin)->vVelocity.y -= (GRAVITY * (*iter_begin)->fAge)  * fTimeDelta;
				(*iter_begin)->vPos += (*iter_begin)->vVelocity  /** m_ParticleInfo.fVelocity*/ * fTimeDelta;
			}
			else
				(*iter_begin)->vPos += (*iter_begin)->vVelocity /** m_ParticleInfo.fVelocity*/ * fTimeDelta;
		}


		else		// 한 점으로(Dest) 모여.
		{
			D3DXVECTOR3 vDir;
			vDir = m_ParticleInfo.vDestPos - (*iter_begin)->vPos;
			D3DXVec3Normalize(&vDir, &vDir);
			(*iter_begin)->vPos += m_ParticleInfo.fVelocity * vDir * fTimeDelta;
		}

		// 사라지기 1초전부터, FadeOut처리를 해준다.
		if (m_ParticleInfo.bFadeOut)
		{
			float fFadeOutTime = (*iter_begin)->fLifeTime - m_vFadeInfo.x;
			if ((*iter_begin)->fAge >= fFadeOutTime)
			{
				(*iter_begin)->Color.a -= fTimeDelta * m_vFadeInfo.y;

				if (m_bUseFadeColor)
				{
					if ((*iter_begin)->Color.r > m_vFadeColor.x)
						(*iter_begin)->Color.r -= fTimeDelta;
					else if((*iter_begin)->Color.r < m_vFadeColor.x)
						(*iter_begin)->Color.r += fTimeDelta;

					if ((*iter_begin)->Color.g > m_vFadeColor.y)
						(*iter_begin)->Color.g -= fTimeDelta;
					else if ((*iter_begin)->Color.g < m_vFadeColor.y)
						(*iter_begin)->Color.g += fTimeDelta;

					if ((*iter_begin)->Color.b > m_vFadeColor.z)
						(*iter_begin)->Color.b -= fTimeDelta;
					else if ((*iter_begin)->Color.b < m_vFadeColor.z)
						(*iter_begin)->Color.b += fTimeDelta;
				}
			}
		}

		//생명이 다된 파티클 종료
		if ((*iter_begin)->fAge > (*iter_begin)->fLifeTime && (*iter_begin)->bIsDead == FALSE)	// 생명보다 살아온 시간이 길면
		{
			Engine::Safe_Delete(*iter_begin);
			iter_begin = m_vecParticle.erase(iter_begin);
		}
		else
			iter_begin++;
	}

	// Loop
	if (!m_ParticleInfo.bLoop && m_iCount <= 0)
	{
		if (!Is_Empty())		// 동적 할당된 파티클이 다 사라지면 지우자.
			return 0;
		else
		{
			m_bDead = TRUE;
			return 1;		// 루프가 아니면서, 할당된 갯수만큼 생성했으면 끝
		}
	}

	if (m_ParticleInfo.fEmitRate < m_fDelayCount && m_ParticleInfo.bCreateOnce == FALSE)
	{
		//최대 갯수보다 작으면
		if ((size_t)m_ParticleInfo.iMaxParticles > m_vecParticle.size())
		{
			Add_Particle();
			--m_iCount;		// 하나씩 줄여나간당.
		}
		m_fDelayCount = 0.f;
	}

	else if (m_ParticleInfo.bCreateOnce == TRUE )
	{
		if ((size_t)m_ParticleInfo.iMaxParticles < m_vecParticle.size())
			return 0;

		for (int i = 0; i < m_ParticleInfo.iMaxParticles; i++)
		{
			Add_Particle();		// 설정한 갯수만큼 한번에 생성한다.
			--m_iCount;
		}
	}

	m_fDelayCount += fTimeDelta;

	return 0;
}

void CEffect_Particle::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (!Is_Empty())
	{
		//렌더 상태를 지정.
		Pre_Render();

		m_pGraphicDev->SetStreamSource(0, m_pVb, 0, sizeof(Engine::VTXPARTICLE));
		m_pGraphicDev->SetFVF(Engine::D3DFVF_VTXPARTICLETEX);
		m_pTextureCom->SetUp_OnGraphicDev(m_ParticleInfo.iTextureIndex);
		// 정점 정보가 담겨 있는 정점 버퍼를 출력 스트림으로 할당


		// 버텍스 버퍼를 벗어날 경우 처음부터 시작한다.
		m_dwVbOffset += m_dwVbBatchSize;
		if (m_dwVbOffset >= m_dwVbSize)
			m_dwVbOffset = 0;

		Engine::VTXPARTICLE* Particle = nullptr;

		// _vbBatchSize - 하나의 단계에 정의된 파티클의 수.
		m_pVb->Lock(
			m_dwVbOffset * sizeof(Engine::VTXPARTICLE),
			m_dwVbBatchSize * sizeof(Engine::VTXPARTICLE),
			(void**)&Particle,
			m_dwVbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DWORD numParticlesInBatch = 0;

		if (Particle == nullptr)
			return;

		// 모든 파티클이 렌더링될 때까지
		list<Engine::PARTICLE_ATT*>::iterator iter_begin = m_vecParticle.begin();
		for (; iter_begin != m_vecParticle.end(); iter_begin++)
		{
			// 한단계의 생존한 파티클을다음 버텍스 버퍼 세그먼트로 복사.
 			Particle->vPos = (*iter_begin)->vPos;
			Particle->Color = (D3DCOLOR)(*iter_begin)->Color;
			Particle++; // next element;

						// 현재 단계가 모두 채워져 있는가?
			if (++numParticlesInBatch == m_dwVbBatchSize)
			{
				// 버텍스 버퍼로 복사된 마지막 단계의 파티클들을 그린다.
				m_pVb->Unlock();

				m_pGraphicDev->DrawPrimitive(
					D3DPT_POINTLIST,
					numParticlesInBatch,
					m_dwVbBatchSize);
				// 단계가 그려지는 동안 다음 단계를 파티클로 채운다.
				// 다음 단계의 처음 오프셋으로 이동한다.
				m_dwVbOffset += m_dwVbBatchSize;

				// 버텍스 버퍼의 경계를 넘는메모리로 오프셋을 설정하지 않는다.
				// 경계를 넘을 경우 처음부터 시작.
				if (m_dwVbOffset >= m_dwVbSize)
					m_dwVbOffset = 0;

				m_pVb->Lock(
					m_dwVbOffset * sizeof(Engine::VTXPARTICLE),
					m_dwVbBatchSize * sizeof(Engine::VTXPARTICLE),
					(void**)&Particle,
					m_dwVbOffset ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

				numParticlesInBatch = 0; // 다음 단계를 위한 리셋
			}
		}

		m_pVb->Unlock();


		if (numParticlesInBatch > 0)
		{
			m_pGraphicDev->DrawPrimitive(
				D3DPT_POINTLIST,
				m_dwVbOffset,
				numParticlesInBatch);
		}

		// 다음 블록
		m_dwVbOffset += m_dwVbBatchSize;

		// reset render states
		Post_Render();
	}

}

HRESULT CEffect_Particle::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, m_eSceneID, L"Com_Texture_Particle", Engine::CComponent::COM_STATIC, L"Com_Texture");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");

	return NOERROR;

}

void CEffect_Particle::Reset_Effect(void)
{

}

void CEffect_Particle::Add_Particle(void)
{
	Engine::PARTICLE_ATT* Attribute = new Engine::PARTICLE_ATT;

	//시작위치
	if(!m_ParticleInfo.bStartPosSpread)
		Attribute->vPos = m_ParticleInfo.vOriginPos;
	else
	{
		float fAngle = RandFloat * 3.14f * m_ParticleInfo.fSpreadDegree;
		Attribute->vPos.x = m_ParticleInfo.fSize * RandFloat * cosf(fAngle)  * m_ParticleInfo.fSpreadDegree + m_ParticleInfo.vOriginPos.x;
		Attribute->vPos.y = m_ParticleInfo.fSize * RandFloat * sinf(fAngle) * m_ParticleInfo.fSpreadDegree + m_ParticleInfo.vOriginPos.y;
		fAngle = RandFloat * 3.14f * m_ParticleInfo.fSpreadDegree;
		Attribute->vPos.z = m_ParticleInfo.fSize * RandFloat * cosf(fAngle) * m_ParticleInfo.fSpreadDegree + m_ParticleInfo.vOriginPos.z;

		if (m_ParticleInfo.bSpreadExceptY)
			Attribute->vPos.y = m_ParticleInfo.vOriginPos.y;
	}

	if (m_bFollowTrackingPath && m_pmatFix != nullptr)	// 궤적 옵션을 쓰지만, 고정할 행렬이 없으면 의미 X
	{
		_vec3 vFixmatrixPos;

		if (m_ParticleInfo.bStartPosSpread)	// 퍼지기를 쓴다면 (보통 궤적 따라가게하면 쓸꺼야.)
		{
			if (m_pmatParent == nullptr)		// 부모행렬은 없다면. .
			{
				memcpy(&vFixmatrixPos, &m_pmatFix->m[3][0], sizeof(_vec3));
			}
			else	// 퍼지기 + 부모행렬까지 있다면.
			{
				_matrix TempMat = (*m_pmatFix) *  (*m_pmatParent);
				memcpy(&vFixmatrixPos, &TempMat.m[3][0], sizeof(_vec3));
			}

			float fAngle = RandFloat * 3.14f * m_ParticleInfo.fSpreadDegree;
			Attribute->vPos.x = m_ParticleInfo.fSize * RandFloat * cosf(fAngle)  * m_ParticleInfo.fSpreadDegree + vFixmatrixPos.x;
			Attribute->vPos.y = m_ParticleInfo.fSize * RandFloat * sinf(fAngle) * m_ParticleInfo.fSpreadDegree + vFixmatrixPos.y;
			fAngle = RandFloat * 3.14f * m_ParticleInfo.fSpreadDegree;
			Attribute->vPos.z = m_ParticleInfo.fSize * RandFloat * cosf(fAngle) * m_ParticleInfo.fSpreadDegree + vFixmatrixPos.z;

		}
		else											// 쓰지 않는다면
		{
			if (m_pmatParent == nullptr)		// 부모행렬은 없다면. .
			{
				memcpy(&vFixmatrixPos, &m_pmatFix->m[3][0], sizeof(_vec3));
			}
			else										// 퍼지기X + 부모행렬 있다면.
			{
				_matrix TempMat = (*m_pmatFix) *  (*m_pmatParent);
				memcpy(&vFixmatrixPos, &TempMat.m[3][0], sizeof(_vec3));

			}
			Attribute->vPos = vFixmatrixPos;
		}
	}

	//컬러
	if (m_ParticleInfo.bRandColor)
	{
		float r = m_ParticleInfo.vMinColor.x + (m_ParticleInfo.vMaxColor.x - m_ParticleInfo.vMinColor.x)*RandFloat;
		float g = m_ParticleInfo.vMinColor.y + (m_ParticleInfo.vMaxColor.y - m_ParticleInfo.vMinColor.y)*RandFloat;
		float b = m_ParticleInfo.vMinColor.z + (m_ParticleInfo.vMaxColor.z - m_ParticleInfo.vMinColor.z)*RandFloat;
		Attribute->Color = D3DXCOLOR(r, g, b, 1.f);
	}
	else	// 정해진 컬러라면..
	{
		Attribute->Color = D3DXCOLOR(((float)m_ParticleInfo.vPickColor.x / 255.f)
			, ((float)m_ParticleInfo.vPickColor.y / 255.f)
			, ((float)m_ParticleInfo.vPickColor.z / 255.f)
			, 1.f);
	}

	if (m_bUseFadeColor)
	{
		Attribute->ColorFade = D3DXCOLOR(m_vFadeColor.x, m_vFadeColor.y, m_vFadeColor.z, 1.f);
	}
	else
	{
		Attribute->ColorFade = Attribute->Color;
	}
	
	//방향벡터
	if (m_ParticleInfo.bApplyDir)	// Dir 적용
	{
		Attribute->vVelocity.x = m_ParticleInfo.vDir.x * RandFloat;
		Attribute->vVelocity.y = m_ParticleInfo.vDir.y * RandFloat;
		Attribute->vVelocity.z = m_ParticleInfo.vDir.z * RandFloat;
	}
	else					// 아니면
	{
		Get_RandomVector(&Attribute->vVelocity
			, &D3DXVECTOR3(-m_ParticleInfo.fVelocity, -m_ParticleInfo.fVelocity, -m_ParticleInfo.fVelocity)
			, &D3DXVECTOR3(m_ParticleInfo.fVelocity, m_ParticleInfo.fVelocity, m_ParticleInfo.fVelocity));	
		//ㄴ 불꽃 형태

		if (m_bFireWorkYPositive && Attribute->vVelocity.y <= 0.f)
		{
			float fTemp = m_ParticleInfo.fVelocity - 15.f;
			Get_RandomVector(&Attribute->vVelocity
				, &D3DXVECTOR3(-fTemp, -m_ParticleInfo.fVelocity, -fTemp)
				, &D3DXVECTOR3(fTemp, m_ParticleInfo.fVelocity, fTemp));

			Attribute->vVelocity.y *= -1.f;		// 아래로 꺼지게 하고싶지 않음.
		}
	}

	if (m_ParticleInfo.bGravityTimeRand)		// 중력도 랜덤으로 주고싶으면
	{
		float fRand = Get_RandomFloat(-1.f, 1.f);
		Attribute->fGravityStartTime = m_fGravityStartTime + fRand;
	}
	else
		Attribute->fGravityStartTime = m_fGravityStartTime;

	_float fRandLife = Get_RandomFloat(-0.5f, 0.5f);
	Attribute->fLifeTime = m_ParticleInfo.fLife + fRandLife;


	Attribute->bIsDead = FALSE;
	Attribute->fAge = 0.f;
	Attribute->vAcc = D3DXVECTOR3(0.f, 0.f, 0.f);

	m_vecParticle.push_back(Attribute);
}


void CEffect_Particle::Pre_Render(void)
{
	// 렌더링전 초기 렌더 상태 설정.

	D3DXMATRIX matIdentity;
	m_pGraphicDev->SetTransform(D3DTS_WORLD, D3DXMatrixIdentity(&matIdentity));
	// 행렬 초기화
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);	// 텍스쳐를 이용한다
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);	// 스케일을 이용한다

	//// Z버퍼 끔
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// 거리에 따른 파티클의 크기를 제어
	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE_MIN, FtoDW(0.f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_A, FtoDW(0.f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_B, FtoDW(0.f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALE_C, FtoDW(1.f));
	m_pGraphicDev->SetRenderState(D3DRS_POINTSIZE, FtoDW(m_ParticleInfo.fSize));


	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	if (m_ParticleInfo.bAlphaBlend)
	{
		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		if (m_bDestBlendInv)
		{
			m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		}
	}
	else
	{
		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE); 
		m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0x1f);
	}

}

void CEffect_Particle::Post_Render(void)
{

	// 특정 파티클이 지정했을수 있는 렌더상태를 복구
	// 파티클마다 다를수있음
	m_pGraphicDev->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_LIGHTING, TRUE);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	m_pGraphicDev->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG1, 0);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAARG2, 0);
	m_pGraphicDev->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

	if (m_ParticleInfo.bAlphaBlend)
	{
		m_pGraphicDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);	
		m_pGraphicDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		m_pGraphicDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	}
	else
	{
		m_pGraphicDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
		m_pGraphicDev->SetRenderState(D3DRS_ALPHAREF, 0);
	}

}

bool CEffect_Particle::Is_Empty(void)
{
	return m_vecParticle.empty();
}

HRESULT CEffect_Particle::Clean_Up(void)
{
	// 동적할당한 파티클 정보와, 텍스쳐, 버퍼를 싹다 밀어버림.
	list<Engine::PARTICLE_ATT*>::iterator iter_begin = m_vecParticle.begin();
	list<Engine::PARTICLE_ATT*>::iterator iter_end = m_vecParticle.end();

	for (; iter_begin != iter_end; ++iter_begin)
		Safe_Delete(*iter_begin);
	m_vecParticle.clear();

	return S_OK;
}

void CEffect_Particle::Set_ID(SCENEID eID)
{
	m_eSceneID = eID;
}

float CEffect_Particle::Get_RandomFloat(float LowBound, float HighBound)
{
	if (LowBound >= HighBound)
		return LowBound;

	float f = (rand() % 10000) * 0.0001f;

	return (f *(HighBound - LowBound)) + LowBound;
}


void CEffect_Particle::Get_RandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max)
{
	out->x = Get_RandomFloat(min->x, max->x);
	out->y = Get_RandomFloat(min->y, max->y);
	out->z = Get_RandomFloat(min->z, max->z);
}


CEffect_Particle* CEffect_Particle::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::PARTICLE_INFO ParticleInfo, SCENEID eID)
{
	CEffect_Particle* pInstance = new CEffect_Particle(pGraphicDev);
	
	pInstance->Set_ID(eID);

	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CEffect_Particle Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}

	pInstance->Initialize(ParticleInfo);

	return pInstance;

}

void CEffect_Particle::Free(void)
{
	DWORD Count = 0;

	Clean_Up();

	Count = Safe_Release(m_pRendererCom);
	Count = Safe_Release(m_pTextureCom);
	Count = Safe_Release(m_pVb);

	Engine::CEffect::Free();

	//cout << "파티클 소멸!" << endl;
}
