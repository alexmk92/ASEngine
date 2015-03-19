
/*
******************************************************************
* ASTextureShader.cpp
******************************************************************
* Implement all function prototype in ASTextureShader.h
*/

#include "ASTerrainShader.h"

/*
******************************************************************
* Consturctor
******************************************************************
* Set all pointers to D3D objects to null, these will be properly
* initialised later
*/

ASTerrainShader::ASTerrainShader()
{
	m_vShader     = 0;
	m_pShader     = 0;
	m_iLayout	  = 0;
	m_cBuffer	  = 0;
	m_sampleState = 0;
	m_lBuffer     = 0;
	m_camBuffer   = 0;
}

/*
******************************************************************
* Empty constructor
******************************************************************
*/

ASTerrainShader::ASTerrainShader(const ASTerrainShader& shader)
{}

/*
******************************************************************
* Destructor
******************************************************************
*/

ASTerrainShader::~ASTerrainShader()
{}

/*
******************************************************************
* METHOD: Init
******************************************************************
* Calls the private InitShader method to initialise the shader
*
* @param ID3D11Device* - The device we are rendering with
* @param HWND - The handle to application window
*
* @return bool - True if the shader initialised successfully, else false
*/

bool ASTerrainShader::Init(ID3D11Device* device, HWND handle)
{
	// Load vertex and pixel shaders from source file, these will be compiled via HLSL in
	// the InitShader method
	bool success = InitShader(device, handle, L"./ASTerrainVS.hlsl", L"./ASTerrainPS.hlsl");
	if(!success)
		return false;
	else
		return true;
}

/*
******************************************************************
* METHOD: Init Shader
******************************************************************
* Loads the shader files and passes them to the GPU. This also defines
* the layout of data to be passed to the rendering pipeline (GPU)
* The InputLayout will match that of the structure in the Vertex Shader
*
* @param ID3D11Device* - The device we are rendering with
* @param HWND - Handle to the application window we are using 
* @param WCHAR* - The vertex shader path appended with filename
* @param WCHAR* - The pixel shader path appended with filename
*
* @return bool - True if shader init successfully, else false
*/

bool ASTerrainShader::InitShader(ID3D11Device* device, HWND handle, WCHAR* vsFile, WCHAR* psFile)
{
	HRESULT hr;

	// Init pointers to null
	ID3D10Blob* err = 0;
	ID3D10Blob* vsBuffer = 0;
	ID3D10Blob* psBuffer = 0;

	// Other descriptors for buffer
	D3D11_INPUT_ELEMENT_DESC polyLayout[4];	

	D3D11_BUFFER_DESC  cBufferDesc;
	D3D11_BUFFER_DESC  lightBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	unsigned int numElements;

	// Compile shaders into buffers - any errors are caught and pushed to an error stack, if the 
	// file is not found user is alerted by a popup message
	hr = D3DX11CompileFromFile(vsFile, NULL, NULL, "TerrainVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
							   0, NULL, &vsBuffer, &err, NULL);
	if(FAILED(hr))
	{
		if(err)
			RaiseShaderError(err, handle, vsFile);
		else 
			MessageBox(handle, vsFile, L"Could not find the vertex shader file, please check to see if it exists.", MB_OK);

		return false;
	}

	// Compile the pixel shader
	hr = D3DX11CompileFromFile(psFile, NULL, NULL, "TerrainPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
							  0, NULL, &psBuffer, &err, NULL);
	if(FAILED(hr))
	{
		if(err)
			RaiseShaderError(err, handle, psFile);
		else
			MessageBox(handle, vsFile, L"Could not find the pixel shader file, please check to see if it exists.", MB_OK);
		return false;
	}

	// Create shader objects from the pixel and vertex buffers. These pointers will be used to interact with the 
	// vertex and pixel shaders
	hr = device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), NULL, &m_vShader);
	if(FAILED(hr))
		return false;
	hr = device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), NULL, &m_pShader);
	if(FAILED(hr))
		return false;

	// Create the input layout of the vertex data. We define how far apart in memory vertex and pixel data sit,
	// in this case every 12 bytes are position and every 16 bytes after that will be color - this tells the GPU
	// the structure of the input, so it knows to process vertex information for 12 byes and then color information 
	// for 16 bytes after... 12 bytes as a triangle has 3 vertices of FLOAT = 3*4 = 12.  16 bytes color = float = rgba = 4*4 = 16
	// The structure here directly maps that found in the vertex shader , order of declaration matters
	polyLayout[0].SemanticName  = "POSITION";
	polyLayout[0].SemanticIndex = 0;
	polyLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polyLayout[0].InputSlot = 0;
	polyLayout[0].AlignedByteOffset = 0;
	polyLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polyLayout[0].InstanceDataStepRate = 0;

	// Describe the tex coord information, this will be loaded into the input layout var
	polyLayout[1].SemanticName = "TEXCOORD";
	polyLayout[1].SemanticIndex = 0;
	polyLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polyLayout[1].InputSlot = 0;
	polyLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polyLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polyLayout[1].InstanceDataStepRate = 0;

	// Describe the vertex normal information, this will be loaded into the input layout var
	polyLayout[2].SemanticName = "NORMAL";
	polyLayout[2].SemanticIndex = 0;
	polyLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polyLayout[2].InputSlot = 0;
	polyLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polyLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polyLayout[2].InstanceDataStepRate = 0;

	// Set the color vertex information
	polyLayout[3].SemanticName = "COLOR";
	polyLayout[3].SemanticIndex = 0;
	polyLayout[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polyLayout[3].InputSlot = 0;
	polyLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polyLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polyLayout[3].InstanceDataStepRate = 0;

	// Create the input layout ont he device using our rendering device.
	numElements = sizeof(polyLayout) / sizeof(polyLayout[0]);
	hr = device->CreateInputLayout(polyLayout, numElements, vsBuffer->GetBufferPointer(),
								   vsBuffer->GetBufferSize(), &m_iLayout);
	if(FAILED(hr))
		return false;

	// Release the buffers and set null pointers to them as we have loaded the data into pointers
	vsBuffer->Release();
	psBuffer->Release();
	vsBuffer = 0;
	psBuffer = 0;

	// Create a texture sampler to map the texture to the object
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler by reading values from samplerDesc into m_sampleState
	hr = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if(FAILED(hr))
		return false;

	// Describe the constant buffer to pass to the Vertex Shader - the CB is used to talk directly to the shader
	cBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cBufferDesc.ByteWidth = sizeof(ASConstantBuffer);
	cBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBufferDesc.MiscFlags = 0;
	cBufferDesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&cBufferDesc, NULL, &m_cBuffer);
	if(FAILED(hr))
		return false;

	// Describe the light constant buffer, this will process the diffuse color and direction of the light.  
	// Buffers are set as a multiple of 16bytes to ensure DX does not throw any memory alloc errors
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(ASLightBuffer);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Map the described light buffer to the class light buffer pointer
	hr = device->CreateBuffer(&lightBufferDesc, NULL, &m_lBuffer);
	if(FAILED(hr))
		return false;

	return true;
}

