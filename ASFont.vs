
/*
******************************************************************
* ASLight.vs
******************************************************************
* Vertex shader to map each tex coord to the object
*
* The pixel shader recieves its input from the output of the
* vertex shader
******************************************************************
*/

cbuffer ContstantBuffer
{
	matrix world;
	matrix view;
	matrix projection;
};

// TYPE DEFS 
struct VertexInputType
{
	float4 position : POSITION;
	float2 texCoord : TEXCOORD0;
	float3 normal   : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD0;
};

/*
******************************************************************
* VERTEX SHADER
******************************************************************
* @parapm VertexInputType - The input pixel to have its light calculated
* @return PixelInputType - The output pixel to be sent to shader
*/

PixelInputType FontVertexShader(VertexInputType inputVertex) 
{
	PixelInputType outputPixel;
	float4 worldPos;

	// Modify the vertex
	inputVertex.position.w = 1.0f;

	// Calculate the position of the vertex
	outputPixel.position = mul(inputVertex.position, world);
	outputPixel.position = mul(outputPixel.position, view);
	outputPixel.position = mul(outputPixel.position, projection);

	// Store texture coords
	outputPixel.texCoord = inputVertex.texCoord;

	return outputPixel;
}

