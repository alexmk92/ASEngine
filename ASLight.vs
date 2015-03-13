
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
	float3 normal   : NORMAL;
};

/*
******************************************************************
* VERTEX SHADER
******************************************************************
* @parapm VertexInputType - The input pixel to have its light calculated
* @return PixelInputType - The output pixel to be sent to shader
*/

PixelInputType LightVertexShader(VertexInputType inputVertex) 
{
	PixelInputType outputPixel;

	// Modify the vertex
	inputVertex.position.w = 1.0f;

	// Calculate the position of the vertex
	outputPixel.position = mul(inputVertex.position, world);
	outputPixel.position = mul(outputPixel.position, view);
	outputPixel.position = mul(outputPixel.position, projection);

	// Store thje texture coords in the shader
	outputPixel.texCoord = inputVertex.texCoord;

	// Calculate the VN of the vertex against the world matrix (to get global lighting)
	outputPixel.normal = mul(inputVertex.normal, (float3x3)world);

	// Normalise the vector before returning it 
	outputPixel.normal = normalize(outputPixel.normal);

	return outputPixel;
}

