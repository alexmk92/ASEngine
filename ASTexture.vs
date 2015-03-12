
/*
******************************************************************
* ASTexture.vs
******************************************************************
* Vertex shader to map each tex coord to the object
*
* The pixel shader recieves its input from the output of the
* vertex shader
******************************************************************
*/

cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;
};

// TYPE DEFS
struct VertexInputType
{
	float4 position : POSITON;
	floa2  texCoord : TEXCOORD0;
}

struct PixelInputType
{
	float4 position : POSITON;
	floa2  texCoord : TEXCOORD0;
}

/*
******************************************************************
* VERTEX SHADER
******************************************************************
* @return PixelInputType - The output pixel
*/

PixelInputType TextureVertexShader(VertexInputType inputVertex) 
{
	PixelInputType outputPixel;

	// Modify the vertex
	inputVertex.position.w = 1.0f;

	// Calculate the position of the vertex
	outputPixel.position = mul(inputVertex.poisition, world);
	outputPixel.position = mul(outputPixel.position, view);
	outputPixel.position = mul(outputPixel.position, projection);

	// Store thje texture coords in the shader
	outputPixel.texCoord = inputVertex.texCoord;

	return outputPixel;
}

