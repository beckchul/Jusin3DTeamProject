#include "stdafx.h"
#include "Effect_RectParticle.h"
#include "DataManager.h"


CEffect_RectParticle::CEffect_RectParticle(LPDIRECT3DDEVICE9 pGraphicDev)
	: Engine::CEffect(pGraphicDev)
	, m_pRendererCom(nullptr)
	, m_pTextureCom(nullptr)
	, m_pShaderCom(nullptr)
	, m_pBufferCom(nullptr)
	, m_fDelayCount(0.f)
	, m_iCount(0)
	, m_fGravityStartTime(0.f)
	, m_bParticleRotZ(FALSE)
	, m_iDeadParticleCount(0)
	, m_bBubble(FALSE)
	, m_fBubbleDegree(5.f)
	, m_fBubbleTime(0.8f)
	, m_bSmoke(FALSE)
	, m_fSmokeFadeSpeed(0.03f)
	, m_fDefaultAlpha(1.f)
	, m_bFollowTrackingPath(FALSE)
	, m_bUseFadeColor(FALSE)
	, m_vFadeColor(0.f, 0.f, 0.f)
	, m_vFadeInfo(1.f, 1.f)
	, m_fFadeColorSpeed(1.f)
{
}

CEffect_RectParticle::CEffect_RectParticle(const CEffect_RectParticle& rhs)
	: Engine::CEffect(rhs)
	, m_pRendererCom(rhs.m_pRendererCom)
	, m_pTextureCom(rhs.m_pTextureCom)
	, m_pShaderCom(rhs.m_pShaderCom)
	, m_pBufferCom(rhs.m_pBufferCom)
	, m_pTextureMaskCom(rhs.m_pTextureMaskCom)
	, m_pTextureFaderCom(rhs.m_pTextureFaderCom)
	, m_fDelayCount(rhs.m_fDelayCount)
	, m_fGravityStartTime(rhs.m_fGravityStartTime)
	, m_ParticleInfo(rhs.m_ParticleInfo)
	, m_iCount(rhs.m_ParticleInfo.iMaxParticles)
	, m_bParticleRotZ(rhs.m_bParticleRotZ)
	, m_iDeadParticleCount(rhs.m_iDeadParticleCount)
	, m_bBubble(rhs.m_bBubble)
	, m_fBubbleDegree(rhs.m_fBubbleDegree)
	, m_fBubbleTime(rhs.m_fBubbleTime)
	, m_bSmoke(rhs.m_bSmoke)
	, m_fSmokeFadeSpeed(rhs.m_fSmokeFadeSpeed)
	, m_fDefaultAlpha(rhs.m_fDefaultAlpha)
	, m_bFollowTrackingPath(rhs.m_bFollowTrackingPath)
	, m_bUseFadeColor(rhs.m_bUseFadeColor)
	, m_vFadeColor(rhs.m_vFadeColor)
	, m_vFadeInfo(rhs.m_vFadeInfo)
	, m_fFadeColorSpeed(rhs.m_fFadeColorSpeed)
{
	m_ParticleInfo.bSpreadExceptY = FALSE;
	m_pRendererCom->Add_Ref();
	m_pTextureCom->Add_Ref();
	m_pShaderCom->Add_Ref();
	m_pBufferCom->Add_Ref();

	m_pTextureMaskCom->Add_Ref();
	m_pTextureFaderCom->Add_Ref();

	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Render", m_pRendererCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Texture", m_pTextureCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Shader", m_pShaderCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_Buffer", m_pBufferCom));

	m_pRendererCom->Add_Ref();
	m_pTextureCom->Add_Ref();
	m_pShaderCom->Add_Ref();
	m_pBufferCom->Add_Ref();

	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_MaskTexture", m_pTextureMaskCom));
	m_mapComponent[Engine::CComponent::COM_STATIC].insert(MAPCOMPONENT::value_type(L"Com_FaderTexture", m_pTextureFaderCom));

	m_pTextureMaskCom->Add_Ref();
	m_pTextureFaderCom->Add_Ref();

	// For.Transform
	m_pTransformCom = ((Engine::CTransform*)Engine::CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Com_Transform"));
	if (nullptr == m_pTransformCom)
		return;
	m_mapComponent[Engine::CComponent::COM_DYNAMIC].insert(MAPCOMPONENT::value_type(L"Com_Transform", m_pTransformCom));
	m_pTransformCom->Add_Ref();
	// Transform 컴포넌트만 다른 주소값을 가지게 할당 

	// For.Transform_Arr
	_uint iMaxCount = m_ParticleInfo.iMaxParticles;
	m_pTransformArr = new Engine::CTransform*[iMaxCount];

	for (_uint iIndex = 0; iIndex < iMaxCount; ++iIndex)
		m_pTransformArr[iIndex] = nullptr;

	for (_uint iIndex = 0; iIndex < iMaxCount; ++iIndex)
	{
		_tchar* cTemp = nullptr;
		cTemp = new _tchar[18];
		ZeroMemory(cTemp, sizeof(_tchar) * 18);

		wsprintf(cTemp, L"Com_Transform_%d", iIndex);

		m_pTransformArr[iIndex] = ((Engine::CTransform*)Engine::CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Com_Transform"));
		if (nullptr == m_pTransformCom)
			return;
		m_mapComponent[Engine::CComponent::COM_DYNAMIC].insert(MAPCOMPONENT::value_type(cTemp, m_pTransformArr[iIndex]));
		m_pTransformArr[iIndex]->Add_Ref();
		m_listHeapName.push_back(cTemp);

		if(m_ParticleInfo.iTextureIndex == 21)
			m_pTransformArr[iIndex]->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 4.f, 1.f));
		else
			m_pTransformArr[iIndex]->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(m_ParticleInfo.fSize, m_ParticleInfo.fSize, 1.f));

		//Start Pos Init
		if (!m_ParticleInfo.bStartPosSpread)
		{
			_vec3 vPos(5000.f, -5000.f, 5000.f);
			m_pTransformArr[iIndex]->Set_Information(CTransform::INFO_POSITION, &vPos);

			//m_pTransformArr[iIndex]->Set_Information(CTransform::INFO_POSITION, &m_ParticleInfo.vOriginPos);
		}
		else
		{
			_vec3 vPos(5000.f, -5000.f, 5000.f);
			//float fAngle = RandFloat * 3.14f * m_ParticleInfo.fSpreadDegree;
			//vPos.x = m_ParticleInfo.fSize * RandFloat * cosf(fAngle)  * m_ParticleInfo.fSpreadDegree + m_ParticleInfo.vOriginPos.x;
			//vPos.y = m_ParticleInfo.fSize * RandFloat * sinf(fAngle) * m_ParticleInfo.fSpreadDegree + m_ParticleInfo.vOriginPos.y;
			//fAngle = RandFloat * 3.14f * m_ParticleInfo.fSpreadDegree;
			//vPos.z = m_ParticleInfo.fSize * RandFloat * cosf(fAngle) * m_ParticleInfo.fSpreadDegree + m_ParticleInfo.vOriginPos.z;
			//if (m_ParticleInfo.bSpreadExceptY)
			//	vPos.y = m_ParticleInfo.vOriginPos.y;

			m_pTransformArr[iIndex]->Set_Information(CTransform::INFO_POSITION, &vPos);
		}
	}
}

