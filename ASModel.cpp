
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
	m_texture      = 0;
	m_mesh         = 0;
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
* @param WCHAR*        - Pointer to the texture file the model uses
* @param char*         - Pointer to the model file the model is loaded from
*
* @return bool - True if initialisation succeeded, else false
******************************************************************
*/

bool ASModel::Init(ID3D11Device* device, WCHAR* textureFile, char* modelFile)
{
	// Load model data into constant buffer
	bool success = LoadModel(modelFile);
	if(!success)
		return false;

	// Initialise the buffers to begin rendering
	success = InitBuffers(device);
	if(!success)
		return false;

	// Load the texture for the model
	success = LoadTexture(device, textureFile);
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
	ReleaseTexture();
	ReleaseBuffers();
	ReleaseModel();
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

void ASModel::Render(ID3D11DeviceContext* deviceCtx)
{
	RenderBuffers(deviceCtx);
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
* METHOD: Get Texture
******************************************************************
* @return ID3D11ShaderResourceView* - the texture we loaded
******************************************************************
*/

ID3D11ShaderResourceView* ASModel::GetTexture()
{
	return m_texture->GetTexture();
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

	ASVertex*      vertices;
	unsigned long* indices;

	D3D11_BUFFER_DESC vBufferDesc;
	D3D11_BUFFER_DESC iBufferDesc;

	D3D11_SUBRESOURCE_DATA vData;
	D3D11_SUBRESOURCE_DATA iData;

	// Create an array to hold the vertices
	vertices = new ASVertex[m_numVertices];
	if(!vertices)
		return false;

	// Create an array to hold the indices
	indices = new unsigned long[m_numIndices];
	if(!indices)
		return false;

	// Populate the indice and vertice arrays with data from the model
	for(unsigned int i = 0; i < m_numVertices; i++)
	{
		// Set all information for this vertice, reading data from the mesh
		vertices[i].position = D3DXVECTOR3(m_mesh[i].posX, m_mesh[i].posY, m_mesh[i].posZ);
		vertices[i].texture  = D3DXVECTOR2(m_mesh[i].texU, m_mesh[i].texV);
		vertices[i].normal   = D3DXVECTOR3(m_mesh[i].normX, m_mesh[i].normY, m_mesh[i].normZ);

		// Set the index for this vertice in the index buffer - this is used for fast computation
		// when querying for vertices later in the rendering pipeline
		indices[i] = i;
	}

	// Describe the vertex buffer, then call CreateBuffer on the device, this
	// will return a pointer to the buffer
	vBufferDesc.Usage				= D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth			= sizeof(ASVertex) * m_numVertices;
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
	iBufferDesc.ByteWidth			= sizeof(unsigned long) * m_numIndices;
	iBufferDesc.BindFlags			= D3D11_BIND_INDEX_BUFFER;
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
* METHOD: Load Texture
******************************************************************
* Creates a new texture object based upon the Texture file that
* hasd been given - this is called from Init()
******************************************************************
*/

bool ASModel::LoadTexture(ID3D11Device* device, WCHAR* file)
{
	m_texture = new ASTexture;
	if(!m_texture)
		return false;

	// Initialise the texture object
	bool success = m_texture->Init(device, file);
	if(!success)
		return false;
	else
		return true;
}

/*
******************************************************************
* METHOD: Release Texture
******************************************************************
* Disposes of any any resources utilised by the texture
******************************************************************
*/

void ASModel::ReleaseTexture()
{
	if(m_texture)
	{
		m_texture->Release();
		delete m_texture;
		m_texture = 0;
	}

	return;
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
	unsigned int stride = sizeof(ASVertex);
	unsigned int offset = 0;

	// Flag the vertex and index buffers as active in the assembler, allowing rendering to commence
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered for the Vertex buffer
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

/*
******************************************************************
* METHOD: Load Model
******************************************************************
* Reads all vertice information from a model file (.obj), this 
* operation gets the total number of vertices and indices in the file and 
* then populates the Mesh struct with vertex information, this information
* is then used in the InitBuffers method
*
* @param char* - a pointer to the object file to read from
******************************************************************
*/

bool ASModel::LoadModel(char* objectFile)
{
	ifstream fileIn;
	char readChar;
	
	// Open the file and check if we opened it without error, else kill program
	fileIn.open(objectFile);
	if(fileIn.fail())
		return false;

	// Keep reading from the file until we encounter ":" (eof), this will tell the number of vertices
	fileIn.get(readChar);
	while(readChar != ':')
		fileIn.get(readChar);

	// Read the total number of vertices into global vertex count - total indices will be the same
	// as the number of vertices, so set that here too
	fileIn >> m_numVertices;
	m_numIndices = m_numVertices;

	// Create the mesh
	m_mesh = new ASMesh[m_numVertices];
	if(!m_mesh)
		return false;

	// 
	fileIn.get(readChar);
	while(readChar != ':')
		fileIn.get(readChar);
	fileIn.get(readChar);
	fileIn.get(readChar);
	
	// Read in vertex data from model - each cycle will build one triangle, a triangle is composed
	// of three sets of coordinates (pos, tex, norm) - these are passed to shaders for loading mesh
	for(unsigned int i = 0; i < m_numVertices; i++)
	{
		fileIn >> m_mesh[i].posX >> m_mesh[i].posY >> m_mesh[i].posZ;			// read next three floats into x,y,z
		fileIn >> m_mesh[i].texU >> m_mesh[i].texV;								// read next two floats into u,v
		fileIn >> m_mesh[i].normX >> m_mesh[i].normY >> m_mesh[i].normZ;		// read next three float into x,y,z
	}

	// Close the file
	fileIn.close();

	return true;
}

/*
******************************************************************
* METHOD: Release Model
******************************************************************
* Dispose of any resources held by the mesh to safely dealloc it
******************************************************************
*/

void ASModel::ReleaseModel()
{
	if(m_mesh)
	{
		delete [] m_mesh;
		m_mesh = 0;
	}

	return;
}