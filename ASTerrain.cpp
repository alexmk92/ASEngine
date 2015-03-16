
/*
******************************************************************
* ASTerrain.cpp
*******************************************************************
* Implement all methods prototyped in ASTerrain.h
*******************************************************************
*/

#include "ASTerrain.h"

/*
*******************************************************************
* Constructor
*******************************************************************
* Set all member values for the player object so we know our 
* player (hero) has some sort of value!
*/

ASTerrain::ASTerrain()
{
	m_height      = 0;
	m_width       = 0;
	m_numIndices  = 0;
	m_numVertices = 0;
	m_iBuffer     = 0;
	m_vBuffer     = 0;
}

/*
*******************************************************************
* Empty consturctor
*******************************************************************
*/

ASTerrain::ASTerrain(const ASTerrain&)
{}

/*
*******************************************************************
* Destructor
*******************************************************************
*/

ASTerrain::~ASTerrain()
{}

/*
*******************************************************************
* METHOD: Init
*******************************************************************
* Initialise the terrain objects by setting default values for
* the size of the plane, this method will also call private 
* interface for initialising the index and vertex buffers
*
* @param ID3D11Device* - Pointer to the rendering device
* @return bool - True if successfully intiialised, else false
*/

bool ASTerrain::Init(ID3D11Device* device)
{
	// Width and height of the terrain (its a 2D plane across the Y axis)
	m_width  = 400;
	m_height = 400;

	// Initialise the buffers through the private interface, return the callback
	return InitBuffers(device);
}

/*
*******************************************************************
* METHOD: Init Buffers
*******************************************************************
* Initialises the index and vertex buffers for the terrain
*
* @param ID3D11Device* - Pointer to the rendering device
* @return bool - True if successfully intiialised, else false
*/

bool ASTerrain::InitBuffers(ID3D11Device* device)
{
	HRESULT hr;

	// Used to track the vertices and indices of the mesh
	ASVertex*      vertices;
	unsigned long* indices;

	// Position of the terrain in 3D space
	float posX = 0;
	float posZ = 0;

	// Buffers to be described to be sent to the rendering pipeline
	D3D11_BUFFER_DESC vBufferDesc;
	D3D11_BUFFER_DESC iBufferDesc;

	// Data to be sent to the rendering pipeline
	D3D11_SUBRESOURCE_DATA vData;
	D3D11_SUBRESOURCE_DATA iData;

	// Get the number of vertices and indices for the mesh, -1 from each
	// height and width because C++ is 0 indexed, then * 8 to the total
	// (each square drawn uses eight vertices to create a square), this
	// is resembled in the loop below
	m_numVertices = (m_width - 1) * (m_height - 1) * 8;
	m_numIndices  = m_numVertices;

	// Create the veritce and indice arrays to populate buffers
	vertices = new ASVertex[m_numVertices];
	if(!vertices)
		return false;
	// Use long instead of int as meshes can become very large
	indices = new unsigned long[m_numIndices];
	if(!indices)
		return false;

	// 
	int currIndex = 0;

	// Populate the vertice and indice arrays with information for the terrain
	for(unsigned int i = 0; i < (m_height - 1); i++)
	{
		for(unsigned int j = 0; j < (m_width - 1); j++)
		{
			// Vertice 1 - Upper left
			posX = (float)j;
			posZ = (float)i++;

			vertices[currIndex].pos = D3DXVECTOR3(posX, 0.0f, posZ);
			vertices[currIndex].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[currIndex] = currIndex;
			currIndex++;

			// Upper right
			posX = (float)j++;
			posZ = (float)i++;

			vertices[currIndex].pos = D3DXVECTOR3(posX, 0.0f, posZ);
			vertices[currIndex].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[currIndex] = currIndex;
			currIndex++;

			// Vertice 2 - Upper right
			posX = (float)j++;
			posZ = (float)i++;

			vertices[currIndex].pos = D3DXVECTOR3(posX, 0.0f, posZ);
			vertices[currIndex].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[currIndex] = currIndex;
			currIndex++;

			// bottom right
			posX = (float)j+1;
			posZ = (float)i;

			vertices[currIndex].pos = D3DXVECTOR3(posX, 0.0f, posZ);
			vertices[currIndex].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[currIndex] = currIndex;
			currIndex++;

			// Vertice 3 - Bottom right
			posX = (float)j+1;
			posZ = (float)i;

			vertices[currIndex].pos = D3DXVECTOR3(posX, 0.0f, posZ);
			vertices[currIndex].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[currIndex] = currIndex;
			currIndex++;

			// Bottom left
			posX = (float)j;
			posZ = (float)i;

			vertices[currIndex].pos = D3DXVECTOR3(posX, 0.0f, posZ);
			vertices[currIndex].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[currIndex] = currIndex;
			currIndex++;

			// Vertice 4 bottom left
			posX = (float)j;
			posZ = (float)i;

			vertices[currIndex].pos = D3DXVECTOR3(posX, 0.0f, posZ);
			vertices[currIndex].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[currIndex] = currIndex;
			currIndex++;

			// upper left
			posX = (float)j;
			posZ = (float)i++;

			vertices[currIndex].pos = D3DXVECTOR3(posX, 0.0f, posZ);
			vertices[currIndex].color = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
			indices[currIndex] = currIndex;
			currIndex++;
		}
	}

	// Describe the vertex and index arrays 
	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth = sizeof(ASVertex) * m_numVertices;
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags = 0;
	vBufferDesc.MiscFlags = 0;
	vBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data, then create
	// the vertex buffer to be passed to the device
	vData.pSysMem = vertices;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&vBufferDesc, &vData, &m_vBuffer);
	if(FAILED(hr))
		return false;

	// Describe the index buffer
	// Set up the description of the static index buffer.
	iBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	iBufferDesc.ByteWidth = sizeof(unsigned long) * m_numIndices;
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.CPUAccessFlags = 0;
	iBufferDesc.MiscFlags = 0;
	iBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data, then create the
	// index buffer to be passed to the device
	iData.pSysMem = indices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	// Create the index buffer.
	hr = device->CreateBuffer(&iBufferDesc, &iData, &m_iBuffer);
	if(FAILED(hr))
		return false;

	// Release the local buffer descriptors
	delete [] vertices;
	delete [] indices;
	vertices = 0;
	indices  = 0;

	// Everything was successful
	return true;
}