CEffect_RectParticle::~CEffect_RectParticle(void)
{

}

CEffect_RectParticle* CEffect_RectParticle::Clone(void)
{
	return new CEffect_RectParticle(*this);
}

void CEffect_RectParticle::Set_OriginPos(_vec3 OriginPos)
{
	_vec3 vAdjustPos = OriginPos + m_vDetailPos;
	m_ParticleInfo.vOriginPos = vAdjustPos;

	// bGartherToSpot을 사용하지않으면 , OriginPos에 생성된다.
}

HRESULT CEffect_RectParticle::Set_DestPos(_vec3 DestPos)
{
	m_ParticleInfo.vDestPos = DestPos;

	return NOERROR;
}

void CEffect_RectParticle::Set_Dir(_vec3 vDir)
{
	m_ParticleInfo.vDir = vDir;
}

void CEffect_RectParticle::Set_Speed(_float fSpeed)
{
	m_ParticleInfo.fVelocity = fSpeed;
}

HRESULT CEffect_RectParticle::Ready_GameObject(void)
{
	if (FAILED(Engine::CGameObject::Ready_GameObject()))
		return E_FAIL;

	if (FAILED(Add_Component()))
		return E_FAIL;

	_float fRotZ = 0.f;

	for (_uint i = 0; i < (_uint)m_ParticleInfo.iMaxParticles; ++i)
	{
		fRotZ += 10.f;
		//Add_Particle();
		//m_pTransformArr[i]->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian(fRotZ)));
		if (m_ParticleInfo.iTextureIndex == 21)
			m_pTransformArr[i]->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 4.f, 1.f));
		else
			m_pTransformArr[i]->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(m_ParticleInfo.fSize, m_ParticleInfo.fSize, 1.f));

		//Start Pos Init
		if (!m_ParticleInfo.bStartPosSpread)
			m_pTransformArr[i]->Set_Information(CTransform::INFO_POSITION, &m_ParticleInfo.vOriginPos);
		else
		{
			_vec3 vPos(5000.f, 5000.f, 5000.f);
			//float fAngle = RandFloat * 3.14f * m_ParticleInfo.fSpreadDegree;
			//vPos.x = m_ParticleInfo.fSize * RandFloat * cosf(fAngle)  * m_ParticleInfo.fSpreadDegree + m_ParticleInfo.vOriginPos.x;
			//vPos.y = m_ParticleInfo.fSize * RandFloat * sinf(fAngle) * m_ParticleInfo.fSpreadDegree + m_ParticleInfo.vOriginPos.y;
			//fAngle = RandFloat * 3.14f * m_ParticleInfo.fSpreadDegree;
			//vPos.z = m_ParticleInfo.fSize * RandFloat * cosf(fAngle) * m_ParticleInfo.fSpreadDegree + m_ParticleInfo.vOriginPos.z;
			//if (m_ParticleInfo.bSpreadExceptY)
			//	vPos.y = m_ParticleInfo.vOriginPos.y;

			m_pTransformArr[i]->Set_Information(CTransform::INFO_POSITION, &vPos);
		}
	}

	m_eShaderPass = EFFECT_RECTPARTICLE;

	return NOERROR;
}


