
/*
******************************************************************
* ASSkyShader.cpp
*******************************************************************
* Implement all methods prototyped in ASSkyShader.h
*******************************************************************
*/

#include "ASSkyShader.h"

/*
*******************************************************************
* Constructor
*******************************************************************
* Set all member values for the player object so we know our 
* player (hero) has some sort of value!
*/

ASSkyShader::ASSkyShader()
{
	m_vShader = 0;
	m_pShader = 0;
	m_iLayout = 0;
	m_cBuffer = 0;
	m_gBuffer = 0;
}

/*
*******************************************************************
* Empty consturctor
*******************************************************************
*/

ASSkyShader::ASSkyShader(const ASSkyShader& box)
{}

/*
*******************************************************************
* Destructor
*******************************************************************
*/

ASSkyShader::~ASSkyShader()
{}

/*
*******************************************************************
* METHOD: Init
*******************************************************************
* Initialise the skybox shader by setting the vertex and pixel
* shader files.
*/

bool ASSkyShader::Init(ID3D11Device* device, HWND handle, WCHAR* vs, WCHAR* ps)
{
	// Set pointers to 0 and declare other local vars
	ID3D10Blob* err	     = 0;
	ID3D10Blob* vsBuffer = 0;
	ID3D10Blob* psBuffer = 0;
	D3D11_INPUT_ELEMENT_DESC vLayout[1];
	D3D11_BUFFER_DESC cBufferDesc;
	D3D11_BUFFER_DESC gBufferDesc;
	HRESULT hr;

	// Compile the vertex shader
	hr = D3DX11CompileFromFile(vs, NULL, NULL, "ASSkyVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vsBuffer, &err, NULL);
	if(FAILED(hr))
	{
		if(err)
			RaiseShaderError(err, handle, vs);
		else
			MessageBox(handle, vs, L"Missing Vertex Shader File", MB_OK);
		return false;
	}
	// Compile the pixel shader
	hr = D3DX11CompileFromFile(ps, NULL, NULL, "ASSkyPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &psBuffer, &err, NULL);
	if(FAILED(hr))
	{
		if(err)
			RaiseShaderError(err, handle, ps);
		else
			MessageBox(handle, vs, L"Missing Pixel Shader File", MB_OK);
		return false;
	}
	// Create the vertex shader
	hr = device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, &m_vShader);
	if(FAILED(hr))
		return false;
	// create pixel shader
	hr = device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &m_pShader);
	if(FAILED(hr))
		return false;

	/*
	 * Build the input layout
	 */

	vLayout[0].SemanticName = "POSITION";
	vLayout[0].SemanticIndex = 0;
	vLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vLayout[0].InputSlot = 0;
	vLayout[0].AlignedByteOffset = 0;
	vLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vLayout[0].InstanceDataStepRate = 0;

	unsigned int numElements = sizeof(vLayout) / sizeof(vLayout[0]);
	hr = device->CreateInputLayout(vLayout, numElements, vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &m_iLayout);
	if(FAILED(hr))
		return false;

	// Dispose of local resources, if we return false later they will still exist in memory (bad!)
	vsBuffer->Release();
	psBuffer->Release();
	psBuffer = 0;
	vsBuffer = 0;

	/*
	* Describe and create the constant buffer
	*/

	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.ByteWidth = sizeof(ASMatrixBuffer);
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;
	 
	hr = device->CreateBuffer(&cBufferDesc, NULL, &m_cBuffer);
	if(FAILED(hr))
		return false;

	/*
	* Describe and create the gradient buffer
	*/

	gBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	gBufferDesc.ByteWidth = sizeof(ASGradient);
	gBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	gBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	gBufferDesc.MiscFlags = 0;
	gBufferDesc.StructureByteStride = 0;
	 
	hr = device->CreateBuffer(&gBufferDesc, NULL, &m_gBuffer);
	if(FAILED(hr))
		return false;

	return true;
}

/*
*******************************************************************
* METHOD: Render
*******************************************************************
* Renders the buffers, putting the skybox geometry into the graphics
* pipeline, redy for rendering
*/

bool ASSkyShader::RenderSky(ID3D11DeviceContext* deviceCtx, int numIndices, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection,
							D3DXVECTOR4 skyColorA, D3DXVECTOR4 skyColorB)
{
	// Attempt to initialise the shader
	bool success = SetShaderParameters(deviceCtx, world, view, projection, skyColorA, skyColorB);
	if(!success)
		return false;

	// If shader was set succesfully, render the sky
	deviceCtx->IASetInputLayout(m_iLayout);
	deviceCtx->VSSetShader(m_vShader, NULL, 0);
	deviceCtx->PSSetShader(m_pShader, NULL, 0);
	deviceCtx->DrawIndexed(numIndices, 0, 0);
	
	return true;
}

