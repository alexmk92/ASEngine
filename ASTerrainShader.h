
/*
******************************************************************
* ASTextureShader.h
******************************************************************
* Invoke HLSL shaders to draw models which are stored in the GPU
******************************************************************
*/

#ifndef _ASTERRAINSHADER_H_
#define _ASTERRAINSHADER_H_

/*
******************************************************************
* Includes
******************************************************************
*/

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
#include "ASLightShader.h"
using namespace std;

/*
******************************************************************
* Class Declaration
******************************************************************
*/

class ASTerrainShader : public ASLightShader
{
private:
	// Struct to hold the light buffer, mapped to that in the vertex shader
	struct ASLightBuffer
	{
		D3DXVECTOR4 diffuse;
		D3DXVECTOR4 ambient;
		D3DXVECTOR3 lightDir;
		float padding;
	};
public:
	// Constructors and Destructors
	ASTerrainShader();
	ASTerrainShader(const ASTerrainShader&);
	~ASTerrainShader();

	// Public methods
	bool Init(ID3D11Device*, HWND);
	void Release();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, 
				D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3);

private:
	// Private methods
	bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ReleaseShader();
	void RaiseShaderError(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, 
							 D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3);
	void RenderShader(ID3D11DeviceContext*, int);
};

#endif