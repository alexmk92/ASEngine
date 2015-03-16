
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
protected:
	// Struct to hold the buffer descriptor of the WVP matrices
	struct ASConstantBuffer
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};
	// Struct to hold the camera current position (matches that in the Vertex Shader)
	struct ASCameraBuffer
	{
		D3DXVECTOR3 cameraPos;
		float padding;
	};
	// Struct to hold the light buffer, mapped to that in the vertex shader
	struct ASLightBuffer
	{
		D3DXVECTOR4 diffuse;
		D3DXVECTOR4 ambient;
		D3DXVECTOR3 lightDir;
		float specularIntensity;
		D3DXVECTOR4 specularColor;
	};

public:
	// Constructors and Destructors
	ASLightShader();
	ASLightShader(const ASLightShader&);
	~ASLightShader();

	// Public methods
	virtual bool Init(ID3D11Device*, HWND);
	virtual void Release();
	virtual bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, 
				ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float);

protected:
	// Private methods
	virtual bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	virtual void ReleaseShader();
	virtual void RaiseShaderError(ID3D10Blob*, HWND, WCHAR*);

	virtual bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, 
							 ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float);
	virtual void RenderShader(ID3D11DeviceContext*, int);

	// Private member variables
	ID3D11VertexShader* m_vShader;
	ID3D11PixelShader*  m_pShader;
	ID3D11InputLayout*  m_iLayout;
	ID3D11SamplerState* m_sampleState;

	ID3D11Buffer*       m_cBuffer;
	ID3D11Buffer*       m_lBuffer;
	ID3D11Buffer*       m_camBuffer;

};

#endif