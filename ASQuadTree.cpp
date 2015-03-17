
/*
******************************************************************
* ASQuadTree.cpp
******************************************************************
* Implement all function prototype in ASQuadTree.h
*/

#include "ASQuadTree.h"

/*
******************************************************************
* Consturctor
******************************************************************
* Set all pointers to objects to null, these will be properly
* initialised later
*/

ASQuadTree::ASQuadTree()
{
	m_vertices   = 0;
	m_parentNode = 0;
}

/*
******************************************************************
* Empty constructor
******************************************************************
*/

ASQuadTree::ASQuadTree(const ASQuadTree& tree)
{}

/*
******************************************************************
* Destructor
******************************************************************
*/

ASQuadTree::~ASQuadTree()
{}

/*
******************************************************************
* METHOD: Init
******************************************************************
* Initialises the quad tree to be drawn in the rendering pipeline
*
* @param ID3D11Device* - The device we are rendering with
* @param ASTerrain* - pointer to the terrain object we are rendering
*
* @return bool - True if the shader initialised successfully, else false
*/

bool ASQuadTree::Init(ID3D11Device* device, ASTerrain* terrain)
{
	// Output parameters for the quad
	float quadWidth;
	float centerX;
	float centerZ;

	int numVertices = terrain->GetNumVertices();
	m_numTriangles  = numVertices / 3;
	m_vertices = new ASVertex[numVertices];
	if(!m_vertices)
		return false;

	// Copy vertex array from the terrain structure into the m_vertice struct
	terrain->GetVerticeArray((void*)m_vertices);

	// Populate the output parameters based on the mesh dimension
	CalculateMeshDimensions(numVertices, centerX, centerZ, quadWidth); 

	// Create the parent node for the tree structure, then build the tree recursively
	// based on the parent node and data passed back from output parameters
	m_parentNode = new ASNode;
	if(!m_parentNode)
		return false;

	CreateTreeNode(m_parentNode, centerX, centerZ, quadWidth, device);

	// Dispose of the vertex list as it has been populated and no longer serves purpose
	if(m_vertices)
	{
		delete [] m_vertices;
		m_vertices = 0;
	}

	return true;
}

/*
******************************************************************
* METHOD: Render
******************************************************************
* Calls render node to recursively build nodes from the tree using
* the frustum which will determine what polys can be seen in the view.
*
* @param ASFrustum* - Pointer to the frustum class we use for rendering
* @param ID3D11DeviceContext* - The rendering device
* @param ASTerrainShader* - The terrain shader to calculate normals and tex coords
*/

void ASQuadTree::Render(ASFrustrum* frustum, ASTerrainShader* shader, ID3D11DeviceContext* deviceCtxt)
{
	m_numPolys = 0;
	// Traverse through the tree, rendering each node
	RenderNode(m_parentNode, frustum, deviceCtxt, shader);
}

/*
******************************************************************
* METHOD: Create Tree Node
******************************************************************
* Creates a new node attached to a parent, this function
* builds the quad tree. This method works by calling the parent
* node and then recursively calling this function for every
* child node of the current node
*
* @param ASNode* - pointer the current node (has up to 4 children with N depth)
* @param float   - nodes x coordinate 
* @param float   - nodes y coordinate
* @param float   - width of the quadtree
* @param ID3D11Device* - pointer to the rendering device
*/

