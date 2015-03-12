
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
*/

#include <d3d11.h>
#include <d3dx10math.h>

/*
******************************************************************
* Class declaration
******************************************************************
*/

class ASModel
{
private:
	// Struct mapped to the struct found in ASColor.vs
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};

public:
	// Constructors and Destructors
	ASModel();
	ASModel(const ASModel&);
	~ASModel();

	// Methods to manage the models buffers
	bool Init(ID3D11Device*);
	void Release();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

private:
	// Private methods
	bool InitBuffers(ID3D11Device*);
	void ReleaseBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	// Private member variables
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;
	
	int m_numVertices;
	int m_numIndices;

};

#endif