
/*
******************************************************************
* ASQuadTree.h
*******************************************************************
* Builds a Quad Tree that improves performance (by culling polygons
* that aren't within the current viewing quad), as well as allowing
* the player to move over the terrain (achieved by determining what
* node the player is currently standing on)
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
	// Requires an ASVertex struct to pass vertex information to shader
	struct ASVertex 
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 texCoord;
		D3DXVECTOR3 norm;
	};
	// Struct to define each node in the quad tree, aside from the obvious
	// members, each ASNode has 4 ASNode children (the quad inside that view)
	struct ASNode
	{
		D3DXVECTOR3 pos;
		float width;
		int numTriangles;
		ID3D11Buffer* vBuffer;
		ID3D11Buffer* iBuffer;
		ASNode* nodes[4];   
	};
public:
	// Constructors / Destructor prototype
	ASQuadTree();
	ASQuadTree(const ASQuadTree&);
	~ASQuadTree();

	// Public methods
	bool Init(ID3D11Device*, ASTerrain*);
	void Render(ASFrustrum*, ASTerrainShader*, ID3D11DeviceContext*);
	int  GetPolyCount();

	void Release();

private:
	// Private methods
	void CalculateMeshDimensions(int, float&, float&, float&);
	void CreateTreeNode(ASNode*, float, float, float, ID3D11Device*);
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

const int MAX_POLYS = 15000;	// Determines the number of polys that can be rendered at one time

#endif