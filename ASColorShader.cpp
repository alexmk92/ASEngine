
/*
******************************************************************
* ASColorShader.cpp
******************************************************************
* Implement all function prototype in ASColorShader.h
*/

#include "ASColorShader.h"

/*
******************************************************************
* Consturctor
******************************************************************
* Set all pointers to D3D objects to null, these will be properly
* initialised later
*/

ASColorShader::ASColorShader()
{
	m_vShader = 0;
	m_pShader = 0;
	m_layout  = 0;
	m_mBuffer = 0;
}

/*
******************************************************************
* Empty constructor
******************************************************************
*/

ASColorShader::ASColorShader(const ASColorShader& shader)
{}

/*
******************************************************************
* Destructor
******************************************************************
*/

ASColorShader::~ASColorShader()
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

bool ASColorShader::Init(ID3D11Device* device, HWND handle)
{
	bool success = InitShader(device, handle, L"./ASColor.vs", L"./ASColor.ps");
	if(!success)
		return false;
	else
		return true;
}

/*
******************************************************************
* METHOD: Release
******************************************************************
* Calls the private implementation of the ReleaseShader() method
*/

void ASColorShader::Release() 
{
	ReleaseShader();
	return;
}

/*
******************************************************************
* METHOD: Render
******************************************************************
* Sets the shaders parameters and then calls RenderShader()
* to draw the model using the HLSL shader.
*
* @param ID3D11DeviceContext* - the device we are using to render
* @param int - The number of indices for the model
* @param D3DXMATRIX - The World Matrix
* @param D3DXMATRIX - The View Matrix
* @param D3DXMATRIX - The Projection Matrix
*
* @return bool - True if the shaders were set and sent successfully, else false
*/

bool ASColorShader::Render(ID3D11DeviceContext* deviceContext, int numIndices,
						    D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection)
{
	bool success = SetShaderParameters(deviceContext, world, view, projection);
	if(!success)
		return false;
	
	RenderShader(deviceContext, numIndices);
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

bool ASColorShader::InitShader(ID3D11Device* device, HWND handle, WCHAR* vsFile, WCHAR* psFile)
{
	HRESULT hr;

	ID3D10Blob* err;
	ID3D10Blob* vsBuffer;
	ID3D10Blob* psBuffer;

	D3D11_BUFFER_DESC mBufferDesc;
	D3D11_INPUT_ELEMENT_DESC polyLayout[2];
	unsigned int numElements;

	// Compile shaders into buffers - any errors are caught and pushed to an error stack, if the 
	// file is not found user is alerted by a popup message
	hr = D3DX11CompileFromFile(vsFile, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 
							   0, NULL, &vsBuffer, &err, NULL);
	if(FAILED(hr))
	{
		if(err)
			ReportShaderError(err, handle, vsFile);
		else 
			MessageBox(handle, vsFile, L"Could not find the vertex shader file, please check to see if it exists.", MB_OK);
		return false;
	}

	// Compile the pixel shader
	hr = D3DX11CompileFromFile(psFile, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS,
							  0, NULL, &psBuffer, &err, NULL);
	if(FAILED(hr))
	{
		if(err)
			ReportShaderError(err, handle, psFile);
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
	polyLayout[0].SemanticName  = "POSITION";
	polyLayout[0].SemanticIndex = 0;
	polyLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polyLayout[0].InputSlot = 0;
	polyLayout[0].AlignedByteOffset = 0;
	polyLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polyLayout[0].InstanceDataStepRate = 0;

	polyLayout[1].SemanticName  = "COLOR";
	polyLayout[1].SemanticIndex = 0;
	polyLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polyLayout[1].InputSlot = 0;
	polyLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polyLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polyLayout[1].InstanceDataStepRate = 0;

	// Create the input layout ont he device using our rendering device.
	numElements = sizeof(polyLayout) / sizeof(polyLayout[0]);
	hr = device->CreateInputLayout(polyLayout, numElements, vsBuffer->GetBufferPointer(),
								   vsBuffer->GetBufferSize(), &m_layout);
	if(FAILED(hr))
		return false;

	// Release the buffers and set null pointers to them as we have loaded the data into pointers
	vsBuffer->Release();
	psBuffer->Release();
	vsBuffer = 0;
	psBuffer = 0;

	// Describe the constant buffer to pass to the Vertex Shader - the CB is used to talk directly to the shader
	mBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	mBufferDesc.ByteWidth = sizeof(ASMatrixBuffer);
	mBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mBufferDesc.MiscFlags = 0;
	mBufferDesc.StructureByteStride = 0;

	hr = device->CreateBuffer(&mBufferDesc, NULL, &m_mBuffer);
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

void ASColorShader::ReportShaderError(ID3D10Blob* msg, HWND handle, WCHAR* shaderName)
{
	ofstream fout;

	char* err = (char*)(msg->GetBufferPointer());
	unsigned long bSize = msg->GetBufferSize();

	// Open a file to write out to
	fout.open("shader-error.txt");
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
* @param D3DXMATRIX - the world matrix our objects live in
* @param D3DXMATRIX - the view matrix (what we currently see)
* @param D3DXMATRIX - the projection matrix
* 
* @return bool - True if the shader was set, else false
*/

bool ASColorShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX world, D3DXMATRIX view, D3DXMATRIX projection)
{
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE res;
	ASMatrixBuffer* shader;
	unsigned int bufferNum;

	// Transpose matrices and prepare them for shader
	D3DXMatrixTranspose(&world, &world);
	D3DXMatrixTranspose(&view, &view);
	D3DXMatrixTranspose(&projection, &projection);

	// Lock the constant buffer so it can be written to
	hr = deviceContext->Map(m_mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	if(FAILED(hr))
		return false;

	// Attain a refernece to the constant buffer data
	shader = (ASMatrixBuffer*)res.pData;

	// Write the matrices to the constant buffer to be processed by the shader
	shader->world = world;
	shader->view  = view;
	shader->projection = projection;

	// Unlock the constant buffer - it can now be acquired for write by another process
	deviceContext->Unmap(m_mBuffer, 0);

	// Set the position of the constant buffer in the vertex shader, and then update the VS with the CB values
	bufferNum = 0;
	deviceContext->VSSetConstantBuffers(bufferNum, 1, &m_mBuffer);
	
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

void ASColorShader::RenderShader(ID3D11DeviceContext* deviceContext, int numIndices)
{
	// Set the input layout, vertex shader and pixel shader
	deviceContext->IASetInputLayout(m_layout);
	deviceContext->VSSetShader(m_vShader, NULL, 0);
	deviceContext->PSSetShader(m_pShader, NULL, 0);

	// Render the model
	deviceContext->DrawIndexed(numIndices, 0, 0);

	return;
}

/*
******************************************************************
* METHOD: Release Shader
******************************************************************
* Disposes of any resources used by the shader and then sets a null
* pointer to each of the member variables so we can't access it
*/

void ASColorShader::ReleaseShader()
{
	// Destroy constant buffer
	if(m_mBuffer)
	{
		m_mBuffer->Release();
		m_mBuffer = 0;
	}
	// Destroy the input layout
	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
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
