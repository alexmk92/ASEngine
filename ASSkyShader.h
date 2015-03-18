
/*
******************************************************************
* ASTextureShader.h
******************************************************************
* Invoke HLSL shaders to draw models which are stored in the GPU
******************************************************************
*/

#ifndef _ASSKYSHADER_H_
#define _ASSKYSHADER_H_

/*
******************************************************************
* Includes
******************************************************************
*/

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;

/*
******************************************************************
* Class Declaration
******************************************************************
*/

class ASSkyShader
{
private:
	// Struct to hold the world matrixes, this will be used to multiply the
	// current position of the skybox relative to the world
	struct ASMatrixBuffer
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};
	// Structure to hold the color of the sky
	struct ASGradient
	{
		D3DXVECTOR4 skyColorA;
		D3DXVECTOR4 skyColorB;
	};
public:
	// Constructors and Destructors
	ASSkyShader();
	ASSkyShader(const ASSkyShader&);
	~ASSkyShader();

	// Public methods
	bool Init(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void Release();
	bool RenderSky(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4);

private:
	// Private methods
	void RaiseShaderError(ID3D10Blob*, HWND, WCHAR*);
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, 
							 D3DXVECTOR4, D3DXVECTOR4);

	// Member variables
	ID3D11Buffer*       m_cBuffer;	// WVP Buffer
	ID3D11Buffer*       m_gBuffer;	// Gradient Buffer
	ID3D11VertexShader* m_vShader;	// Vertex shader
	ID3D11PixelShader*  m_pShader;	// Pixel shader
	ID3D11InputLayout*  m_iLayout;	// Input layout
};

#endif