/*
******************************************************************
* METHOD: Report Shader Error
******************************************************************
* Displays error messages that the compiler generates when trying
* to load Pixel or Vertex shaders from their files
*/

void ASTerrainShader::RaiseShaderError(ID3D10Blob* msg, HWND handle, WCHAR* shaderName)
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

	return;
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
* @param D3DXVECTOR4 - the ambient color of the light
* @param D3DXVECTOR4 - the diffuse color of the light
* @param D3DXVECTOR3 - The vector holding direction of light
* @param ID3D11ShaderResourceView* - Pointer to the texture 
*
* @return bool - True if the shader was set, else false
*/

bool ASTerrainShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection, 
										  D3DXVECTOR4 ambient, D3DXVECTOR4 diffuse, D3DXVECTOR3 lightDir, 
										  vector<ID3D11ShaderResourceView*> textures)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE res;

	// Buffer pointers to be passed to the shader
	ASConstantBuffer* cBuffer;
	ASLightBuffer*    lBuffer;

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
	cBuffer = (ASConstantBuffer*)res.pData;

	// Write the matrices to the constant buffer to be processed by the shader
	cBuffer->world = world;
	cBuffer->view  = view;
	cBuffer->projection = projection;

	// Unlock the constant buffer - it can now be acquired for write by another process
	deviceContext->Unmap(m_cBuffer, 0);

	// Set the position of the constant buffer in the vertex shader
	deviceContext->VSSetConstantBuffers(0, 1, &m_cBuffer);
	
	/*
	* LIGHT BUFFER
	*/
	// Lock the light buffer so that it can be written to
	hr = deviceContext->Map(m_lBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	if(FAILED(hr))
		return false;

	// Attain a reference to the light buffer data
	lBuffer = (ASLightBuffer*)res.pData;

	// Write the info from the buffer to m_lBuffer to be processed by the Vertex and Pixel shaders,
	// also add padding to ensure the struct remains a multiple of 16 - this struct is 8 bytes
	lBuffer->ambient  = ambient;
	lBuffer->diffuse  = diffuse;
	lBuffer->lightDir = lightDir;
	lBuffer->padding  = 0.0f;

	// Release the buffer, it can now be acquired for write by another process
	deviceContext->Unmap(m_lBuffer, 0);

	// Set the position of the constant buffer in the pixel shader - this will then be used for drawing
	// at the next stage of the render pipeline
	deviceContext->PSSetConstantBuffers(0, 1, &m_lBuffer);

	// Loop through each of the textures and set it in the shader
	for(int i = 0; i < textures.size(); i++)
		deviceContext->PSSetShaderResources(i, 1, &textures.at(i));

	return true;
}

/*
******************************************************************
* METHOD: Render Shader
******************************************************************
* Sets the input layout for the assembler, this describes the format
* of the data to the shader so it knows how to process it.  
* This method also sets the vertex and pixel shaders on the device
*
* @param ID3D11DeviceContext* - The device we are using
* @param int - the number of indices in the model
*/

void ASTerrainShader::RenderShader(ID3D11DeviceContext* deviceContext, int numIndices)
{
	// Set the input layout, vertex shader and pixel shader
	deviceContext->IASetInputLayout(m_iLayout);
	deviceContext->VSSetShader(m_vShader, NULL, 0);
	deviceContext->PSSetShader(m_pShader, NULL, 0);
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the model
	deviceContext->DrawIndexed(numIndices, 0, 0);

	return;
}

/*
******************************************************************
* METHOD: Release
******************************************************************
* Calls the private implementation of the ReleaseShader() method
*/

void ASTerrainShader::Release() 
{
	// Destroy the sampler
	if(m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}
	// Destroy the light buffer
	if(m_lBuffer)
	{
		m_lBuffer->Release();
		m_lBuffer = 0;
	}
	// Destroy constant buffer
	if(m_cBuffer)
	{
		m_cBuffer->Release();
		m_cBuffer = 0;
	}
	// Destroy the input layout
	if(m_iLayout)
	{
		m_iLayout->Release();
		m_iLayout = 0;
	}
	// Destroy the pixel shader
	if(m_pShader)
	{
		m_pShader->Release();
		m_pShader = 0;
	}
	// Destroy the vertex shader
	if(m_vShader)
	{
		m_vShader->Release();
		m_vShader = 0;
	}
	return;
}