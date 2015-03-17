
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

	// Calculate the number of faces in the mesh and initialise the vertex array which will
	// hold all data on this class
	m_numTriangles  = numVertices / 3;
	m_vertices = new ASVertex[numVertices];
	if(!m_vertices)
		return false;

	// Copy vertex array from the terrain structure into the m_vertice struct
	terrain->GetVerticeArray((void*)m_vertices);

	// Populate the output parameters based on the mesh dimension
	GetMeshDimensions(numVertices, centerX, centerZ, quadWidth); 

	// Create the parent node for the tree structure, then build the tree recursively
	// based on the parent node and data passed back from output parameters
	m_parentNode = new ASNode;
	if(!m_parentNode)
		return false;

	// Recursive function to build the tree with vert data
	AppendNode(m_parentNode, centerX, centerZ, quadWidth, device);

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

void ASQuadTree::AppendNode(ASNode* node, float posX, float posZ, float width, ID3D11Device* device)
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
	node->posX = posX;
	node->posZ = posZ;
	node->width = width;
	// Set buffers and triangle total to null for now
	node->numTriangles = 0;
	node->vertices = 0;
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
	if(numTriangles > MAX_TRIANGLES)
	{
		for(int i = 0; i < NODE_CHILDREN; i++)
		{
			// Calculate child nodes offset by multiplying either -1.0 of 1.0 by the radius
			// of the quad
			float offsetX = (((i % 2) < 1)? -1.0f : 1.0f) * (width / 4.0f);
			float offsetZ = (((i % 4) < 2)? -1.0f : 1.0f) * (width / 4.0f);

			// See if the new node has any triangles within the current diameter of the quad
			int total = GetTriangleCount((posX + offsetX), (posZ + offsetZ), (width / 2.0f));
			if(total > 0)
			{
				// The new node has triangles, create a child node at the current index and call the
				// CreateTreeNode method to create the new child node
				node->nodes[i] = new ASNode;
				AppendNode(node->nodes[i], (posX + offsetX), (posZ + offsetZ), (width / 2.0f), device);
			}
		}
		// Computed this node, restart loop
		return;
	}
	// There is no need to create a child node as the number of triangles in this section of the tree are within
	// the threshold, thereofre this node is at the bottom of the tree
	node->numTriangles = numTriangles;
	int numVertices    = numTriangles * 3;

	// Create an indice and vertice list to send to the shader, the node vertice list
	// keeps track of all vertices inside the current node, for quick line intersection
	// processing
	vertices       = new ASVertex[numVertices];
	indices        = new unsigned long[numVertices];
	node->vertices = new ASVector[numVertices];

	// Use a tracker variable to populate the indice array, if we used the
	// i iterator, then if a successful triangle wasn't in the view we would have
	// gaps in the indice count (which makes no sense...it would also pull back redundant 
	// data from the global vertice array)
	int currIndex = 0;

	for(int i = 0; i < m_numTriangles; i++)
	{
		// Check if the triangle is in the frustum, if it is, then add it to the vertex array
		if(IsTriangleInQuad(i, posX, posZ, width))
		{
			// Get the next three vertices for this triangle from the vertex list, we multiply
			// by three as each iteration of the loop will extract three vertices for each face
			// (extract corresponding 3 vertices from global array dependent on
			// the current position of this array (tracked by external currIndex var, instead of i)
			// * Also, the nodes vertice list is populated as well as the global vertice list, this
			// is so each node can keep track of its own vertices for line intersection checking later
			int vertIndex = i * 3;
			vertices[currIndex].pos      = m_vertices[vertIndex].pos;
			vertices[currIndex].texCoord = m_vertices[vertIndex].texCoord;
			vertices[currIndex].norm     = m_vertices[vertIndex].norm;
			// Set nodes vertice buffer
			node->vertices[currIndex].x  = m_vertices[vertIndex].pos.x;
			node->vertices[currIndex].y  = m_vertices[vertIndex].pos.y;
			node->vertices[currIndex].z  = m_vertices[vertIndex].pos.z;
			// Set indice buffer
			indices[currIndex] = currIndex;
			currIndex++;

			// Get the next vertex for this face
			vertIndex++;
			vertices[currIndex].pos      = m_vertices[vertIndex].pos;
			vertices[currIndex].texCoord = m_vertices[vertIndex].texCoord;
			vertices[currIndex].norm     = m_vertices[vertIndex].norm;
			// Set nodes vertice buffer
			node->vertices[currIndex].x  = m_vertices[vertIndex].pos.x;
			node->vertices[currIndex].y  = m_vertices[vertIndex].pos.y;
			node->vertices[currIndex].z  = m_vertices[vertIndex].pos.z;
			// Set indice buffer
			indices[currIndex] = currIndex;
			currIndex++;

			// Get the last vertex for this face
			vertIndex++;
			vertices[currIndex].pos      = m_vertices[vertIndex].pos;
			vertices[currIndex].texCoord = m_vertices[vertIndex].texCoord;
			vertices[currIndex].norm     = m_vertices[vertIndex].norm;
			// Set nodes vertice buffer
			node->vertices[currIndex].x  = m_vertices[vertIndex].pos.x;
			node->vertices[currIndex].y  = m_vertices[vertIndex].pos.y;
			node->vertices[currIndex].z  = m_vertices[vertIndex].pos.z;
			// Set indice buffer
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

	device->CreateBuffer(&vBufferDesc, &vData, &node->vBuffer);

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
	iData.pSysMem = indices;
	iData.SysMemPitch = 0;
	iData.SysMemSlicePitch = 0;

	// Create the buffers on the nodes buffer to be rendered
	device->CreateBuffer(&iBufferDesc, &iData, &node->iBuffer);

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
	bool success = frustum->CheckCube(node->posX, 0.0f, node->posZ, (node->width / 2.0f));
	if(!success)
		return;

	// Check which child node can see whats in the current frustum
	int count = 0;
	for(int i = 0; i < NODE_CHILDREN; i++)
	{
		// Only populate a child node if it exists in the tree, 0 indicates a null pointer
		// to the node at the current index
		if(node->nodes[i] != 0)
		{
			count++;
			RenderNode(node->nodes[i], frustum, deviceCtx, shader);
		}
	}

	// Check if the count is not 0, if it is then we can assume that the parent nodes have
	// no triangles (because the child nodes contain them, therefore we need to traverse no further)
	if(count != 0)
		return;

	// The current node has seen the triangles we want to render, set the buffers for
	// rendering in ASGraphics.cpp
	unsigned int stride = sizeof(ASVertex);
	unsigned int offset = 0;

	deviceCtx->IASetVertexBuffers(0, 1, &node->vBuffer, &stride, &offset);
	deviceCtx->IASetIndexBuffer(node->iBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceCtx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Render the polygons for this node in the shader with the amount of indices 
	// calculated from total polygons * 3 (for each point in triangle)
	shader->RenderShader(deviceCtx, (node->numTriangles * 3));

	// Set the amount of polys that have been rendered for the frame
	m_numPolys += node->numTriangles;
}

/*
******************************************************************
* METHOD: Get Mesh Dimensions
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

void ASQuadTree::GetMeshDimensions(int numVertices, float& centerX, float& centerZ, float& quadWidth)
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
		if(w > maxWidth) 
			maxWidth = w;
		if(d > maxDepth) 
			maxDepth = d;
		if(w < minWidth) 
			minWidth = w;
		if(d < minDepth) 
			minDepth = d;
	}

	// Find the maximum of all three components
	float maxX = (float)__max(fabs(minWidth), fabs(maxWidth));
	float maxZ = (float)__max(fabs(minDepth), fabs(maxDepth));

	// Set the max bounds of the quad by multiplying the maximum of the X and Z values by 2
	// this will give the viewing diameter of the quad
	quadWidth = __max(maxX, maxZ) * 2.0f;
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
		if(IsTriangleInQuad(i, posX, posZ, width))
			count++;
	}

	return count;
}

/*
******************************************************************
* METHOD: Get Terrain Height at Position
******************************************************************
* Returns the height of the terrain at a given X,Z position.
* 
* @param float  - the x position to locate
* @param float  - the z position to locate
* @param float& - output height value based on x,z coord
*/

bool ASQuadTree::GetTerrainHeightAtPosition(float posX, float posZ, float& height)
{
	// Const used to determine viewing radius of the quad
	float N_RADIUS = m_parentNode->width / 2.0f;

	// Determine the bounds of the quad view
	float minX = m_parentNode->posX - N_RADIUS;
	float maxX = m_parentNode->posX + N_RADIUS;
	float minZ = m_parentNode->posZ - N_RADIUS;
	float maxZ = m_parentNode->posZ + N_RADIUS;

	// Check that the points are within bounds of the min max clips
	if((posX < minX) || (posX > maxX) || (posZ < minZ) || (posZ > maxZ))
		return false;

	// Find the node in the terrain which coincides with the poly at this location
	GetNodeAtPosition(m_parentNode, posX, posZ, height);
	return true;
}

/*
******************************************************************
* METHOD: Get Node at Position
******************************************************************
* Invoked by GetHeightAtPosition, this method will output the 
* height of the node at the given X, Z position.   This is 
* calculated by searching its vertice list to locate which triangle
* matches the X,Z coordinates
*
* It should also be noted that this methods first call always starts
* at the parent node, and traverses through the tree until a match
* has been found.
* 
* @param ASNode* - pointer to the node tree to traverse
* @param float   - the x position to locate
* @param float   - the z position to locate
* @param float&  - output height value based on x,z coord
*/

void ASQuadTree::GetNodeAtPosition(ASNode* node, float x, float z, float& height)
{
	// Get the dimensions of the node by finding its bounds based on the 
	// x,z relative to the radius of the quad width
	float N_RADIUS = node->width / 2.0f;

	float minX = node->posX - N_RADIUS;
	float maxX = node->posX + N_RADIUS;
	float minZ = node->posZ - N_RADIUS;
	float maxZ = node->posZ + N_RADIUS;

	// Check if the current node contains the triangles to be analysed
	if((x < minX) || (x > maxX) || (z < minZ) || (z > maxZ))
		return;

	// Check whether this node has any children which need to be traversed
	int count = 0;
	for(int i = 0; i < NODE_CHILDREN; i++)
	{
		if(node->nodes[i] != 0)
		{
			count++;
			GetNodeAtPosition(node->nodes[i], x, z, height);
		}
	}
	// If child nodes were found, break out of traverse as the node will be in 
	// one of those child nodes
	if(count > 0)
		return;

	// No children were found, the polygon we're looking for is in this node
	for(int i = 0; i < node->numTriangles; i++)
	{
		int vertIndex = i * 3;	// Get the next three vertices on each loop (because we read 3 each time per face)

		D3DXVECTOR3 vecA = D3DXVECTOR3(node->vertices[vertIndex].x, node->vertices[vertIndex].y, node->vertices[vertIndex].z);
		vertIndex++;
		D3DXVECTOR3 vecB = D3DXVECTOR3(node->vertices[vertIndex].x, node->vertices[vertIndex].y, node->vertices[vertIndex].z);
		vertIndex++;
		D3DXVECTOR3 vecC = D3DXVECTOR3(node->vertices[vertIndex].x, node->vertices[vertIndex].y, node->vertices[vertIndex].z);

		// Check if the current polygon corresponds to the triangle we want to find.
		if(GetTriangleHeightAtPosition(x, z, height, vecA, vecB, vecC) == true)
 			return;
	}

	return;
}

/*
******************************************************************
* METHOD: Get Triangle Height at Position
******************************************************************
* Checks whether a given line intersects an input triangle, this
* is calculated by testing if the position (X,Z) falls inside the
* bounds of the given face.  If this is true, the height at the
* given line is calculated and returned
* 
* @param float  - the x position we are testing at
* @param float  - the y position we are testing at
* @param float& - the output height for a valid interset
* @param D3DXVECTOR3 - The first vertex
* @param D3DXVECTOR3 - The second vertex
* @param D3DXVECTOR3 - The third vertex
*
* @return bool - true if successful, else false
*/

bool ASQuadTree::GetTriangleHeightAtPosition(float x, float z, float& heightOut, D3DXVECTOR3 vecA, D3DXVECTOR3 vecB, D3DXVECTOR3 vecC)
{
	// Starting position of the ray that is being cast.
	D3DXVECTOR3 start = D3DXVECTOR3(x, 0.0f, z);
	D3DXVECTOR3 dir   = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

	// Calculate the two edges from the three points given.
	D3DXVECTOR3 edgeA = D3DXVECTOR3(vecB.x - vecA.x, vecB.y - vecA.y, vecB.z - vecA.z);
	D3DXVECTOR3 edgeB = D3DXVECTOR3(vecC.x - vecA.x, vecC.y - vecA.y, vecC.z - vecA.z);

	// Calculate the normal of the triangle from the two edges.
	D3DXVECTOR3 norm = D3DXVECTOR3( (edgeA.y * edgeB.z) - (edgeA.z * edgeB.y),
									(edgeA.z * edgeB.x) - (edgeA.x * edgeB.z),
									(edgeA.x * edgeB.y) - (edgeA.y * edgeB.x));

	// Calculate the magintude of the vector, then from that normalis the vector and
	// then calculate the distance from origin to plane
	float magnitude = (float)sqrt((norm.x * norm.x) + (norm.y * norm.y) + (norm.z * norm.z));
	norm.x = norm.x / magnitude;
	norm.y = norm.y / magnitude;
	norm.z = norm.z / magnitude;

	float distance = ((-norm.x * vecA.x) + (-norm.y * vecA.y) + (-norm.z * vecA.z));

	// Get the denominator, ensuring its value is not < 0 (shouldn't happen)
	float denominator = ((norm.x * dir.x) + (norm.y * dir.y) + (norm.z * dir.z));
	if(fabs(denominator) < 0.0001f)
		return false;

	// Get the numerator, then calculate the point of intersection
	float numerator = -1.0f * (((norm.x * start.x) + (norm.y * start.y) + (norm.z * start.z)) + distance);
	float intersect = numerator / denominator;

	// Get the vector at the intersection point
	D3DXVECTOR3 vecIntersect = D3DXVECTOR3( start.x + (dir.x * intersect),
										    start.y + (dir.y * intersect),
										    start.z + (dir.z * intersect));

	// Calculate the edges of the intersect vector
	D3DXVECTOR3 intersectEdgeA = D3DXVECTOR3( vecB.x - vecA.x, vecB.y - vecA.y, vecB.z - vecA.z );
	D3DXVECTOR3 intersectEdgeB = D3DXVECTOR3( vecC.x - vecB.x, vecC.y - vecB.y, vecC.z - vecB.z );
	D3DXVECTOR3 intersectEdgeC = D3DXVECTOR3( vecA.x - vecC.x, vecA.y - vecC.y, vecA.z - vecC.z );

	// Calculate the normal for the first edge.
	D3DXVECTOR3 edgeNormal = D3DXVECTOR3((intersectEdgeA.y * norm.z) - (intersectEdgeA.z * norm.y),
										 (intersectEdgeA.z * norm.x) - (intersectEdgeA.x * norm.z),
										 (intersectEdgeA.x * norm.y) - (intersectEdgeA.y * norm.x));

	// Calculate determinant to check whether the point lies within the bounds of the
	// vec (inside, outside or edge of given point), the determinent tells us the bounds of the point
	// relative to the input vector - this process is repeated for each of the edges, from that
	// a height is for the intersecting vector
	D3DXVECTOR3 vecTmp = D3DXVECTOR3(vecIntersect.x - vecA.x,
									 vecIntersect.y - vecA.y,
									 vecIntersect.z - vecA.z);

	float determinant = ((edgeNormal.x * vecTmp.x) + (edgeNormal.y * vecTmp.y) + (edgeNormal.z * vecTmp.z));

	// Check if it is outside.
	if(determinant > 0.001f)
		return false;

	// Calculate the normal for the second edge.
	edgeNormal.x = (intersectEdgeB.y * norm.z) - (intersectEdgeB.z * norm.y);
	edgeNormal.y = (intersectEdgeB.z * norm.x) - (intersectEdgeB.x * norm.z);
	edgeNormal.z = (intersectEdgeB.x * norm.y) - (intersectEdgeB.y * norm.x);

	// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
	vecTmp.x = vecIntersect.x - vecB.x;
	vecTmp.y = vecIntersect.y - vecB.y;
	vecTmp.z = vecIntersect.z - vecB.z;

	determinant = ((edgeNormal.x * vecTmp.x) + (edgeNormal.y * vecTmp.y) + (edgeNormal.z * vecTmp.z));

	// Check if it is outside.
	if(determinant > 0.001f)
	{
		return false;
	}

	// Calculate the normal for the third edge.
	edgeNormal.x = (intersectEdgeC.y * norm.z) - (intersectEdgeC.z * norm.y);
	edgeNormal.y = (intersectEdgeC.z * norm.x) - (intersectEdgeC.x * norm.z);
	edgeNormal.z = (intersectEdgeC.x * norm.y) - (intersectEdgeC.y * norm.x);

	// Calculate the determinant to see if it is on the inside, outside, or directly on the edge.
	vecTmp.x = vecIntersect.x - vecC.x;
	vecTmp.y = vecIntersect.y - vecC.y;
	vecTmp.z = vecIntersect.z - vecC.z;

	determinant = ((edgeNormal.x * vecTmp.x) + (edgeNormal.y * vecTmp.y) + (edgeNormal.z * vecTmp.z));

	// Check if it is outside.
	if(determinant > 0.001f)
	{
		return false;
	}

	// Now we have our height.
	heightOut = vecIntersect.y;

	return true;
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
	if(maxX < (posX - radius))
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

/*
******************************************************************
* METHOD: Release Node
******************************************************************
* To release a node, all of its children must be disposed of, therefore
* this method recursively deletes all child nodes before deleting itself
*/


void ASQuadTree::ReleaseNode(ASNode* node)
{
	// Recursively dispose of nodes in this tree
	for(int i = 0; i < NODE_CHILDREN; i++)
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
		// release the nodes vertice array
		if(node->vertices)
		{
			delete [] node->vertices;
			node->vertices = 0;
		}
		// Release child nodes once we know there are no more levels of recursoion
		for(int j = 0; j < NODE_CHILDREN; j++)
		{
			delete node->nodes[i];
			node->nodes[i] = 0;
		}
	}
}