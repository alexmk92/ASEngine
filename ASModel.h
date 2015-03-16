
/*
******************************************************************
* ASModel.h
******************************************************************
* Encapsulates the Geometry for any given 3D model
******************************************************************
*/

#ifndef _ASMODEL_H_
#define _ASMODEL_H_

/*
******************************************************************
* Includes
******************************************************************
* + ASTextureShader.h - Supports texturing of models
* + fstream - allows us to read files (for OBJ model loading)
*/

#include <d3d11.h>
#include <d3dx10math.h>
#include "ASTexture.h"
#include <fstream>

// Use the std namespace to make our life easier calling funcs from the namespace
using namespace std;

/*
******************************************************************
* Class declaration
******************************************************************
*/

class ASModel
{
private:
	// Struct mapped to the struct found in ASLight.vs
	struct ASVertex
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};
	// Struct to hold data on the model, this is then sent to the rendering pipeline
	// to draw the model to the scene
	struct ASMesh
	{
		// Vertex positions for each vertice
		float posX;
		float posY;
		float posZ;
		// Texture coordinates for each vertice
		float texU;
		float texV;
		// Normals for each vertice, used to be passed to PS for lighting
		float normX;
		float normY;
		float normZ;
	};
	// Struct to hold data on the material associated with the model
	struct ASMaterial
	{
		WCHAR	matName[MAX_PATH]; 
		WCHAR   texName[MAX_PATH];
		D3DXVECTOR3 ambient;
		D3DXVECTOR3 diffuse;
		D3DXVECTOR3 specular;
		int specularIntensity;
		float alpha;
		bool hasSpecular;
		ID3D11ShaderResourceView* texture;
		ID3D10EffectTechnique* technique;
	};

public:
	// Constructors and Destructors
	ASModel();
	ASModel(const ASModel&);
	~ASModel();

	// Methods to manage the models buffers
	bool Init(ID3D11Device*, WCHAR*, char*);
	void Release();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	// Used to pass the models texture resource to the shader
	ID3D11ShaderResourceView* GetTexture();

private:
	// Private methods
	bool InitBuffers(ID3D11Device*);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	// Methods to handle the loading and disposing of texture resource on a model
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	// Functions to handle the loading and disposal of a model
	bool LoadModel(char*);
	void ReleaseModel();

	// Private member variables
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	ASMaterial*   m_materials;
	
	int m_numVertices;
	int m_numIndices;

	ASTexture* m_texture;
	ASMesh*    m_mesh;

};

#endif