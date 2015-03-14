
/*
******************************************************************
* ASFont.cpp
******************************************************************
* Implement all function prototype in ASTextureShader.h
*/

#include "ASText.h"

/*
******************************************************************
* Consturctor
******************************************************************
* Set all pointers to D3D objects to null, these will be properly
* initialised later
*/

ASText::ASText()
{
	m_font = 0;
	m_sentence = 0;
	m_fontShader = 0;
}

/*
******************************************************************
* Empty constructor
******************************************************************
*/

ASText::ASText(const ASText& shader)
{}

/*
******************************************************************
* Destructor
******************************************************************
*/

ASText::~ASText()
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

bool ASText::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND handle, int width, int height, D3DXMATRIX viewMatrix)
{
	m_width = width;
	m_height = height;
	m_viewMatrix = viewMatrix;

	// Create the font object
	m_font = new ASFont;
	if(!m_font)
		return false;

	bool success = m_font->Init(device, "./font/fontdata.txt", L"./font/font.dds");
	if(!success) {
		MessageBox(handle, L"Error when initialisng the font object", L"Error", MB_OK);
		return false;
	}

	// Create the font shader object
	m_fontShader = new ASFontShader;
	if(!m_fontShader)
		return false;
	success = m_fontShader->Init(device, handle);
	if(!success)
	{
		MessageBox(handle, L"Error when initialising the font shader.", L"Error", MB_OK);
		return false;
	}

	// Initialise the sentence
	success = InitSentence(&m_sentence, 16, device);
	if(!success)
		return false;

	// Update the sentence vertex buffer
	success = UpdateSentence(m_sentence, "Hi", 100, 100, 1.0f, 1.0f, 1.0f, deviceContext);
	if(!success)
		return false;

	return true;
}

/*
******************************************************************
* METHOD: Release
******************************************************************
* Calls the private implementation of the ReleaseShader() method
*/

void ASText::Release() 
{
	// release the sentence structure
	ReleaseSentence(&m_sentence);
	// Delete the font shader
	if(m_fontShader)
	{
		m_fontShader->Release();
		delete m_fontShader;
		m_fontShader = 0;
	}
	// Delete the font object
	if(m_font)
	{
		m_font->Release();
		delete m_font;
		m_font = 0;
	}
}

/*
******************************************************************
* METHOD: Render
******************************************************************
* Sets the shaders parameters and then calls RenderShader()
* to draw the model using the HLSL shader.
*
* @param ID3D11DeviceContext* - the device we are using to render
* @param D3DMATRIX - the world matrix
* @param D3DMATRIX - orthographic matrix to project onto the screen
*
* @return bool - True if the text was rendered successfully, else false
*/

bool ASText::Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX world, D3DXMATRIX ortho)
{
	bool success = RenderSentence(deviceContext, m_sentence, world, ortho);
	if(!success)
		return false;
	
	return true;
}

/*
******************************************************************
* METHOD: Init Shader
******************************************************************
* Initialises the sentence by creating an empty vertex buffer
* and then populating with the inbound data contained in the string
*/

bool ASText::InitSentence(ASSentence** sentence, int maxLen, ID3D11Device* device)
{
	HRESULT hr;

	ASVertex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vBufferDesc;
	D3D11_BUFFER_DESC iBufferDesc;
	D3D11_SUBRESOURCE_DATA vData;
	D3D11_SUBRESOURCE_DATA iData;

	// Build the new sentence object
	*sentence = new ASSentence;
	if(!*sentence)
		return false;

	// Set the sentence buffers
	(*sentence)->vBuffer = 0;
	(*sentence)->iBuffer = 0;
	(*sentence)->maxLen = maxLen;
	(*sentence)->numVerts = 6 * maxLen;
	(*sentence)->numIndices = (*sentence)->numVerts;
	
	// Create vertex array
	vertices = new ASVertex[(*sentence)->numVerts];
	if(!vertices)
		return false;

	// Create the indice array
	indices = new unsigned long[(*sentence)->numVerts];
	if(!indices)
		return false;

	// Initialise vertex and index arrays
	memset(vertices, 0, (sizeof(ASVertex) * (*sentence)->numVerts));
	for(unsigned int i = 0; i < (*sentence)->numIndices; i++)
		indices[i] = i;	// index arrays should be contiguous

	// Set up the description of the dynamic vertex buffer.
	vBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vBufferDesc.ByteWidth = sizeof(ASVertex) * (*sentence)->numVerts;
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vBufferDesc.MiscFlags = 0;
	vBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vData.pSysMem = vertices;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	hr = device->CreateBuffer(&vBufferDesc, &vData, &(*sentence)->vBuffer);
	if(FAILED(hr))
		return false;

	// Set up the description of the static index buffer.
	iBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	iBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->numIndices;
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.CPUAccessFlags = 0;
	iBufferDesc.MiscFlags = 0;
	iBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	iData.pSysMem = indices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	// Create the index buffer.
	hr = device->CreateBuffer(&iBufferDesc, &iData, &(*sentence)->iBuffer);
	if(FAILED(hr))
		return false;

	// Release the vertex array as it is no longer needed.
	delete [] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete [] indices;
	indices = 0;

	return true;
}

