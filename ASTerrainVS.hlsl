
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
struct ASVertex
{
	float4 position : POSITION;
	float4 texCoord : TEXCOORD0;
	float3 normal   : NORMAL;
	float4 color    : COLOR;
};

struct ASPixel
{
	float4 position : SV_POSITION;
	float4 texCoord : TEXCOORD0;
	float3 normal   : NORMAL;
	float4 color    : COLOR;
	float4 depthPos : TEXCOORD1;    // Checks the current depth from camera to determine if a depth 
									// detail texture should be mapped to the pixel
};

/*
******************************************************************
* VERTEX SHADER
******************************************************************
* @parapm VertexInputType - The input pixel to have its light calculated
* @return PixelInputType - The output pixel to be sent to shader
*/

ASPixel TerrainVertexShader(ASVertex inputVertex) 
{
	ASPixel outputPixel;

	// Modify the vertex so we can do 4x4 matrix mult
	inputVertex.position.w = 1.0f;

	// Calculate the position of the vertex
	outputPixel.position = mul(inputVertex.position, world);
	outputPixel.position = mul(outputPixel.position, view);
	outputPixel.position = mul(outputPixel.position, projection);

	// Set the texture coordinates
	outputPixel.texCoord = inputVertex.texCoord;

	// Calculate the VN of the vertex against the world matrix (to get global lighting)
	outputPixel.normal = mul(inputVertex.normal, (float3x3)world);

	// Normalise the vector before returning it 
	outputPixel.normal = normalize(outputPixel.normal);

	// Set the pixels color map to the input vertex pixel (sent from ASTerrain.cpp)
	outputPixel.color    = inputVertex.color;

	// Set the output pixels depth to the current position 
	outputPixel.depthPos = outputPixel.position;

	return outputPixel;
}

