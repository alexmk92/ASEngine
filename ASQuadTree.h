
/*
******************************************************************
* ASQuadTree.h
*******************************************************************
* Builds a Quad Tree that improves performance (by culling polygons
* that aren't within the current viewing quad), as well as allowing
* the player to move over the terrain (achieved by determining what
* node the player is currently standing on, by computing if there
* are any line intersections between the object and the finite
* amount of triangles in the current quad)
*******************************************************************
*/

#ifndef _ASQUADTREE_H_
#define _ASQUADTREE_H_

/*
******************************************************************
* Includes
******************************************************************
*/

#include "ASTerrain.h"
#include "ASFrustrum.h"
#include "ASTerrainShader.h"

/*
******************************************************************
* Class declaration
******************************************************************
*/

class ASQuadTree 
{
private:

	// Configuration constants
	static const int NODE_CHILDREN = 4;    // how many children does each node have
	static const int MAX_TRIANGLES = 10000;	// Determines the number of triangles that can be rendered at one time

	// Requires an ASVertex struct to pass vertex information to shader
	struct ASVertex 
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 texCoord;
		D3DXVECTOR3 norm;
	};
	// Holds x,y,z coordinates
	struct ASVector
	{
		float x;
		float y;
		float z;
	};
	// Struct to define each node in the quad tree, aside from the obvious
	// members, each ASNode has 4 ASNode children (the quad inside that view)
	struct ASNode
	{
		float         posX, posZ;
		float         width;
		int           numTriangles;
		ID3D11Buffer* vBuffer;
		ID3D11Buffer* iBuffer;
		ASVector*     vertices;
		ASNode*       nodes[NODE_CHILDREN];   
	};
public:
	// Constructors / Destructor prototype
	ASQuadTree();
	ASQuadTree(const ASQuadTree&);
	~ASQuadTree();

	// Public methods
	bool Init(ID3D11Device*, ASTerrain*);
	void Render(ASFrustrum*, ASTerrainShader*, ID3D11DeviceContext*);
	bool GetTerrainHeightAtPosition(float, float, float&);
	int  GetPolyCount();

	void Release();

private:
	// Private methods
	void GetMeshDimensions(int, float&, float&, float&);
	void AppendNode(ASNode*, float, float, float, ID3D11Device*);
	void GetNodeAtPosition(ASNode*, float, float, float&);
	bool GetTriangleHeightAtPosition(float, float, float&, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR3);
	int  GetTriangleCount(float, float, float);
	bool IsTriangleInQuad(int, float, float, float);
	void RenderNode(ASNode*, ASFrustrum*, ID3D11DeviceContext*, ASTerrainShader*);
	void ReleaseNode(ASNode*);

	// Private member variables
	ASVertex* m_vertices;		// List of vertices of mesh
	ASNode*   m_parentNode;		// Children nodes can be chained from this parent
	int		  m_numTriangles;
	int		  m_numPolys;


};

#endif