void ASQuadTree::CreateTreeNode(ASNode* node, float posX, float posZ, float width, ID3D11Device* device)
{
	// list of vertices and indices
	ASVertex*      vertices;
	unsigned long* indices;
	// Buffer descriptors
	D3D11_BUFFER_DESC vBufferDesc;
	D3D11_BUFFER_DESC iBufferDesc;
	D3D11_SUBRESOURCE_DATA vData;
	D3D11_SUBRESOURCE_DATA iData;

	// Set the initial structure of the node, set all unkown parameters to null pointers
	node->pos.x = posX;
	node->pos.y = 0.0f; // never used, but safe to populate structure!
	node->pos.z = posZ;
	node->width = width;
	// Set buffers and triangle total to null for now
	node->numTriangles = 0;
	node->vBuffer = 0;
	node->iBuffer = 0;
	// Set all child nodes to null pointers for now
	node->nodes[0] = 0;
	node->nodes[1] = 0; 
	node->nodes[2] = 0;
	node->nodes[3] = 0;

	// Get the number of triangles for this node from the terrain class, this
	// method call queries the ASTerrain object for its total number of triangles and writes the
	// results back to the input parameters specified here.
	int numTriangles = GetTriangleCount(posX, posZ, width);

	// Node is empty, this node in tree is complete and doesn't need to recurse deeper
	if(numTriangles == 0)
		return;
	// Too many triangles in this subset, create 4 sub nodes to hold the data
	if(numTriangles > MAX_POLYS)
	{
		for(int i = 0; i < 4; i++)
		{
			// Calculate child nodes offset
			float offsetX = (((i % 2) < 1)? -1.0f : 1.0f) * (width / 4.0f);
			float offsetZ = (((i % 4) < 2)? -1.0f : 1.0f) * (width / 4.0f);

			// See if the new node has any triangles
			int total = GetTriangleCount((posX + offsetX), (posZ + offsetZ), (width / 2.0f));
			if(total > 0)
			{
				// The new node has triangles, create a child node at the current index and call the
				// CreateTreeNode method to create the new child node
				node->nodes[i] = new ASNode;
				CreateTreeNode(node->nodes[i], (posX + offsetX), (posZ + offsetZ), (width / 2.0f), device);
			}
		}
	}
	// There is no need to create a child node as the number of triangles in this section of the tree are within
	// the threshold, thereofre this node is at the bottom of the tree
	node->numTriangles = numTriangles;
	int numVertices = numTriangles * 3;

	// Create an indice and vertice list to send to the shader
	vertices = new ASVertex[numVertices];
	indices  = new unsigned long[numVertices];

	// Use a tracker variable to populate the indice array, if we used the
	// i iterator, then if a successful triangle wasn't in the view we would have
	// gaps in the indice count (which makes no sense...it would also pull back redundant 
	// data from the global vertice array)
	int currIndex = 0;

	for(int i = 0; i < m_numTriangles; i++)
	{
		// Check if the triangle is in the frustum, if it is, then add it to the vertex array
		bool success = IsTriangleInQuad(i, posX, posZ, width);
		if(success)
		{
			// Get the next three vertices for this triangle from the vertex list, we multiply
			// by three as m_vertices had its index count multiplied by 3 to account
			// for each face
			int vertIndex = i * 3;
			vertices[currIndex].pos      = m_vertices[vertIndex].pos;
			vertices[currIndex].texCoord = m_vertices[vertIndex].texCoord;
			vertices[currIndex].norm     = m_vertices[vertIndex].norm;
			indices[currIndex] = currIndex;
			currIndex++;

			// Get the next vertex for this face
			vertIndex++;
			vertices[currIndex].pos      = m_vertices[vertIndex].pos;
			vertices[currIndex].texCoord = m_vertices[vertIndex].texCoord;
			vertices[currIndex].norm     = m_vertices[vertIndex].norm;
			indices[currIndex] = currIndex;
			currIndex++;

			// Get the last vertex for this face
			vertices[currIndex].pos      = m_vertices[vertIndex].pos;
			vertices[currIndex].texCoord = m_vertices[vertIndex].texCoord;
			vertices[currIndex].norm     = m_vertices[vertIndex].norm;
			indices[currIndex] = currIndex;
			currIndex++;
		}
	}

	/*
	* VERTEX BUFFER DESC
	*/

	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.ByteWidth = sizeof(ASVertex) * numVertices;
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.CPUAccessFlags = 0;
	vBufferDesc.MiscFlags = 0;
	vBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vData.pSysMem = vertices;
	vData.SysMemPitch = 0;
	vData.SysMemSlicePitch = 0;

	/*
	* INDEX BUFFER DESC
	*/

	iBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	iBufferDesc.ByteWidth = sizeof(unsigned long) * numVertices;
	iBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBufferDesc.CPUAccessFlags = 0;
	iBufferDesc.MiscFlags = 0;
	iBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	iData.pSysMem = vertices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	// Create the buffers on the device
	device->CreateBuffer(&iBufferDesc, &iData, &node->iBuffer);
	device->CreateBuffer(&vBufferDesc, &vData, &node->vBuffer);

	// Clean up local resources as we no longer need them
	delete [] vertices;
	delete [] indices;
	vertices = 0;
	indices = 0;
}