/*
******************************************************************
* METHOD: Set Shader Parameters
******************************************************************
* Sets the global variables for the shader by using pointers 
* that have been intialised in the InitShader() method.  
* These vars are then used by the ASGraphics class to draw new
* objects to the screen in ASGraphics::RenderFrame()
*
* @param ID3D11DeviceContext* - The device we are using
* @param D3DXMATRIX  - the world matrix our objects live in
* @param D3DXMATRIX  - the view matrix (what we currently see)
* @param D3DXMATRIX  - the projection matrix
* @param D3DXVECTOR4 - the skys base color
* @param D3DXVECTOR4 - the skys highlight color
*
* @return bool - True if the shader was set, else false
*/

bool ASSkyShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, 
								      D3DXVECTOR4 skyColorA, D3DXVECTOR4 skyColorB)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE res;

	// Buffer pointers to be passed to the shader
	ASMatrixBuffer* cBuffer;
	ASGradient*     gBuffer;

	unsigned int bufferNum;

	// Transpose matrices and prepare them for shader
	D3DXMatrixTranspose(&world, &world);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&projection, &projection);

	/*
	* CONSTANT BUFFER
	*/
	// Lock the constant buffer so it can be written to
	hr = deviceContext->Map(m_cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	if(FAILED(hr))
		return false;

	// Attain a refernece to the constant buffer data
	cBuffer = (ASMatrixBuffer*)res.pData;

	// Write the matrices to the constant buffer to be processed by the shader
	cBuffer->world = world;
	cBuffer->view  = view;
	cBuffer->projection = projection;

	// Unlock the constant buffer - it can now be acquired for write by another process
	deviceContext->Unmap(m_cBuffer, 0);

	// Set the position of the constant buffer in the vertex shader
	deviceContext->VSSetConstantBuffers(0, 1, &m_cBuffer);
	
	/*
	* GRADIENT BUFFER
	*/
	// Lock the gradient buffer so that it can be written to
	hr = deviceContext->Map(m_gBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	if(FAILED(hr))
		return false;

	// Attain a reference to the light buffer data
	gBuffer = (ASGradient*)res.pData;

	// Write the info from the buffer to m_lBuffer to be processed by the Vertex and Pixel shaders,
	// also add padding to ensure the struct remains a multiple of 16 - this struct is 8 bytes
	gBuffer->skyColorA = skyColorA;
	gBuffer->skyColorB = skyColorB;

	// Release the buffer, it can now be acquired for write by another process
	deviceContext->Unmap(m_gBuffer, 0);

	// Set the position of the constant buffer in the pixel shader - this will then be used for drawing
	// at the next stage of the render pipeline
	deviceContext->PSSetConstantBuffers(0, 1, &m_gBuffer);

	return true;
}

/*
*******************************************************************
* METHOD: Raise Shader Error
*******************************************************************
* Logs a shader error to the shader error log
*/

void ASSkyShader::RaiseShaderError(ID3D10Blob* msg, HWND handle, WCHAR* shaderName)
{
	ofstream fout;

	char* err = (char*)(msg->GetBufferPointer());
	unsigned long bSize = msg->GetBufferSize();

	// Open a file to write out to
	fout.open("./log/shader-error.txt");
	for(unsigned int i = 0; i < bSize; i++)
		fout << err[i];

	// Close the file reader and dispose of the msg object (we don't need it anymore)
	fout.close();

	msg->Release();
	msg = 0;

	// Create a message dialog for the user
	MessageBox(handle, L"Error when compiling the shader.  Please consult shader-error.txt for more details.", shaderName, MB_OK);
}

/*
*******************************************************************
* METHOD: Release
*******************************************************************
* Release all items held by the class and set their pointers to 
* null
*/

void ASSkyShader::Release()
{
	// Dispose of index buffer
	if(m_cBuffer)
	{
		m_cBuffer->Release();
		m_cBuffer = 0;
	}
	// Dispose of input layout
	if(m_iLayout)
	{
		m_iLayout->Release();
		m_iLayout = 0;
	}
	// Dispose of gradient buffer
	if(m_gBuffer)
	{
		m_gBuffer->Release();
		m_gBuffer = 0;
	}
	// Dispose of pixel shader
	if(m_pShader)
	{
		m_pShader->Release();
		m_pShader = 0;
	}
	// Dispose of vertex shader
	if(m_vShader)
	{
		m_vShader->Release();
		m_vShader = 0;
	}
}


