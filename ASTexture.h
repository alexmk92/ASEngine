
/*
******************************************************************
* ASTexture.h
******************************************************************
* Load textures for a model from a .DDL file
******************************************************************
*/

#ifndef _ASTEXTURE_H_
#define _ASTEXTURE_H_

/*
******************************************************************
* Includes
******************************************************************
*/

#include <d3d11.h>
#include <d3dx11tex.h>

/*
******************************************************************
* Class Declaration
******************************************************************
*/

class ASTexture
{
public:
	// Constructors and Destructors
	ASTexture();
	ASTexture(const ASTexture&);
	~ASTexture();

	// Public methods
	bool Init(ID3D11Device*, WCHAR*);
	void Release();

	ID3D11ShaderResourceView* GetTexture();
	
private:
	// Private member variables
	ID3D11ShaderResourceView* m_texture;
};

#endif