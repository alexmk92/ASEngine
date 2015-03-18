
/*
*******************************************************************
* ASDirect3D.h 
*******************************************************************
* Handles all Direct3D system calls
*******************************************************************
*/

#ifndef _ASDIRECT3D_H_
#define _ASDIRECT3D_H_

/*
*******************************************************************
* Linker dependencies
*******************************************************************
* Any libs that need to be included at the linking phase should
* be declared here
*******************************************************************
*/

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

/*
*******************************************************************
* Includes
*******************************************************************
* Any libraries needded from the linked dependencies should be 
* included here
*******************************************************************
*/

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>

/*
*******************************************************************
* Class Declaration
*******************************************************************
*/

class ASDirect3D 
{
public:
	// Constructors and Destructors
	ASDirect3D();
	ASDirect3D(const ASDirect3D&);
	~ASDirect3D();

	// Public methods
	bool Init(int, int, float, float, bool, bool, HWND);
	void Release();

	void PrepareBuffers(float, float, float, float);
	void RenderScene();

	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetInterfaceMatrix(D3DXMATRIX&);

	void GetVideoCardInfo(char*, int&);

	ID3D11Device*		 GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void DisableZBuffer();			// Needed for drawing orthographic items to the scene (interface components)
	void EnableZBuffer();
	void DisableAlphaBlending();
	void EnableAlphaBlending();
	void DisableCulling();			// Used for drawing the sky, it requires back culling to be off
	void EnableCulling();			// so we can view items in skybox, else we would see nothing

private:
	// Member variables
	bool					 m_vsync_enabled;
	int						 m_videoCard_memory;
	char				     m_videoCardDesc[128];

	IDXGISwapChain*			 m_swapChain;
	ID3D11DeviceContext*     m_deviceContext;
	ID3D11Device*			 m_device;
	ID3D11RenderTargetView*  m_renderTargetView;
	ID3D11Texture2D*		 m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView*  m_depthStencilView;
	ID3D11RasterizerState*   m_cullingEnabledState;
	ID3D11RasterizerState*   m_cullingDisabledState;

	D3DXMATRIX				 m_projectionMatrix;
	D3DXMATRIX				 m_worldMatrix;
	D3DXMATRIX				 m_interfaceMatrix;

	// For 2D rendering
	ID3D11DepthStencilState* m_depthStencilDisabledState;
	ID3D11BlendState*		 m_alphaEnableBlendState;
	ID3D11BlendState*		 m_alphaDisableBlendState;
};

#endif