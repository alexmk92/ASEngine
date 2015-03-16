
/*
******************************************************************
* ASTerrain.h
*******************************************************************
* Builds the world map, this accounts for using heightmaps to 
* extrude the surface and produce a contoured world
*******************************************************************
*/

#ifndef _ASTERRAIN_H_
#define _ASTERRAIN_H_

/*
*******************************************************************
* Includes:
*******************************************************************
*/

#include <stdio.h>
#include <d3d11.h>
#include <d3dx10math.h>

/*
*******************************************************************
* Class declaration
*******************************************************************
*/

class ASTerrain
{
private:
	// Vertex structure to describe the terrain
	struct ASVertex
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normal;
	};
	// Struct to hold information on the heightmap
	struct ASHeightMap
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normals;
	};
	// Vector to calculate lighting between angles
	struct ASLightVector
	{
		D3DXVECTOR3 vn;
	};
public:
	// Constructors and Destructors
	ASTerrain();
	ASTerrain(const ASTerrain&);
	~ASTerrain();

	// Public methods
	bool Init(ID3D11Device*, char*);
	void Release();
	void Render(ID3D11DeviceContext*);

	int GetNumIndices();	// returns number of indices in the frustum
	int GetNumVertices();

private:
	// Private methods
	bool InitBuffers(ID3D11Device*);
	void RenderBuffers(ID3D11DeviceContext*);
	void ReleaseBuffers();

	// Height map handling code
	bool LoadHeightMap(char*);
	void NormaliseHeightMap();
	bool CalculateMapNormals();
	void ReleaseHeightMap();

	// Private member variables
	int m_width;
	int m_height;
	int m_numVertices;
	int m_numIndices;

	ASHeightMap*  m_heightMap;
	ID3D11Buffer* m_vBuffer;	// Vertices for the terrain mesh
	ID3D11Buffer* m_iBuffer;	// Indices for the terrain mesh
};

#endif