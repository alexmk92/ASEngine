
/*
******************************************************************
* ASSkyBox.h
******************************************************************
* Builds a sphere with a mapped texture to act as the sky for
* the scene
******************************************************************
*/

#ifndef _ASSKYBOX_H_
#define _ASSKYBOX_H_

/*
******************************************************************
* Includes
******************************************************************
*/

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>

// Use the std namespace to make our life easier calling funcs from the namespace
using namespace std;

/*
******************************************************************
* Class declaration
******************************************************************
*/

class ASSkyBox
{
private:
	// Struct mapped to the struct found in ASLight.vs
	struct ASVertex
	{
		D3DXVECTOR3 pos;
	};
	// Struct to hold data on the model, this is then sent to the rendering pipeline
	// to draw the model to the scene
	struct ASMesh
	{
		// Vertex positions for each vertice
		D3DXVECTOR3 pos;
		// Texture coordinates for each vertice
		D3DXVECTOR2 tex;
		// Normals for each vertice, used to be passed to PS for lighting
		D3DXVECTOR3 norm;
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
	ASSkyBox();
	ASSkyBox(const ASSkyBox&);
	~ASSkyBox();

	// Methods to manage the models buffers
	bool Init(ID3D11Device*);
	void Render(ID3D11DeviceContext*);
	void Release();

	int GetNumIndices();
	D3DXVECTOR4 GetColorA();
	D3DXVECTOR4 GetColorB();

private:
	// Private methods
	bool InitBuffers(ID3D11Device*);
	bool LoadSkyBox(char*);
	bool LoadModel(char*);

	// Private member variables
	ID3D11Buffer* m_vBuffer;
	ID3D11Buffer* m_iBuffer;
	int			  m_numVertices;
	int			  m_numIndices;
	ASMesh*		  m_mesh;
	D3DXVECTOR4   m_skyColorA;
	D3DXVECTOR4	  m_skyColorB;

};

#endif