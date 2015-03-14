
/*
******************************************************************
* ASText.h
*******************************************************************
* Used to build text to the screen, utilises the ASFont and 
* ASFontShader resources to output font to the screen
*******************************************************************
*/

#ifndef _ASTEXT_H_
#define _ASTEXT_H_

/*
*******************************************************************
* Includes:
*******************************************************************
*/

#include "ASFont.h"
#include "ASFontShader.h"

/*
*******************************************************************
* Class declaration
*******************************************************************
*/

class ASText
{
private:
	// Struct to describe the font
	struct ASSentence
	{
		ID3D11Buffer* vBuffer;
		ID3D11Buffer* iBuffer;
		int numVerts;
		int numIndices;
		int maxLen;
		float r;
		float g;
		float b;
	};
	// Vertex structure to render the character
	struct ASVertex
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 tex;
	};

public:
	// Constructors and Destructors
	ASText();
	ASText(const ASText&);
	~ASText();

	// Public methods
	bool Init(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, D3DXMATRIX);
	void Release();
	bool Render(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX);

private:
	// Private methods
	bool InitSentence(ASSentence**, int, ID3D11Device*);
	bool UpdateSentence(ASSentence*, char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(ASSentence**);
	bool RenderSentence(ID3D11DeviceContext*, ASSentence*, D3DXMATRIX, D3DXMATRIX);

	// Private member variables
	ASFont* m_font;
	ASFontShader* m_fontShader;
	int m_width;
	int m_height;
	D3DXMATRIX m_viewMatrix;

	// Holds the sentence
	ASSentence* m_sentence;
	 
};

#endif