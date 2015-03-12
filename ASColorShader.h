
/*
******************************************************************
* ASColorShader.h
******************************************************************
* Invoke HLSL shaders to draw models which are stored in the GPU
******************************************************************
*/

#ifndef _ASCOLORSHADER_H_
#define _ASCOLORSHADER_H_

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

class ASColorShader
{
private:
	// Context buffer to be passed to the vertex shader, this matches that of what is found in ASColor.ps
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

public:
	// Constructors and Destructors
	ASColorShader();
	ASColorShader(const ASColorShader&);
	~ASColorShader();

	// Public interface methods to access private Init, Run and Release methods
	bool Init(ID3D11Device*, HWND);
	void Release();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
	
private:
	// Methods invoked by the public interface
	bool InitShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ReleaseShader();
	void ReportShaderError(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);
	void RenderShader(ID3D11DeviceContext*, int);

	// Private member variables
	ID3D11VertexShader* m_vShader;
	ID3D11PixelShader*  m_pShader;
	ID3D11InputLayout*  m_layout;
	ID3D11Buffer*       m_mBuffer;

};

#endif