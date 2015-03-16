
/*
******************************************************************
* ASTerrain.vs
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
	float3 normal   : NORMAL;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
	float3 normal   : NORMAL;
};

/*
******************************************************************
* VERTEX SHADER
******************************************************************
* @parapm VertexInputType - The input pixel to have its light calculated
* @return PixelInputType - The output pixel to be sent to shader
*/

PixelInputType TerrainVertexShader(VertexInputType inputVertex) 
{
	PixelInputType outputPixel;
	float4 worldPos;

	// Modify the vertex
	inputVertex.position.w = 1.0f;

	// Calculate the position of the vertex
	outputPixel.position = mul(inputVertex.position, world);
	outputPixel.position = mul(outputPixel.position, view);
	outputPixel.position = mul(outputPixel.position, projection);

	// Calculate the VN of the vertex against the world matrix (to get global lighting)
	outputPixel.normal = mul(inputVertex.normal, (float3x3)world);

	// Normalise the vector before returning it 
	outputPixel.normal = normalize(outputPixel.normal);

	return outputPixel;
}