HRESULT CEffect_RectParticle::Initialize(Engine::PARTICLE_INFO ParticleInfo)
{
	m_ParticleInfo = ParticleInfo;
	m_ParticleInfo.bSpreadExceptY = FALSE;

	m_iCount = m_ParticleInfo.iMaxParticles;

	// Gravity
	m_ParticleInfo.bGravityTimeRand = TRUE;
	m_fGravityStartTime = 1.f;

	if (m_ParticleInfo.iTextureIndex == 9)
		m_bBubble = TRUE;

	return S_OK;
}

int CEffect_RectParticle::Update_GameObject(const float & fTimeDelta)
{
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


	_uint imaxSize = m_vecParticle.size();		// vector와 Transform이 1-1이 되도록..

	for(_uint iIndex = 0 ; iIndex < imaxSize ; ++iIndex)
	{
		if (m_vecParticle[iIndex]->bIsDead == TRUE)
		{
			m_iDeadParticleCount++;
			if (m_iDeadParticleCount >= m_ParticleInfo.iMaxParticles * 2.2)
				return 1;

			continue;
		}

		m_vecParticle[iIndex]->fAge += fTimeDelta;
		//Compute_ViewZ(&m_pTransformArr[iIndex]->Get_Info(CTransform::INFO_POSITION));

		if (!m_ParticleInfo.bGatherToSpot)
		{
			if (m_ParticleInfo.bGravity && m_vecParticle[iIndex]->fAge >= m_vecParticle[iIndex]->fGravityStartTime)
			{
				//중력도 시간이 지날수록 떨어지는 속도가 빨라지니까..
				m_vecParticle[iIndex]->vVelocity.y -= ((GRAVITY* 1.35f) * m_vecParticle[iIndex]->fAge)  * fTimeDelta;
				m_vecParticle[iIndex]->vPos += m_vecParticle[iIndex]->vVelocity  * fTimeDelta;
				m_pTransformArr[iIndex]->Set_Information(Engine::CTransform::INFO_POSITION, &m_vecParticle[iIndex]->vPos);
			}
			else
			{	
				m_vecParticle[iIndex]->vPos += m_vecParticle[iIndex]->vVelocity * fTimeDelta;
				m_pTransformArr[iIndex]->Set_Information(Engine::CTransform::INFO_POSITION, &m_vecParticle[iIndex]->vPos);
			}
		}

		else		// 한 점으로(Dest) 모여.
		{
			_vec3 vDir;
			vDir = m_ParticleInfo.vDestPos - m_vecParticle[iIndex]->vPos;
			D3DXVec3Normalize(&vDir, &vDir);
			m_vecParticle[iIndex]->vPos += m_ParticleInfo.fVelocity * vDir * fTimeDelta;
			m_pTransformArr[iIndex]->Set_Information(Engine::CTransform::INFO_POSITION, &m_vecParticle[iIndex]->vPos);
		}

		if (m_ParticleInfo.bFadeOut)
		{
			float fFadeOutTime = m_vecParticle[iIndex]->fLifeTime - m_vFadeInfo.x;
			if (m_vecParticle[iIndex]->fAge >= fFadeOutTime)
			{
				m_vecParticle[iIndex]->Color.a -= fTimeDelta * m_vFadeInfo.y;

				if (m_bUseFadeColor)
				{
					if (m_vecParticle[iIndex]->Color.r > m_vFadeColor.x)
						m_vecParticle[iIndex]->Color.r -= fTimeDelta * m_fFadeColorSpeed;
					else if (m_vecParticle[iIndex]->Color.r < m_vFadeColor.x)
						m_vecParticle[iIndex]->Color.r += fTimeDelta * m_fFadeColorSpeed;

					if (m_vecParticle[iIndex]->Color.g > m_vFadeColor.y)
						m_vecParticle[iIndex]->Color.g -= fTimeDelta * m_fFadeColorSpeed;
					else if (m_vecParticle[iIndex]->Color.g < m_vFadeColor.y)
						m_vecParticle[iIndex]->Color.g += fTimeDelta * m_fFadeColorSpeed;

					if (m_vecParticle[iIndex]->Color.b > m_vFadeColor.z)
						m_vecParticle[iIndex]->Color.b -= fTimeDelta * m_fFadeColorSpeed;
					else if (m_vecParticle[iIndex]->Color.b < m_vFadeColor.z)
						m_vecParticle[iIndex]->Color.b += fTimeDelta * m_fFadeColorSpeed;

					_vec3 vScale = m_pTransformArr[iIndex]->Get_Info(Engine::CTransform::INFO_SCALE);
					vScale.x -= m_fBubbleDegree * fTimeDelta;
					vScale.y -= m_fBubbleDegree * fTimeDelta;	// 변수만들기 귀챃아서 돌ㄹ려씀
					m_pTransformArr[iIndex]->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(vScale.x, vScale.y, 1.f));

				}

			}
		}

		if (m_bBubble)		// 버블효광
		{
			float fBubbleTime = m_vecParticle[iIndex]->fLifeTime - m_fBubbleTime;		// 사라지기 0.5초전에커짐
			if (m_vecParticle[iIndex]->fAge >= fBubbleTime)
			{
				_vec3 vScale = m_pTransformArr[iIndex]->Get_Info(Engine::CTransform::INFO_SCALE);
				vScale.x += m_fBubbleDegree * fTimeDelta;
				vScale.y += m_fBubbleDegree * fTimeDelta;
				if (vScale.x <= 0.f || vScale.y <= 0.f)
					vScale = _vec3(0.f, 0.f, 1.f);
				m_pTransformArr[iIndex]->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(vScale.x, vScale.y, 1.f));
			}
		}

		////생명이 다된 파티클 리셋 재활용
		//if (m_vecParticle[iIndex]->fAge > m_vecParticle[iIndex]->fLifeTime 
		//	&& m_vecParticle[iIndex]->bIsDead == FALSE
		//	&& m_ParticleInfo.bLoop )	// 생명보다 살아온 시간이 길면 && 루프면
		//{
		//	Reset_Particle(m_vecParticle[iIndex], m_pTransformArr[iIndex]);
		//}
		/*else */if (m_vecParticle[iIndex]->fAge > m_vecParticle[iIndex]->fLifeTime
			&& m_vecParticle[iIndex]->bIsDead == FALSE
			&& !m_ParticleInfo.bLoop)
		{

			m_vecParticle[iIndex]->bIsDead = TRUE;
		}


		if(m_bParticleRotZ)
		{
			_float fRotZ = 0.f;
			if(iIndex % 2 == 0)
				fRotZ = D3DXToRadian(180.f) * fTimeDelta;
			else
				fRotZ = D3DXToRadian(-180.f) * fTimeDelta;
			m_pTransformArr[iIndex]->Rotation(Engine::CTransform::ANGLE_Z, fRotZ);
		}



		// ---BillBoard---
		_matrix		matBill;
		D3DXMatrixIdentity(&matBill);

		matBill = m_pDataMgr->Get_ViewInverseMatrix();
		ZeroMemory(&matBill.m[3][0], sizeof(D3DXVECTOR3));

		_vec3 vPos		 = m_pTransformArr[iIndex]->Get_Info(Engine::CTransform::INFO_POSITION);
		_vec3 vScale	 = m_pTransformArr[iIndex]->Get_Info(Engine::CTransform::INFO_SCALE);
		_vec3 vRot		 = m_pTransformArr[iIndex]->Get_Info(Engine::CTransform::INFO_ANGLE);

		_matrix	matScale, matRotZ, matTrans;

		D3DXMatrixScaling(&matScale, vScale.x, vScale.y, vScale.z);
		D3DXMatrixRotationZ(&matRotZ, vRot.z);
		D3DXMatrixTranslation(&matTrans, vPos.x, vPos.y, vPos.z);

		matBill = matScale *  matRotZ * matBill * matTrans;

		m_pTransformArr[iIndex]->Set_WorldMatrix(&(matBill));
		// ---------------
	}

	// Loop
	if (!m_ParticleInfo.bLoop && m_iCount <= 0)
	{
		if (!m_vecParticle.empty())		// 동적 할당된 파티클이 다 사라지면 지우자.
			return 0;
		else
			return 1;		// 루프가 아니면서, 할당된 갯수만큼 생성했으면 끝
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

	else if (m_ParticleInfo.bCreateOnce == TRUE)
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

void CEffect_RectParticle::Compute_RectParticle_ViewZ(list<ALPHA_SORT*> plist)
{
	if (plist.empty())
		return;

	_matrix matView = m_pDataMgr->Get_ViewInverseMatrix();

	_vec3	vCamPos = D3DXVECTOR3(matView._41, matView._42, matView._43);

	list<ALPHA_SORT*>::iterator iter_begin = plist.begin();
	list<ALPHA_SORT*>::iterator iter_end = plist.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		_vec3	vPos = _vec3((*iter_begin)->matWorld->_41
			, (*iter_begin)->matWorld->_42
			, (*iter_begin)->matWorld->_43);

		(*iter_begin)->fViewZ = D3DXVec3Length(&(vCamPos - vPos));
	}

	//plist.sort(Compare_ViewZ);
}