/*
******************************************************************
* METHOD: Render Node
******************************************************************
* Draws all visible nodes in the tree to the screen, this will
* be called by ASGraphics.cpp, This method will utilise the frustum
* class to check if the user can view the current quad, the terrain
* shader is also utilised here to render each.  
*
* @param ASNode* - pointer to the current node
* @param ASFrustum* - pointer to the frustum object
* @param ID3D11DeviceContext* - Pointer to the rendering device
* @param ASTerrainShader* - Pointer to the terrain shader
*/

void ASQuadTree::RenderNode(ASNode* node, ASFrustrum* frustum, ID3D11DeviceContext* deviceCtx, ASTerrainShader* shader)
{
	// Check if the node can be viewed in the frustum, the quad that has been generated is a
	// cube which bounds the map, therefore we can use the CheckCube method in the frustum,
	// the Y parameter is not important, so long as we have the X and Y culling values set
	// an unsuccessful callback the parent node nor its children will have it in the view,
	// therefore we break out the function
	bool success = frustum->CheckCube(node->pos.x, 0.0f, node->pos.z, (node->width / 2.0f));
	if(!success)
		return;

	// Check which child node can see whats in the current frustum
	int count = 0;
	for(int i = 0; i < 4; i++)
	{
		if(node->nodes[i] != 0)
		{
			count++;
			RenderNode(node->nodes[i], frustum, deviceCtx, shader);
		}
	}

	// Check if the count is not 0, if it is then we can assume that the parent nodes have
	// no triangles (because the child nodes contain them
	if(count != 0)
		return;

	// The current node has seen the triangles we want to render, set the buffers for
	// rendering in ASGraphics.cpp
	unsigned int stride = sizeof(ASVertex);
	unsigned int offset = 0;

	deviceCtx->IASetVertexBuffers(0, 1, &node->vBuffer, &stride, &offset);
	deviceCtx->IASetIndexBuffer(node->iBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render the polygons for this node in the shader
	int numIndices = node->numTriangles * 3;
	shader->RenderShader(deviceCtx, numIndices);

	// Set the amount of polys that have been rendered for the frame
	m_numPolys += node->numTriangles;
}

/*
******************************************************************
* METHOD: Calculate Mesh Dimensions
******************************************************************
* Determines the quad size of the parent node, this is calculated
* by analysing all vertices in the terrains list of vertices.
* These vertices are used to output the width and center x/z planes
* to the Init function, which will then build the parent node
*
* @param int - The total number of vertices in the mesh (sent from ASTerrain)
* @param float& - The output parameter for centerX plane
* @param float& - The output parameter for centerZ plane
* @param float& - The output parameter for the quad width
*/

void ASQuadTree::CalculateMeshDimensions(int numVertices, float& centerX, float& centerZ, float& quadWidth)
{
	// initialise the output parameters
	centerX = 0.0f;
	centerZ = 0.0f;

	// Get the product of all vertices in the mesh, once the product has been calculated,
	// divide it by the number of vertices to get the center point for the X and Z planes
	for(int i = 0; i < numVertices; i++)
	{
		centerX += m_vertices[i].pos.x;
		centerZ += m_vertices[i].pos.z;
	}

	// Set the output parameters
	centerX = centerX / (float)numVertices;
	centerZ = centerZ / (float)numVertices;	// require the float cast for precision

	// determine the max/min depth and width of the mesh (default values based on first index of array
	float minWidth = fabsf(m_vertices[0].pos.x - centerX);
	float minDepth = fabsf(m_vertices[0].pos.z - centerZ);

	float maxWidth = 0.0f;
	float maxDepth = 0.0f;

	// Loop through all vertices to find the max and min width/depth of the mesh
	for(int i = 0; i < numVertices; i++)
	{
		float w = fabsf(m_vertices[i].pos.x - centerX);
		float d = fabsf(m_vertices[i].pos.z - centerZ);

		// compute the new max/min values for the mesh
		if(w > maxWidth) maxWidth = w;
		if(d > maxDepth) maxDepth = d;
		if(w < minWidth) minWidth = w;
		if(d < minDepth) minDepth = d;
	}

	// Find the maximum of all three components
	float maxX = (float)__max(fabs(minWidth), fabs(maxWidth));
	float maxZ = (float)__max(fabs(minDepth), fabs(maxDepth));

	// Set the max bounds of the quad by multiplying the maximum of the X and Z values by 2
	// this will give the viewing diameter of the quad
	quadWidth = max(maxX, maxZ) * 2.0f;
}

/*
******************************************************************
* METHOD: Get Num Polys
******************************************************************
* Returns the number of polys that have been drawn in the current view
*
* @return int - the total number of drawn polys in frustum
*/

int ASQuadTree::GetPolyCount()
{
	return m_numPolys;
}

/*
******************************************************************
* METHOD: Get Num Triangles
******************************************************************
* Determines which of the triangles from the terrain data are
* within the dimensions specified - this works by calling the
* IsTriangleInQuad method, if a success callback is returned from
* that method, then the triangle is inside the quad view, and will
* therefore be added to the triangle total
* 
* @param float - the x we are viewing from
* @param float - the z we are viewing from 
* @param float - the width of the quad
*
* @return int - the number of triangles in the view
*/

int ASQuadTree::GetTriangleCount(float posX, float posZ, float width)
{
	int count = 0;

	// Check which triangles in the mesh should be contained in this node
	for(int i = 0; i < m_numTriangles; i++)
	{
		bool success = IsTriangleInQuad(i, posX, posZ, width);
		if(success)
			count++;
	}

	return count;
}

/*
******************************************************************
* METHOD: Is Triangle In Quad
******************************************************************
* Determines if the triangle at the given index exists within the
* current node
* 
* @param int   - the index the triangle is located at in the vertice array
* @param float - the x we are viewing from
* @param float - the z we are viewing from 
* @param float - the width of the quad
*
* @return bool - true if the triangle exists in the view, else false
*/

bool ASQuadTree::IsTriangleInQuad(int index, float posX, float posZ, float width)
{
	float radius = width / 2.0f;
	int   vIndex = index * 3;		// index of this triangle corresponding to global indice list

	// get the X and Z coords from this triangle
	float x1 = m_vertices[vIndex].pos.x;
	float x2 = m_vertices[vIndex+1].pos.x;
	float x3 = m_vertices[vIndex+2].pos.x;

	float z1 = m_vertices[vIndex].pos.z;
	float z2 = m_vertices[vIndex+1].pos.z;
	float z3 = m_vertices[vIndex+2].pos.z;

	// Check if the min of the x coords are in this node, if the value is greater
	// then we know it is outside of the node therefore return false, do the same for
	// max values
	float minX = __min(x1, __min(x2, x3));
	if(minX > (posX + radius))
		return false;
	float maxX = __max(x1, __max(x2, x3));
	if(maxX > (posX - radius))
		return false;
	float minZ = __min(z1, __min(z2, z3));
	if(minZ > (posZ + radius))
		return false;
	float maxZ = __max(z1, __max(z2, z3));
	if(maxZ < (posZ - radius))
		return false;

	// The triangle is inside the view! return true
	return true;
}

/*
******************************************************************
* METHOD: Release
******************************************************************
* Calls the private implementation of the ReleaseShader() method
*/

void ASQuadTree::Release() 
{
	// Recursively delete children from the tree
	if(m_parentNode)
	{
		ReleaseNode(m_parentNode);
		delete m_parentNode;
		m_parentNode = 0;
	}
}

void ASQuadTree::ReleaseNode(ASNode* node)
{
	// Recursively dispose of nodes in this tree
	for(int i = 0; i < 4; i++)
	{
		// Release base nodes first
		if(node->nodes[i] != 0)
			ReleaseNode(node->nodes[i]);

		// Dispose of all other resources for this node
		if(node->vBuffer)
		{
			node->vBuffer->Release();
			node->vBuffer = 0;
		}
		// release the index buffer
		if(node->iBuffer)
		{
			node->iBuffer->Release();
			node->iBuffer = 0;
		}
		// Release child nodes once we know there are no more levels of recursoion
		for(int j = 0; j < 4; j++)
		{
			delete node->nodes[i];
			node->nodes[i] = 0;
		}
	}
}