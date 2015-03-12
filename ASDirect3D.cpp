
/*
*******************************************************************
* ASDirect3D.cpp
*******************************************************************
* Implementation of the methods described in ASDirect3D.h
*******************************************************************
*/

#include "ASDirect3D.h"

/*
*******************************************************************
* Default Constructor
*******************************************************************
* Initialise all pointers of member variables to a NULL pointer
* so that we know they are set, and are not some random arbitrary
* value assigned by the compiler.  These will later be set
* in the Init() method.
*******************************************************************
*/

ASDirect3D::ASDirect3D()
{
	m_swapChain			 = 0;
	m_device			 = 0;
	m_deviceContext	     = 0;
	m_renderTargetView   = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState  = 0;
	m_depthStencilView   = 0;
	m_rasterState		 = 0;
}

/*
*******************************************************************
* Empty Constructor
*******************************************************************
*/

ASDirect3D::ASDirect3D(const ASDirect3D& e) 
{}

/*
*******************************************************************
* Destructor
*******************************************************************
*/

ASDirect3D::~ASDirect3D()
{}

/*
*******************************************************************
* Method: Init()
*******************************************************************
* Initalises a new instance of ASDirect3D setting all pointers and
* other member variables accordingly.
*
* @param int   : The width of the screen, delegated from ASGraphics.h
* @param int   : The height of the view, delegated from ASGraphics.h
* @param float : The depth of the 3D world we are rendering
* @param float : The near clipping plane fo the 3D world we are viewing
* @param bool  : Determines whether we should sync frame-rate to the users screen refresh rate 
* @param bool  : Determines if the window is in fullscreen mode or not
* @param hwnd  : The handle to the window, delegated from ASgraphics.h 
*
* @return bool : True if the scene was initalised, else false
*******************************************************************
*/