void CEffect_RectParticle::Render_GameObject(LPD3DXEFFECT pArgEffect, _uint uPassIdx)
{
	if (nullptr == m_pShaderCom || m_vecParticle.empty())
		return;

	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;
	pEffect->AddRef();

	//Compute_RectParticle_ViewZ(m_pSortlist);
	//list<ALPHA_SORT*>::iterator iter_begin = m_pSortlist.begin();

	for (_uint i = 0; i < (_uint)m_vecParticle.size() ; ++i)
	{

		if (FAILED(Set_ConstantTable(pEffect, i, 0)))
			return;

		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(m_eShaderPass); 

		m_pBufferCom->Render_Buffer();

		pEffect->EndPass();
		pEffect->End();

		if (m_vecParticle[i]->fAge > m_vecParticle[i]->fLifeTime
			&& m_vecParticle[i]->bIsDead == FALSE
			&& m_ParticleInfo.bLoop)	// 생명보다 살아온 시간이 길면 && 루프면
		{
			Reset_Particle(m_vecParticle[i], m_pTransformArr[i]);
		}
		//++iter_begin;
	}
	Safe_Release(pEffect);
}


HRESULT CEffect_RectParticle::Set_ConstantTable(LPD3DXEFFECT pEffect, _uint iIdex, _uint iTransIndex)
{
	if (nullptr == pEffect
		|| nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom)
		return E_FAIL;


	_matrix			matView, matProj;
	m_pGraphicDev->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphicDev->GetTransform(D3DTS_PROJECTION, &matProj);

	m_pTransformArr[iIdex]->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);
	//(*m_pTransformlist[iIdex])->SetUp_OnShader(pEffect, "g_matWorldViewProj", matView, matProj);

	m_pTextureCom->SetUp_OnShader(pEffect, "g_DiffuseTexture", m_ParticleInfo.iTextureIndex); // Textrue 번호

	m_pTextureMaskCom->SetUp_OnShader(pEffect, "g_MaskTexture", 0);
	m_pTextureFaderCom->SetUp_OnShader(pEffect, "g_FadeTexture", 0);

	_vec4 vColor = _vec4(m_vecParticle[iIdex]->Color.r, m_vecParticle[iIdex]->Color.g, m_vecParticle[iIdex]->Color.b, m_vecParticle[iIdex]->Color.a);
	pEffect->SetVector("g_vColor", &vColor);

	pEffect->SetVector("g_vUV", (D3DXVECTOR4*)&_vec2(1.f, 1.f));
	pEffect->SetFloat("g_fTime", 0.f);
	pEffect->SetFloat("g_fUVAniSpeed", 0.f);

	return NOERROR;
}

