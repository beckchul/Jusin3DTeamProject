#include "TextureManager.h"
#include "GameObject.h"

USING(Engine)
IMPLEMENT_SINGLETON(Engine::CTextureManager)

Engine::CTextureManager::CTextureManager(void)
{

}


Engine::CTextureManager::~CTextureManager(void)
{

}

LPDIRECT3DTEXTURE9* Engine::CTextureManager::Get_Texture(const TCHAR * pTextureTag)
{
	auto iter = m_mapTexture.find(pTextureTag);

	if (iter == m_mapTexture.end())
		return nullptr;

	return &(iter->second);
}

void Engine::CTextureManager::Initialize(LPDIRECT3DDEVICE9 pGraphicDev)
{
	m_pGraphicDev = pGraphicDev;
	m_pGraphicDev->AddRef();

	LPDIRECT3DTEXTURE9 pTexture = nullptr;


	//////////////////////////// Color ///////////////////////////

	// 1/4 씩 조합해서 만든 텍스처.(체스판)
	if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, L"../Resources/Texture/GlobalTexture/chessboard_d.tga", &pTexture)))
		MSG_BOX(L"TextureManager initialize Failed");

	m_mapTexture.insert(MAPTEXTURE::value_type(L"chessboard_d", pTexture));
	


	/////////////////////////// Normal ///////////////////////////


	//// 1/4 씩 조합해서 만든 텍스처.(체스판)
	//if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, L"../Resources/Texture/GlobalTexture/chessboard_n.tga", &pTexture)))
	//	MSG_BOX(L"TextureManager initialize Failed");

	//m_mapTexture.insert(MAPTEXTURE::value_type(L"chessboard_n", pTexture));



	//////////////////////////// Etc ///////////////////////////

	if (FAILED(D3DXCreateCubeTextureFromFile(m_pGraphicDev, L"../Resources/Texture/GlobalTexture/CubeMap.dds", &m_pCubeTex)))
		MSG_BOX(L"TextureManager initialize Failed");


	if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, L"../Resources/Texture/GlobalTexture/lavaNoise_o.tga", &pTexture)))
		MSG_BOX(L"TextureManager initialize Failed");

	m_mapTexture.insert(MAPTEXTURE::value_type(L"lavaNoise_o", pTexture));

	if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, L"../Resources/Texture/GlobalTexture/lavaNoise_n.tga", &pTexture)))
		MSG_BOX(L"TextureManager initialize Failed");

	m_mapTexture.insert(MAPTEXTURE::value_type(L"lavaNoise_n", pTexture));

	if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, L"../Resources/Texture/GlobalTexture/Lave_02_a.tga", &pTexture)))
		MSG_BOX(L"TextureManager initialize Failed");

	m_mapTexture.insert(MAPTEXTURE::value_type(L"Lave_02_a", pTexture));

	if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, L"../Resources/Texture/GlobalTexture/cloudsMerge_1_2_3_o.tga", &pTexture)))
		MSG_BOX(L"TextureManager initialize Failed");

	m_mapTexture.insert(MAPTEXTURE::value_type(L"cloudsMerge_1_2_3_o", pTexture));


	/*if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, L"../Resources/Texture/GlobalTexture/Rain_2_n.tga", &pTexture)))
		MSG_BOX(L"TextureManager initialize Failed");

	m_mapTexture.insert(MAPTEXTURE::value_type(L"Rain_2_n", pTexture));*/


	if (FAILED(D3DXCreateTextureFromFile(m_pGraphicDev, L"../Resources/Texture/GlobalTexture/clouds_a.tga", &pTexture)))
		MSG_BOX(L"TextureManager initialize Failed");

	m_mapTexture.insert(MAPTEXTURE::value_type(L"clouds_a", pTexture));


}

void Engine::CTextureManager::Free(void)
{
	for_each(m_mapTexture.begin(), m_mapTexture.end(), CRelease_Pair());
	m_mapTexture.clear();

	Safe_Release(m_pCubeTex);

	Safe_Release(m_pGraphicDev);
}