bool ASDirect3D::Init(int width, int height, float depth, float nearPlane, bool vSync, bool fullScreen, HWND handle)
{
	HRESULT hr;

	// Variables used to get info on client hardware
	IDXGIFactory* factory;	  // Used to create a DirectX graphics interface factory
	IDXGIAdapter* adapter;    // Adapt from the factory, to get information of the graphics card
	IDXGIOutput*  adapterOut; // Read the output from the adapter, holding graphics card info

	//
	unsigned int numModes;    
	unsigned int numerator;   // used to calculate the refresh rate of the clients monitor
	unsigned int denominator; //
	unsigned int stringLen;

	// 
	DXGI_MODE_DESC*   displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;

	// Buffer descriptors, and other components to describe rendering pipeline
	DXGI_SWAP_CHAIN_DESC		  swapChainDesc;
	D3D_FEATURE_LEVEL			  featureLevel;
	ID3D11Texture2D*			  ptrBackBuffer;
	D3D11_TEXTURE2D_DESC		  depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC	  depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC		  rasterDesc;
	D3D11_VIEWPORT				  viewport;

	// Other configuration variables
	int   error;
	float fov;
	float aspect;

	m_vsync_enabled = vSync;

	// Calculate the refresh rate of the clients machine, failure to do this will cause DirectX 
	// to not perform buffer switching, which will cause choppy frames and produce errors
	// write the results into "factory"
	hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(hr))
		return false;

	// Use the factory to create an adapter for the video card (select 0 index for the primary Graphics card)
	// write the results into "adapter"
	hr = factory->EnumAdapters(0, &adapter);
	if(FAILED(hr))
		return false;

	// Enumerate the output from the video card, writing the results into "AdapterOut"
	hr = adapter->EnumOutputs(0, &adapterOut);
	if(FAILED(hr))
		return false;

	// Get the number of modes that fit DXGI_FORMAT_R8G8B8A8_UNORM display format, write the result
	// back into numModes
	hr = adapterOut->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if(FAILED(hr))
		return false;

	// Create a list to hold display modes for this monitor/video card
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList)
		return false;

	// Fill the display mode list structure
	hr = adapterOut->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(hr))
		return false;

	// Loop through each display mode and find the one that matches the client screen dimensions
	// when a match is found, store the numerator and denominator of the refresh rate for that monitor
	for(unsigned int i = 0; i < numModes; i++)
	{
		if(displayModeList[i].Width == (unsigned int)width && displayModeList[i].Height == (unsigned int) height)
		{
			numerator   = displayModeList[i].RefreshRate.Numerator;
			denominator = displayModeList[i].RefreshRate.Denominator;
		}
	}

	// Load the description of the video card from the adapter
	hr = adapter->GetDesc(&adapterDesc);
	if(FAILED(hr))
		return false;

	// Store the memory of the card in MB
	m_videoCard_memory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Read the name of the video card from the adapter into the description
	error = wcstombs_s(&stringLen, m_videoCardDesc, 128, adapterDesc.Description, 128);
	if(error != 0)
		return false;

	// Release the structures used to query the data from the video card, then set a null pointer
	// to each reference so that we cannot reuse it by mistake
	delete [] displayModeList;
	adapterOut->Release();
	adapter->Release();
	factory->Release();
	displayModeList = 0;
	adapterOut		= 0;
	adapter			= 0;
	factory			= 0;

	/*
	***********************************************************
	* SWAP CHAIN DESCRIPTION
	***********************************************************
	* Begin to initialise DirectX, build the swapchain (used to process a frame on back buffer then move to front and repeat, gives
	* smooth frame output)
	*/ 

	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width  = width;
	swapChainDesc.BufferDesc.Height = height;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Describe the refresh rate of the buffer (how many times per sec draws are sent from back to front buffer). 
	// If vSync is true, at a rate of 60Hz, a constant of 60 frames will be drawn per second, if vsync is false the GPU
	// will render as many frames per second as it can (can cause visual deformation)
	if(m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator   = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else 
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator   = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the back buffer usage - we utilise it for outputting frames to the render target
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window that frames will be rendered to (Delegated from ASGraphics.h)
	swapChainDesc.OutputWindow = handle;
	
	// Disable multisampling
	swapChainDesc.SampleDesc.Count   = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set the flag which determines if the application is in fullscreen mode or not
	if(fullScreen)
		swapChainDesc.Windowed = false;
	else
		swapChainDesc.Windowed = true;

	// Set scan line ordering and scaling
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard back buffer contents after moving to front buffer (no need to keep the
	// info generated on the back buffer once it has been presented)
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	/*
	***********************************************************
	* FEATURE LEVEL INITIALISATION
	***********************************************************
	* Tell DirectX which version we plan to use, here using DX11
	*/ 

	featureLevel = D3D_FEATURE_LEVEL_11_0;

	/*
	***********************************************************
	* SWAP CHAIN
	***********************************************************
	* Build the swap chain following info provided in desc
	*/ 

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
									   D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, 
									   NULL, &m_deviceContext);
	if(FAILED(hr))
		return false;

	/*
	***********************************************************
	* DIRECT3D DEVICE & CONTEXT
	***********************************************************
	* Build the D3D Rendering Device, used for all D3D calls
	* Get a pointer to the back buffer and attach to swap chain
	*/ 

	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&ptrBackBuffer);
	if(FAILED(hr))
		return false;

	// Create render target view from the back buffer reference
	hr = m_device->CreateRenderTargetView(ptrBackBuffer, NULL, &m_renderTargetView);
	if(FAILED(hr))
		return false;

	// Clean up and dispose of unused pointers
	ptrBackBuffer->Release();
	ptrBackBuffer = 0;

	/*
	***********************************************************
	* DEPTH BUFFER DESCRIPTION
	***********************************************************
	* Creates a depth buffer which will allow polys drawn in
	* 3D world to be layered properly (i.e hide occluded geometry)
	* also used to achieve visual effects such as motion blur
	*/ 

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	
	depthBufferDesc.Width				= width;
	depthBufferDesc.Height				= height;
	depthBufferDesc.MipLevels			= 1;
	depthBufferDesc.ArraySize			= 1;
	depthBufferDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count	= 1;
	depthBufferDesc.SampleDesc.Quality	= 0;
	depthBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags		= 0;
	depthBufferDesc.MiscFlags			= 0;

	// Create the depth stencil buffer using the depth buffer description
	hr = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if(FAILED(hr))
		return false;

	/*
	***********************************************************
	* DEPTH STENCIL DESCRIPTION
	***********************************************************
	* Tells Direct3D which depth tests to perform on each pixel
	*/ 

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	// Front facing pixel operations
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Back facing pixel operations
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil, writing its contents to m_depthStencilState
	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(hr))
		return false;

	// Set the depth stencil statge
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	/*
	***********************************************************
	* DEPTH STENCIL VIEW DESCRIPTION
	***********************************************************
	* Tells Direct3D how to use the depth buffer 
	*/ 

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	hr = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if(FAILED(hr))
		return false;

	/*
	***********************************************************
	* OMSetRenderTargets
	***********************************************************
	* Output the depth stencil buffer to the render pipeline
	* this will tell D3D to draw graphics to the back buffer
	* before being swapped to the front buffer for presentation
	*/ 

	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	/*
	***********************************************************
	* CONFIGURE RASTERISER
	***********************************************************
	* Configure the rasteriser, this tells D3D how to render
	* polygons, such configurations may include rendering 
	* meshes in wireframe mode, or draw only back and front
	* faces of polys
	*/ 

	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasteriser on the device
	hr = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if(FAILED(hr))
		return false;
	else 
		m_deviceContext->RSSetState(m_rasterState);

	/*
	***********************************************************
	* CONFIGURE VIEWPORT
	***********************************************************
	* Set up the viewport so D3D can render to the screen, set
	* this to cover the entire window
	*/ 

	viewport.Width    = (float)width;
	viewport.Height   = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport on the device
	m_deviceContext->RSSetViewports(1, &viewport);

	/*
	***********************************************************
	* PROJECTION MATRIX
	***********************************************************
	* Configure the projection matrix, this is used to translate
	* the 3D scene in our 2D viewport (window), ASCamera.cpp will
	* use this viewport to allow player to move around world
	*/ 

	fov = (float)D3DX_PI / 4.0f;
	aspect = (float)width/(float)height;

	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fov, aspect, nearPlane, depth);

	/*
	***********************************************************
	* WORLD MATRIX
	***********************************************************
	* Configure the projection matrix, this contains all 
	* entities in the world, a projection matrix is able to 
	* explore the world by multiplying its positon by the world
	*/ 

	D3DXMatrixIdentity(&m_worldMatrix);

	/*
	***********************************************************
	* INTERFACE MATRIX
	***********************************************************
	* 2D Matrix used to project an interface the the foreground
	* of the projection matrix, this is referred to as a orthographic
	* matrix, but the name interface matrix is more suited to its purpose
	*/ 

	D3DXMatrixOrthoLH(&m_interfaceMatrix, (float)width, (float)height, nearPlane, depth);

	// Init was successful, return true
	return true;
}