HRESULT CEffect_RectParticle::Add_Component(void)
{
	if (nullptr == m_pComponentMgr)
		return E_FAIL;

	Engine::CComponent*			pComponent = nullptr;

	// For.Renderer
	AddComponent(m_pRendererCom, Engine::CRenderer*, SCENE_STATIC, L"Com_Renderer", Engine::CComponent::COM_STATIC, L"Com_Render");

	// For.Texture
	AddComponent(m_pTextureCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_Particle", Engine::CComponent::COM_STATIC, L"Com_Texture");
	
	// For.MaskTexture
	AddComponent(m_pTextureMaskCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_FogMerge", Engine::CComponent::COM_STATIC, L"Com_MaskTexture");

	// For.FaderTexture
	AddComponent(m_pTextureFaderCom, Engine::CTexture*, SCENE_STATIC, L"Com_Texture_FogFader", Engine::CComponent::COM_STATIC, L"Com_FaderTexture");


	// For.Shader
	AddComponent(m_pShaderCom, Engine::CShader*, SCENE_STATIC, L"Com_Shader_Effect", Engine::CComponent::COM_STATIC, L"Com_Shader");

	// For.Buffer
	AddComponent(m_pBufferCom, Engine::CRect_Texture*, SCENE_STATIC, L"Com_Buffer_RcTex", Engine::CComponent::COM_STATIC, L"Com_Buffer");

	// For.Transform
	AddComponent(m_pTransformCom, Engine::CTransform*, SCENE_STATIC, L"Com_Transform", Engine::CComponent::COM_DYNAMIC, L"Com_Transform");



	// For.Transform_Arr
	_uint iMaxCount = m_ParticleInfo.iMaxParticles;

	m_pTransformArr = new Engine::CTransform*[iMaxCount];

	for (_uint iIndex = 0; iIndex < iMaxCount; ++iIndex)
		m_pTransformArr[iIndex] = nullptr;


	for (_uint iIndex = 0; iIndex < iMaxCount; ++iIndex)
	{
		_tchar* cTemp = nullptr;
		cTemp = new _tchar[18];
		ZeroMemory(cTemp, sizeof(_tchar) * 18);
		
		wsprintf(cTemp, L"Com_Transform_%d", iIndex);

		m_pTransformArr[iIndex] = ((Engine::CTransform*)Engine::CComponent_Manager::GetInstance()->Clone_Component(SCENE_STATIC, L"Com_Transform"));
		if (nullptr == m_pTransformCom)
			return E_FAIL;
		m_mapComponent[Engine::CComponent::COM_DYNAMIC].insert(MAPCOMPONENT::value_type(cTemp, m_pTransformArr[iIndex]));
		m_pTransformArr[iIndex]->Add_Ref();

		m_listHeapName.push_back(cTemp);

		//ALPHA_SORT* pAlphaSort = new ALPHA_SORT(m_pTransformArr[iIndex]->Get_WorldMatrix(), 0.f, iIndex);
		//m_pSortlist.push_back(pAlphaSort);
	}
	return NOERROR;
}

void CEffect_RectParticle::Add_Particle(void)
{
	Engine::PARTICLE_ATT* Attribute = new Engine::PARTICLE_ATT;

	//시작위치
	if (!m_ParticleInfo.bStartPosSpread)
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
		Attribute->Color = D3DXCOLOR(r, g, b, m_fDefaultAlpha);
	}
	else	// 정해진 컬러라면..
	{
		Attribute->Color = D3DXCOLOR(((float)m_ParticleInfo.vPickColor.x / 255.f)
			, ((float)m_ParticleInfo.vPickColor.y / 255.f)
			, ((float)m_ParticleInfo.vPickColor.z / 255.f)
			, m_fDefaultAlpha);
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

		if (Attribute->vVelocity.y <= 0.f)
			Attribute->vVelocity.y *= -1.f;		// 아래로 꺼지게 하고싶지 않음.
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
	Attribute->ColorFade = Attribute->Color;
	Attribute->fAge = 0.f;
	Attribute->vAcc = D3DXVECTOR3(0.f, 0.f, 0.f);

	m_vecParticle.push_back(Attribute);
}

void CEffect_RectParticle::Reset_Particle(Engine::PARTICLE_ATT* pParticleInfo, Engine::CTransform* pTransform)
{
	//시작위치
	if (!m_ParticleInfo.bStartPosSpread)
	{
		pParticleInfo->vPos = m_ParticleInfo.vOriginPos;
		pTransform->Set_Information(Engine::CTransform::INFO_SCALE, &pParticleInfo->vPos);
	}
	else
	{
		float fAngle = RandFloat * 3.14f * m_ParticleInfo.fSpreadDegree;
		pParticleInfo->vPos.x = m_ParticleInfo.fSize * RandFloat * cosf(fAngle)  * m_ParticleInfo.fSpreadDegree + m_ParticleInfo.vOriginPos.x;
		pParticleInfo->vPos.y = m_ParticleInfo.fSize * RandFloat * sinf(fAngle) * m_ParticleInfo.fSpreadDegree + m_ParticleInfo.vOriginPos.y;
		fAngle = RandFloat * 3.14f * m_ParticleInfo.fSpreadDegree;
		pParticleInfo->vPos.z = m_ParticleInfo.fSize * RandFloat * cosf(fAngle) * m_ParticleInfo.fSpreadDegree + m_ParticleInfo.vOriginPos.z;

		if (m_ParticleInfo.bSpreadExceptY)
			pParticleInfo->vPos.y = m_ParticleInfo.vOriginPos.y;

		pTransform->Set_Information(Engine::CTransform::INFO_SCALE, &pParticleInfo->vPos);

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
			pParticleInfo->vPos.x = m_ParticleInfo.fSize * RandFloat * cosf(fAngle)  * m_ParticleInfo.fSpreadDegree + vFixmatrixPos.x;
			pParticleInfo->vPos.y = m_ParticleInfo.fSize * RandFloat * sinf(fAngle) * m_ParticleInfo.fSpreadDegree + vFixmatrixPos.y;
			fAngle = RandFloat * 3.14f * m_ParticleInfo.fSpreadDegree;
			pParticleInfo->vPos.z = m_ParticleInfo.fSize * RandFloat * cosf(fAngle) * m_ParticleInfo.fSpreadDegree + vFixmatrixPos.z;

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
			pParticleInfo->vPos = vFixmatrixPos;
		}
	}

	//컬러
	if (m_ParticleInfo.bRandColor)
	{
		float r = m_ParticleInfo.vMinColor.x + (m_ParticleInfo.vMaxColor.x - m_ParticleInfo.vMinColor.x)*RandFloat;
		float g = m_ParticleInfo.vMinColor.y + (m_ParticleInfo.vMaxColor.y - m_ParticleInfo.vMinColor.y)*RandFloat;
		float b = m_ParticleInfo.vMinColor.z + (m_ParticleInfo.vMaxColor.z - m_ParticleInfo.vMinColor.z)*RandFloat;
		pParticleInfo->Color = D3DXCOLOR(r, g, b, 1.f);
	}
	else	// 정해진 컬러라면..
	{
		pParticleInfo->Color = D3DXCOLOR(((float)m_ParticleInfo.vPickColor.x / 255.f)
			, ((float)m_ParticleInfo.vPickColor.y / 255.f)
			, ((float)m_ParticleInfo.vPickColor.z / 255.f)
			, 1.f);
	}

	//방향벡터
	if (m_ParticleInfo.bApplyDir)	// Dir 적용
	{
		pParticleInfo->vVelocity.x = m_ParticleInfo.vDir.x * RandFloat;
		pParticleInfo->vVelocity.y = m_ParticleInfo.vDir.y * RandFloat;
		pParticleInfo->vVelocity.z = m_ParticleInfo.vDir.z * RandFloat;
	}
	else					// 아니면
	{
		Get_RandomVector(&pParticleInfo->vVelocity
			, &D3DXVECTOR3(-m_ParticleInfo.fVelocity, -m_ParticleInfo.fVelocity, -m_ParticleInfo.fVelocity)
			, &D3DXVECTOR3(m_ParticleInfo.fVelocity, m_ParticleInfo.fVelocity, m_ParticleInfo.fVelocity));
		//ㄴ 불꽃 형태

		if (pParticleInfo->vVelocity.y <= 0.f)
			pParticleInfo->vVelocity.y *= -1.f;		// 아래로 꺼지게 하고싶지 않음.
	}

	if (m_ParticleInfo.bGravityTimeRand)		// 중력도 랜덤으로 주고싶으면
	{
		float fRand = Get_RandomFloat(-1.f, 1.f);
		pParticleInfo->fGravityStartTime = m_fGravityStartTime + fRand;
	}
	else
		pParticleInfo->fGravityStartTime = m_fGravityStartTime;

	_float fRandLife = Get_RandomFloat(-0.5f, 0.5f);
	pParticleInfo->fLifeTime = m_ParticleInfo.fLife + fRandLife;


	pParticleInfo->bIsDead = FALSE;
	pParticleInfo->ColorFade = pParticleInfo->Color;
	pParticleInfo->fAge = 0.f;
	pParticleInfo->vAcc = D3DXVECTOR3(0.f, 0.f, 0.f);


	// Transform Reset
	if(!m_bBubble)
		pTransform->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(m_ParticleInfo.fSize, m_ParticleInfo.fSize, 1.f));
	else
	{
		// Bubble이라면, 사이즈를 Random으로 Reset해주자.
		_float fMinSize = m_ParticleInfo.fSize - 0.5f;
		_float fMaxSize = m_ParticleInfo.fSize + 0.5f;
		_float fSize = Get_RandomFloat(fMinSize, fMaxSize);
		pTransform->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(fSize, fSize, 1.f));
		pTransform->Set_Information(Engine::CTransform::INFO_ANGLE, &_vec3(0.f, 0.f, D3DXToRadian((float)Random(360))));
	}

	if(m_ParticleInfo.iTextureIndex == 21)
		pTransform->Set_Information(Engine::CTransform::INFO_SCALE, &_vec3(1.f, 4.f, 1.f));

}

