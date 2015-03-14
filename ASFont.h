
/*
******************************************************************
* ASFont.h
*******************************************************************
* Handles the texture for the font that is being written to the
* screen
*******************************************************************
*/

#ifndef _ASFONT_H_
#define _ASFONT_H_

/*
*******************************************************************
* Includes:
*******************************************************************
*/

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;

/*
*******************************************************************
* Global configuration constants
*******************************************************************
*/

#include "ASTexture.h"

/*
*******************************************************************
* Class declaration
*******************************************************************
*/

class ASFont
{
private:
	// Struct to describe the font
	struct ASFontType
	{
		float left;		// U Coord
		float right;	// V Coord
		int size;		// Width of char
	};
	// Vertex structure to render the character
	struct ASVertex
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 tex;
	};

public:
	// Constructors and Destructors
	ASFont();
	ASFont(const ASFont&);
	~ASFont();

	// Public methods
	bool Init(ID3D11Device*, char*, WCHAR*);
	void Release();

	ID3D11ShaderResourceView* GetTexture();
	void BuildVertexArray(void*, char*, float, float);

private:
	// Private methods
	bool LoadFont(char*);
	void ReleaseFont();
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	// Private member variables
	ASFontType* m_font;
	ASTexture*  m_texture;
	 
};

#endif