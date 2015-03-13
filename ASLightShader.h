
/*
******************************************************************
* ASTextureShader.h
******************************************************************
* Invoke HLSL shaders to draw models which are stored in the GPU
******************************************************************
*/

#ifndef _ASLIGHTSHADER_H_
#define _ASLIGHTSHADER_H_

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

class ASLightShader
{
private:
	// Struct to hold the buffer descriptor of the WVP matrices
	struct ConstantBuffer
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};
	// Struct to hold the light buffer, mapped to that in the vertex shader
	struct LightBuffer
	{
		D3DXVECTOR4 diffuse;
		D3DXVECTOR3 lightDir;
		float padding;
	};

public:
	// Constructors and Destructors
	ASLightShader();
	ASLightShader(const ASLightShader&);
	~ASLightShader();

	// Public methods
	bool Init(ID3D11Device*, HWND);
	void Release();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, 
				ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4);

private:
	// Private methods
	bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ReleaseShader();
	void RaiseShaderError(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, 
							 ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4);
	void RenderShader(ID3D11DeviceContext*, int);

	// Private member variables
	ID3D11VertexShader* m_vShader;
	ID3D11PixelShader*  m_pShader;
	ID3D11InputLayout*  m_iLayout;
	ID3D11Buffer*       m_cBuffer;
	ID3D11Buffer*       m_lBuffer;

	ID3D11SamplerState* m_sampleState;
};

#endif