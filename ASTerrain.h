
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
#include "ASTexture.h"
#include <vector>

using namespace std;

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
		D3DXVECTOR2 texCoord;
		D3DXVECTOR3 normal;
		D3DXVECTOR4 color;
	};
	// Struct to hold information on the heightmap
	struct ASHeightMap
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR2 texCoord;
		D3DXVECTOR3 normals;
		D3DXVECTOR3 color;
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
	bool Init(ID3D11Device*, char*, char*, vector<WCHAR*>);
	void Release();

	void GetVerticeArray(void*);	
	int GetNumVertices();

	ID3D11ShaderResourceView*   GetTextureAtIndex(int);
	void GetTextures(vector<ID3D11ShaderResourceView*>&);

private:
	// Private methods
	bool InitBuffers(ID3D11Device*);

	// Texturre handlign methods
	void CalculateTextureCoords();
	bool LoadTexture(ID3D11Device*, vector<WCHAR*>);

	// Height map handling code
	bool LoadMap(char*, bool);
	void NormaliseHeightMap();
	bool CalculateMapNormals();

	// Private member variables
	int m_width;
	int m_height;
	int m_numVertices;

	ASVertex*           m_vertices;
	vector<ASTexture>*  m_textures;
	ASHeightMap*        m_heightMap;
};

// controls the size of the sample for the texture
const int TEXTURE_TILE_SIZE = 16;

#endif