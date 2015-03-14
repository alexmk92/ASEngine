
/*
******************************************************************
* ASFontShader.h
*******************************************************************
* Handles the texture for the font that is being written to the
* screen
*******************************************************************
*/

#ifndef _ASFONTSHADER_H_
#define _ASFONTSHADER_H_

/*
*******************************************************************
* Includes:
*******************************************************************
*/

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;

/*
*******************************************************************
* Class declaration
*******************************************************************
*/

class ASFontShader
{
private:
	// Struct to describe the font
	struct ASConstantBuffer
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};
	// Vertex structure to render the character
	struct ASPixelBuffer
	{
		D3DXVECTOR4 pixelColor;
	};

public:
	// Constructors and Destructors
	ASFontShader();
	ASFontShader(const ASFontShader&);
	~ASFontShader();

	// Public methods
	bool Init(ID3D11Device*, HWND);
	void Release();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4);

private:
	// Private methods
	bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ReleaseShader();
	void RaiseShaderError(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR4);
	void RenderShader(ID3D11DeviceContext*, int);

	// Private member variables
	ID3D11VertexShader* m_vShader;
	ID3D11PixelShader*  m_pShader;
	ID3D11InputLayout*  m_iLayout;
	ID3D11Buffer*       m_cBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer*       m_pBuffer;
	 
};

#endif