/*
******************************************************************
* METHOD: Update Sentence
******************************************************************
* Changes the contents of the vertex buffer for the sentence, allowing
* its contents to be updated on the fly
*
* @param ASSentence* - pointer to the sentence struct to update its buffers
* @param char*       - pointer to the text that is to updated in the buffer
* @param int         - new x position on the screen for text
* @param int         - new y position on the screen for text
* @param float		 - red component of text
* @param float       - green component of text
* @param float       - blue component of text
* @param float       - alpha component of text
* @param ID3D11DeviceContext* - pointer to the device context
* 
* @return bool - true if the sentence update successfully, else false
*/

bool ASText::UpdateSentence(ASSentence* sentence, char* text, int posX, int posY, float r, float g, float b, ID3D11DeviceContext* deviceContext)
{
	HRESULT hr;

	int numLetters;
	ASVertex* vertices;
	ASVertex* vertPtr;
	float drawX;
	float drawY;
	D3D11_MAPPED_SUBRESOURCE res;

	// Set the new size and color of the sentence
	sentence->r = r;
	sentence->g = g;
	sentence->b = b;
	
	// get num lines of sentence
	numLetters = (int)strlen(text);

	// Check for possible buffer overflow
	if(numLetters > sentence->maxLen)
		return false;

	// Create vertex array
	vertices = new ASVertex[sentence->numVerts];
	if(!vertices)
		return false;

	// Set new position on screen for the sentnece
	drawX = posX;
	drawY = posY;

	// Build the vertex array using ASFont
	m_font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// Lock the vBuffer so we can write to it
	hr = deviceContext->Map(sentence->vBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	if(FAILED(hr))
		return hr;

	// Get pointer to the data in the vertex buffer
	vertPtr = (ASVertex*)res.pData;

	// Update the vertex buffer
	memcpy(vertPtr, (void*)vertices, (sizeof(ASVertex) * sentence->numVerts));

	// Unlock the buffer for future use
	deviceContext->Unmap(sentence->vBuffer, 0);

	// Release the vertex array
	delete [] vertices;
	vertices = 0;

	return true;
}

/*
******************************************************************
* METHOD: Release Sentence
******************************************************************
* Disposes of any resources used by the sentence buffers
*/

void ASText::ReleaseSentence(ASSentence** sentence)
{
	// Dispose of any setnence resources
	if(*sentence)
	{
		// Release setnence vertex buffer
		if((*sentence)->vBuffer)
		{
			(*sentence)->vBuffer->Release();
			(*sentence)->vBuffer = 0;
		}
		// Release index buffer
		if((*sentence)->iBuffer)
		{
			(*sentence)->iBuffer->Release();
			(*sentence)->iBuffer = 0;
		}
		// All objects disposed - delete the sentence and set a null ptr
		delete *sentence;
		*sentence = 0;
	}
}

/*
******************************************************************
* METHOD: Render Sentence
******************************************************************
* 
*/

bool ASText::RenderSentence(ID3D11DeviceContext* deviceContext, ASSentence* sentence, D3DXMATRIX world, D3DXMATRIX ortho)
{
	unsigned int stride;
	unsigned int offset;
	D3DXVECTOR4 pixelColor;
	bool success;

	// Set vertex buffer 
	stride = sizeof(ASVertex);
	offset = 0;

	// Set buffers
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(sentence->iBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	// Set pixel color
	pixelColor = D3DXVECTOR4(sentence->r, sentence->g, sentence->b, 1.0f);

	// Render the text
	success = m_fontShader->Render(deviceContext, sentence->numIndices, world, m_viewMatrix, ortho, m_font->GetTexture(), pixelColor);
	if(!success)
		return false;

	return true;
}
