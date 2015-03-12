
/*
******************************************************************
* ASColor.vs
******************************************************************
* Vertex shader for any colors handled by the application
******************************************************************
*/

// Each Matrix will be updated in the buffer on each frame
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
}

/*
********************************************************************
* TYPEDEFS
********************************************************************
* Any custom typedefs for the Shader are defined here 
* VertexInputType : Defines a Vertex Shader
* PixelInputType  : Defines a Pixel Shader
********************************************************************
*/

struct VertexInputType
{
	float4 position: POSITION;
	float4 color   : COLOR; 
};

struct PixelInputType
{
	float4 position: SV_POSITION;
	float4 color   : COLOR;
}

/*
********************************************************************
* VERTEX SHADER
********************************************************************
* Called by every vertex in the vertex buffer.  This shader takes
* the position of the input vertex and multiplies it by the world, view
* and projection matrices to get the position relative to the world.
*
* @param VertexInputType - Input vertex to be described relative to the world
*
* @return PixelInputType - Output vertex to be passed to the pixel shader
********************************************************************
*/

PixelInputType ColorVertexShader(VertexInputType inputVert)
{
	PixelInputType outputVert;

	// Change position of vertex
	inputVert.position.w = 1.0f;

	// Calculate the position of the vertex to be placed in the world
	outputVert = mul(inputVert.position, worldMatrix);
	outputVert = mul(inputVert.position, viewMatrix);
	outputVert = mul(inputVert.position, projectionMatrix);

	// Store the input color for the shader
	outputVert.color = inputVert.color;

	return outputVert;
}