/*
*******************************************************************
* METHOD: Render
*******************************************************************
* Calls the private RenderBuffers interface 
*
* @param ID3D11DeviceContext* - ptr to the device context we are working with
*/

void ASTerrain::Render(ID3D11DeviceContext* deviceCtxt)
{
	RenderBuffers(deviceCtxt);
}

/*
*******************************************************************
* METHOD: Render Buffers
*******************************************************************
* Send the buffers to the graphics pipe line to be drawn to the 
* scene, the drawing is handled in ASEngine 
*
* @param ID3D11DeviceContext* - ptr to the device we are using to render
*/

void ASTerrain::RenderBuffers(ID3D11DeviceContext* deviceCtxt)
{
	// Describe how the buffers are to be mapped in device mem
	unsigned int stride = sizeof(ASVertex);
	unsigned int offset = 0;

	// Send the vertex and index buffers to the device
	deviceCtxt->IASetVertexBuffers(0, 1, &m_vBuffer, &stride, &offset);
	deviceCtxt->IASetIndexBuffer(m_iBuffer, DXGI_FORMAT_R32_UINT,0);

	// Set the render format for the terrain
	deviceCtxt->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

/*
*******************************************************************
* METHOD: Get Indices
*******************************************************************
* Returns the number of indices in the terrain mesh
*/

int ASTerrain::GetNumIndices()
{
	return m_numIndices;
}

/*
*******************************************************************
* METHOD: Get Vertices
*******************************************************************
* Returns the number of vertices in the terrain mesh
*/

int ASTerrain::GetNumVertices()
{
	return m_numVertices;
}

/*
*******************************************************************
* METHOD: Release
*******************************************************************
* Calls private interface disposal methods
*/

void ASTerrain::Release()
{
	ReleaseBuffers();
}

/*
*******************************************************************
* METHOD: Release Buffers
*******************************************************************
* Disposes of the buffers safely
*/

void ASTerrain::ReleaseBuffers()
{
	// Dispose of the index buffer
	if(m_iBuffer)
	{
		m_iBuffer->Release();
		m_iBuffer = 0;
	}
	// Dispose of the vertex buffer
	if(m_vBuffer)
	{
		m_vBuffer->Release();
		m_vBuffer = 0;
	}
}
