#include "stdafx.h"
#include "DynamicObject.h"
#include "tinyxml2.h"

USING(tinyxml2)

CDynamicObject::CDynamicObject(LPDIRECT3DDEVICE9 pGraphicDev)
: Engine::CGameObject(pGraphicDev)
, m_pRendererCom(nullptr)
, m_pShaderCom(nullptr)
, m_pDynamicMeshCom(nullptr)
{
	ZeroMemory(&m_tInfo, sizeof(INFO));
	m_tInfo.bDead = false;
}

CDynamicObject::~CDynamicObject()
{
}

HRESULT CDynamicObject::Load_AnimationInfo(char* szFilePath)
{
	tinyxml2::XMLDocument doc;

	doc.LoadFile(szFilePath);

	XMLNode* pDeclaration = doc.FirstChild();

	XMLNode* pRoot = doc.FirstChildElement("AnimationData"); // <AnimationData>
	XMLNode* pIndexData = pRoot->FirstChildElement("Index0"); // <Index0>

	while (true)
	{
		if (nullptr == pIndexData)
			break;

		Engine::ANI_INFO tTemp;

		XMLElement* pAniIndex = pIndexData->FirstChildElement("AnimationIndex");
		const XMLAttribute* pAniIndexValue = pAniIndex->FirstAttribute();
		pAniIndexValue->QueryUnsignedValue(&tTemp.uiAniIndex);

		XMLElement* pSpeedFront = pIndexData->FirstChildElement("SpeedFront");
		const XMLAttribute* pSpeedFrontValue = pSpeedFront->FirstAttribute();
		pSpeedFrontValue->QueryFloatValue(&tTemp.fAniSpeedFront);

		XMLElement* pSpeedEnd = pIndexData->FirstChildElement("SpeedEnd");
		const XMLAttribute* pSpeedEndValue = pSpeedEnd->FirstAttribute();
		pSpeedEndValue->QueryFloatValue(&tTemp.fAniSpeedEnd);

		XMLElement* pWeightFront = pIndexData->FirstChildElement("WeightFront");
		const XMLAttribute* pWeightFrontValue = pWeightFront->FirstAttribute();
		pWeightFrontValue->QueryFloatValue(&tTemp.fAniWeightFront);

		XMLElement* pWeightEnd = pIndexData->FirstChildElement("WeightEnd");
		const XMLAttribute* pWeightEndValue = pWeightEnd->FirstAttribute();
		pWeightEndValue->QueryFloatValue(&tTemp.fAniWeightEnd);

		XMLElement* pAniCount = pIndexData->FirstChildElement("AniCount");
		const XMLAttribute* pAniCountValue = pAniCount->FirstAttribute();
		pAniCountValue->QueryDoubleValue(&tTemp.dEndAniCount);

		XMLElement* pMagicNumber = pIndexData->FirstChildElement("MagicNumber");
		const XMLAttribute* pMagicNumberValue = pMagicNumber->FirstAttribute();
		pMagicNumberValue->QueryDoubleValue(&tTemp.dMagicNumber);

		m_vecAnimationInfo.push_back(tTemp);

		pIndexData = pIndexData->NextSibling();
	}


	return NOERROR;
}

void CDynamicObject::Free(void)
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDynamicMeshCom);
	Engine::CGameObject::Free();
}