/*
*******************************************************************
* Method: Prepare Buffers  
*******************************************************************
* Initialise the front and back buffer and prepare the drawing of
* the scene
*******************************************************************
*/

void ASDirect3D::PrepareBuffers(float r, float g, float b, float a)
{
	float color[4];

	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;

	m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

/*
*******************************************************************
* Method: Render Scene
*******************************************************************
* Tell the swapchain to swap the front and back buffers to display
* the scene
*******************************************************************
*/

void ASDirect3D::RenderScene()
{
	// Present the back buffer to the screen to show the latest scene
	if(m_vsync_enabled)
		m_swapChain->Present(1,0);	// lock to refresh rate
	else
		m_swapChain->Present(0,0);	// render as fast as possible

	return;
}

/*
*******************************************************************
* Method: Get Device
*******************************************************************
* Returns a reference to the device pointer; this allows external
* entities to interact with private members through a public interface
*******************************************************************
*/

ID3D11Device* ASDirect3D::GetDevice()
{
	return m_device;
}

/*
*******************************************************************
* Method: Get Device Context
*******************************************************************
* Returns a reference to the device context pointer; this allows external
* entities to interact with private members through a public interface
*******************************************************************
*/

ID3D11DeviceContext* ASDirect3D::GetDeviceContext()
{
	return m_deviceContext;
}

/*
*******************************************************************
* Method: Get Projection Matrix
*******************************************************************
* Returns a copy of the projection matrix; this allows external
* entities to interact with private members through a public interface
* a copy is passed back as we don't want to necessarily modify the
* the current matrix
*
* @param D3DXMATRIX& - Reference to the output parameter to be written to
*******************************************************************
*/

void ASDirect3D::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

/*
*******************************************************************
* Method: Get World Matrix
*******************************************************************
* Returns a copy of the world matrix; this allows external
* entities to interact with private members through a public interface
* a copy is passed back as we don't want to necessarily modify the
* the current matrix
*
* @param D3DXMATRIX& - Reference to the output parameter to be written to
*******************************************************************
*/

void ASDirect3D::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}

/*
*******************************************************************
* Method: Get Interface Matrix
*******************************************************************
* Returns a copy of the interface matrix; this allows external
* entities to interact with private members through a public interface
* a copy is passed back as we don't want to necessarily modify the
* the current matrix
*
* @param D3DXMATRIX& - Reference to the output parameter to be written to
*******************************************************************
*/

void ASDirect3D::GetInterfaceMatrix(D3DXMATRIX& interfaceMatrix)
{
	interfaceMatrix = m_interfaceMatrix;
}

/*
*******************************************************************
* Method: Get Video Card Info
*******************************************************************
* Returns the name of the video card, and the amount of memory
* that the video card has, used for debugging
*
* @param char* - The output name of the card, returned from 
* @param int&  - Output the number of MB the card has
*******************************************************************
*/

void ASDirect3D::GetVideoCardInfo(char* name, int& memory)
{
	strcpy_s(name, 128, m_videoCardDesc);
	memory = m_videoCard_memory;
	return;
}

/*
*******************************************************************
* Method: Release 
*******************************************************************
* Releases and cleans up all pointers used in this code, this
* is done safely by deleting them (or calling Release() on MS objects)
* and then finally setting their pointer to 0, so we know they are null
*******************************************************************
*/

void ASDirect3D::Release()
{
	// Set to windowed mode, otherwise exiting the swapchain will throw an error
	if(m_swapChain)
		m_swapChain->SetFullscreenState(false, NULL);
	// Destroy Rasteriser
	if(m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = 0;
	}
	// Destroy Depth Stencil
	if(m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}
	// Destroy Depth Stencil State
	if(m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}
	// Destroy Depth Stencil Buffer
	if(m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}
	// Destroy Render Target View
	if(m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}
	// Destroy Device Context
	if(m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}
	// Destroy Device
	if(m_device)
	{
		m_device->Release();
		m_device = 0;
	}
	// Destroy Swap Chain
	if(m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}