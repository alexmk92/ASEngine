
/*
******************************************************************
* ASTexture.cpp
******************************************************************
* Implements methods outlined in ASTexture.h
*/

#include "ASTexture.h"

/*
******************************************************************
* Constructor
******************************************************************
* Set member pointers to null
*/

ASTexture::ASTexture()
{
	m_texture = 0;
}

/*
******************************************************************
* Empty constructor
******************************************************************
*/

ASTexture::ASTexture(const ASTexture& texShader)
{}

/*
******************************************************************
* Destructor
******************************************************************
*/

ASTexture::~ASTexture()
{}

/*
******************************************************************
* METHOD: Init
******************************************************************
* Initialise the Texture shader
*
* @param ID3D11Device* - The device we are rendering with
* @param WCHAR* - The name of the texture file we are loading
*
* @return bool - True if the tex loaded, else false
*/

bool ASTexture::Init(ID3D11Device* device, WCHAR* filename)
{
	HRESULT hr;

	// Load the texture into m_texture using the filename and device pointer
	hr = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &m_texture, NULL);
	if(FAILED(hr))
		return false;
	else
		return true;
}

/*
******************************************************************
* METHOD: Release
******************************************************************
* Dispose of any texture resources the application is holding on to
*/

void ASTexture::Release()
{
	if(m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	return;
}

/*
******************************************************************
* METHOD: Get Texture
******************************************************************
* Return the current set texture, either a null pointer or the 
* texture we have initialised
*
* @return ID3D11ShaderResourceView* - A pointer to the texture resource
*/

ID3D11ShaderResourceView* ASTexture::GetTexture()
{
	return m_texture;
}



