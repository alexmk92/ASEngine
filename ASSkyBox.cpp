
/*
******************************************************************
* ASSkyBox.cpp
*******************************************************************
* Implement all methods prototyped in ASSkyBox.h
*******************************************************************
*/

#include "ASSkyBox.h"

/*
*******************************************************************
* Constructor
*******************************************************************
* Set all member values for the player object so we know our 
* player (hero) has some sort of value!
*/

ASSkyBox::ASSkyBox()
{
	m_vBuffer = 0;
	m_iBuffer = 0;
	m_mesh    = 0;
}

/*
*******************************************************************
* Empty consturctor
*******************************************************************
*/

ASSkyBox::ASSkyBox(const ASSkyBox& box)
{}

/*
*******************************************************************
* Destructor
*******************************************************************
*/

ASSkyBox::~ASSkyBox()
{}

/*
*******************************************************************
* METHOD: Init
*******************************************************************
* Initialise the skybox object by loading the model and then 
* passing the vertex and index data to the buffers ready for rendering
*/

bool ASSkyBox::Init(ID3D11Device* device)
{
	// Attempt to load the model for the skybox
	bool success = LoadSkyBox("./models/skybox.txt");
	if(!success)
		return false;

	// Initialise the index and vertex buffers
	success = InitBuffers(device);
	if(!success)
		return false;

	// Initialise the colors for the sky (map loaded in is greatscale, this will apply
	// two highlight colours to give the sky depth)
	m_skyColorA = D3DXVECTOR4(0.23f, 0.25f, 0.75f, 1.0f);
	m_skyColorB = D3DXVECTOR4(0.53f, 0.52f, 0.62f, 1.0f);

	return true;
}

/*
*******************************************************************
* METHOD: Init Buffers
*******************************************************************
* Loads the model into the vertex and index buffers
*/

bool ASSkyBox::InitBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vBufferDesc;
	D3D11_BUFFER_DESC iBufferDesc;
	D3D11_SUBRESOURCE_DATA iData;
	D3D11_SUBRESOURCE_DATA vData;
	HRESULT hr;

	// Initialise temp vertice and indice arrays to be passed to the buffer
	unsigned long* indices = new unsigned long[m_numIndices];
	ASVertex*      vertices = new ASVertex[m_numVertices];
	if(!vertices || !indices)
		return false;

	// Load the vertex array with vertex data
	for(int i = 0; i < m_numVertices; i++)
	{
		vertices[i].pos = D3DXVECTOR3(m_mesh[i].pos.x, m_mesh[i].pos.y, m_mesh[i].pos.z);
		indices[i] = i;
	}

	/*
	* Describe the vertex buffer
	*/ 

	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth = sizeof(ASVertex) * m_numVertices;
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags = 0;
	vBufferDesc.MiscFlags = 0;
	vBufferDesc.StructureByteStride = 0;

	vData.pSysMem = vertices;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&vBufferDesc, &vData, &m_vBuffer);
	if(FAILED(hr))
		return false;

	/*
	* Describe the index buffer
	*/

	iBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	iBufferDesc.ByteWidth = sizeof(unsigned long) * m_numIndices;
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.CPUAccessFlags = 0;
	iBufferDesc.MiscFlags = 0;
	iBufferDesc.StructureByteStride = 0;

	iData.pSysMem = indices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	hr = device->CreateBuffer(&iBufferDesc, &iData, &m_iBuffer);
	if(FAILED(hr))
		return false;

	// Release the temporary resources as the global buffers now hold the data they need
	delete [] vertices;
	delete [] indices;
	vertices = 0;
	indices = 0;

	return true;
}

/*
*******************************************************************
* METHOD: Render
*******************************************************************
* Renders the buffers, putting the skybox geometry into the graphics
* pipeline, redy for rendering
*/

void ASSkyBox::Render(ID3D11DeviceContext* deviceCtx)
{
	// Describe how the buffers should be layed out in memory
	unsigned int stride = sizeof(ASVertex);
	unsigned int offset = 0;

	// Set the buffers on the device
	deviceCtx->IASetVertexBuffers(0, 1, &m_vBuffer, &stride, &offset);
	deviceCtx->IASetIndexBuffer(m_iBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

/*
*******************************************************************
* METHOD: Get Num Indices
*******************************************************************
* Returns number of indices in mesh
*/

int ASSkyBox::GetNumIndices()
{
	return m_numIndices;
}

/*
*******************************************************************
* METHOD: Get Color A
*******************************************************************
* Gets the first set color of the sky (base color)
*/

D3DXVECTOR4 ASSkyBox::GetColorA()
{
	return m_skyColorA;
}

/*
*******************************************************************
* METHOD: Get Color B
*******************************************************************
* Gets the second set color of the sky (highlight color)
*/

D3DXVECTOR4 ASSkyBox::GetColorB()
{
	return m_skyColorB;
}

/*
*******************************************************************
* METHOD: Load Sky Box
*******************************************************************
* Loads the skybox from the .obj file provided
*
* @param char* - pointer to the file we wish to open
* @return bool - True if loaded, else false
*/

bool ASSkyBox::LoadSkyBox(char* filename)
{
	ifstream fin;
	char input;
	int i;


	// Open the model file.
	fin.open(filename);
	
	// If it could not open the file then exit.
	if(fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> m_numVertices;

	// Set the number of indices to be the same as the vertex count.
	m_numIndices = m_numVertices;

	// Create the model using the vertex count that was read in.
	m_mesh = new ASMesh[m_numVertices];
	if(!m_mesh)
		return false;

	// Read up to the beginning of the data.
	fin.get(input);
	while(input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for(i=0; i<m_numVertices; i++)
	{
		fin >> m_mesh[i].pos.x >> m_mesh[i].pos.y >> m_mesh[i].pos.z;
		fin >> m_mesh[i].tex.x >> m_mesh[i].tex.y;
		fin >> m_mesh[i].norm.x >> m_mesh[i].norm.y >> m_mesh[i].norm.z;
	}

	// Close the model file.
	fin.close();

	return true;
}

/*
*******************************************************************
* METHOD: Release
*******************************************************************
* Release all items held by the class and set their pointers to 
* null
*/

void ASSkyBox::Release()
{
	// Dispose of index buffer
	if(m_iBuffer)
	{
		m_iBuffer->Release();
		m_iBuffer = 0;
	}
	// Dispose of vertex buffer
	if(m_vBuffer)
	{
		m_vBuffer->Release();
		m_vBuffer = 0;
	}
	// Dispose of the mesh
	if(m_mesh)
	{
		delete [] m_mesh;
		m_mesh = 0;
	}
}

