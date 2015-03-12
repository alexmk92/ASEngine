
/*
******************************************************************
* ASModel.cpp
******************************************************************
* Implementation of all methods defined in ASModel.h
******************************************************************
*/

#include "ASModel.h"

/*
******************************************************************
* Constructor
******************************************************************
* Set members pointers to null pointers before we init them
*/

ASModel::ASModel()
{
	m_vertexBuffer = 0;
	m_indexBuffer  = 0;
}

/*
******************************************************************
* Empty Constructor
******************************************************************
*/

ASModel::ASModel(const ASModel& model)
{}

/*
******************************************************************
* Destructor
******************************************************************
*/

ASModel::~ASModel() 
{}

/*
******************************************************************
* METHOD: Init()
******************************************************************
* Creates a new instance of ASModel, initialises the vertex and
* index buffers and then captures the result.
*
* @param ID3D11Device* - The rendering device D3D is using
*
* @return bool - True if initialisation succeeded, else false
******************************************************************
*/

bool ASModel::Init(ID3D11Device* device)
{
	bool success = InitBuffers(device);
	if(!success)
		return false;
	else
		return true;
}

/*
******************************************************************
* METHOD: Release
******************************************************************
* Dispose of any resources held by the buffers - calls the private
* ReleaseBuffers() method as we may need to release the class from
* outside the initial context
*/

void ASModel::Release()
{
	ReleaseBuffers();
	return;
}

/*
******************************************************************
* Method: Render
******************************************************************
* Calls the RenderBuffers() method, we call it through this public
* interface as we may need to call the method outside the context
* of this class
*
* @param ID3D11DeviceContext* - the device we are rendering with
*/

void ASModel::Render(ID3D11DeviceContext* device)
{
	RenderBuffers(device);
	return;
}

/*
******************************************************************
* METHOD: Get Index Count
******************************************************************
* @return int - the number of index in this model
******************************************************************
*/

int ASModel::GetIndexCount()
{
	return m_numIndices;
}

/*
******************************************************************
* METHOD: Init Buffers
******************************************************************
* Handles the creation of both the Vertex and Index buffers for
* the model
*
* @param ID3D11Device* - The device we are rendering with
* 
* @return bool - True if the initialisation succeeded, else false
*/

bool ASModel::InitBuffers(ID3D11Device* device)
{
	HRESULT hr;

	VertexType*    vertices;
	unsigned long* indices;

	D3D11_BUFFER_DESC vBufferDesc;
	D3D11_BUFFER_DESC iBufferDesc;

	D3D11_SUBRESOURCE_DATA vData;
	D3D11_SUBRESOURCE_DATA iData;

	// 
	m_numVertices = 3;
	m_numIndices  = 3;

	// Create an array to hold the vertices
	vertices = new VertexType[m_numVertices];
	if(!vertices)
		return false;

	// Create an array to hold the indices
	indices = new unsigned long[m_numIndices];
	if(!indices)
		return false;

	// Populate the vertex and index arrays with data to be passed to the shaders,
	// always draw clock-wise, failure to do so will result in triangle not being drawn
	// due to BACK_FACE_CULL flag set in the rasteriser
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);  // Bottom left.
	vertices[0].color	 = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f);  // Top middle.
	vertices[1].color	 = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  // Bottom right.
	vertices[2].color	 = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);

	indices[0] = 0;  // Bottom left.
	indices[1] = 1;  // Top middle.
	indices[2] = 2;  // Bottom right.

	// Describe the vertex buffer, then call CreateBuffer on the device, this
	// will return a pointer to the buffer
	vBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth			= sizeof(VertexType)* m_numVertices;
	vBufferDesc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags		= 0;
	vBufferDesc.MiscFlags			= 0;
	vBufferDesc.StructureByteStride = 0;
	
	// Give subresource a pointer to vertex data
	vData.pSysMem					= vertices;
	vData.SysMemPitch				= 0;
	vData.SysMemSlicePitch			= 0;

	hr = device->CreateBuffer(&vBufferDesc, &vData, &m_vertexBuffer);
	if(FAILED(hr))
		return false;

	// Describe the index buffer, then call CreateBuffer on the device, this
	// will return a pointer to the buffer
	iBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	iBufferDesc.ByteWidth			= sizeof(unsigned long)* m_numIndices;
	iBufferDesc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
	iBufferDesc.CPUAccessFlags		= 0;
	iBufferDesc.MiscFlags			= 0;
	iBufferDesc.StructureByteStride = 0;

	// Give subsresource a pointer to the index data
	iData.pSysMem					= indices;
	iData.SysMemPitch				= 0;
	iData.SysMemSlicePitch			= 0;

	hr = device->CreateBuffer(&iBufferDesc, &iData, &m_indexBuffer);
	if(FAILED(hr))
		return false;

	// Delete index and vertex arrays as the data has been sent to the buffers
	delete [] vertices;
	delete [] indices;

	vertices = 0;
	indices  = 0;

	return true;
}

/*
******************************************************************
* METHOD: Release Buffers
******************************************************************
* Disposes of any resources used by the index and vertex buffers
******************************************************************
*/

void ASModel::ReleaseBuffers()
{
	// Dispose of Index buffer
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
	// Dispose of Vertex buffer
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

/*
******************************************************************
* METHOD: Render Buffers
******************************************************************
* Set the vertex and index buffer as active on the GPU's input
* assembler.  The GPU the uses the shader to render the buffer 
* that has been passed to it.   
*
* @param ID3D11DeviceContext* - The device we are using to render with
******************************************************************
*/

void ASModel::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	// Set the stride and offset in memory of the vertex buffer
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// Flag the vertex and index buffers as active in the assembler, allowing rendering to commence
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered for the Vertex buffer
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}