HRESULT CEffect_RectParticle::Clean_Up(void)
{
	// 동적할당한 파티클 정보와, 텍스쳐, 버퍼를 싹다 밀어버림.
	vector<Engine::PARTICLE_ATT*>::iterator iter_begin = m_vecParticle.begin();
	vector<Engine::PARTICLE_ATT*>::iterator iter_end = m_vecParticle.end();

	for (; iter_begin != iter_end; ++iter_begin)
		Safe_Delete(*iter_begin);
	m_vecParticle.clear();

	return S_OK;
}


float CEffect_RectParticle::Get_RandomFloat(float LowBound, float HighBound)
{
	if (LowBound >= HighBound)
		return LowBound;

	float f = (rand() % 10000) * 0.0001f;

	return (f *(HighBound - LowBound)) + LowBound;
}


void CEffect_RectParticle::Get_RandomVector(D3DXVECTOR3* out, D3DXVECTOR3* min, D3DXVECTOR3* max)
{
	out->x = Get_RandomFloat(min->x, max->x);
	out->y = Get_RandomFloat(min->y, max->y);
	out->z = Get_RandomFloat(min->z, max->z);
}


CEffect_RectParticle* CEffect_RectParticle::Create(LPDIRECT3DDEVICE9 pGraphicDev, Engine::PARTICLE_INFO ParticleInfo)
{
	CEffect_RectParticle* pInstance = new CEffect_RectParticle(pGraphicDev);

	pInstance->Initialize(ParticleInfo);
	if (FAILED(pInstance->Ready_GameObject()))
	{
		MessageBox(0, L"CEffect_RectParticle Created Failed", nullptr, MB_OK);
		pInstance->Release();
	}


	return pInstance;

}

void CEffect_RectParticle::Free(void)
{
	DWORD Count = 0;

	Clean_Up();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pRendererCom);

	Safe_Release(m_pTextureMaskCom);
	Safe_Release(m_pTextureFaderCom);

	for (_uint iTransformNum = 0; 
		iTransformNum < (_uint)m_ParticleInfo.iMaxParticles; 
		++iTransformNum)
	{
		Safe_Release(m_pTransformArr[iTransformNum]);
	}

	Safe_Delete_Array(m_pTransformArr);

	list<_tchar*>::iterator iter_begin = m_listHeapName.begin();
	list<_tchar*>::iterator iter_end = m_listHeapName.end();

	for (; iter_begin != iter_end; ++iter_begin)
		Safe_Delete((*iter_begin));

	Engine::CEffect::Free();

	cout << "렉트 파티클 소멸!" << endl;
}
