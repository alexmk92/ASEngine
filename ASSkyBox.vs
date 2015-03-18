
/*
******************************************************************
* ASSkyBox.vs
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
struct Vertex
{
	float4 position : POSITION;
};

struct Pixel
{
	float4 position : SV_POSITION;
	float4 domePos  : TEXCOORD0;
};

/*
******************************************************************
* VERTEX SHADER
******************************************************************
* @parapm VertexInputType - The input pixel to have its light calculated
* @return PixelInputType - The output pixel to be sent to shader
*/

Pixel ASSkyVertexShader(Vertex inputVertex) 
{
	Pixel outputPixel;

	// Modify the vertex
	inputVertex.position.w = 1.0f;

	// Calculate the position of the vertex
	outputPixel.position = mul(inputVertex.position, world);
	outputPixel.position = mul(outputPixel.position, view);
	outputPixel.position = mul(outputPixel.position, projection);

	// Send the input position to the pixel shader
	outputPixel.domePos = inputVertex.position;

	return outputPixel;
}

