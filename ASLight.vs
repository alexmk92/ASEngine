
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

// Describe the X, Y, Z position of the camera in the world this is sent to the PS
// to describe specular lighting
// set 1bye of padding to ensure the strucuture maintains a mult of 16
cbuffer CameraBuffer 
{
	float3 cameraPosition;
	float  padding;
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
	float3 viewDir  : TEXCOORD1;	// used for specular lighting calcs in the pixel shader
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
	float4 worldPos;

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

	// In order to calculate specular lighting, the view dir of the camera need to be
	// calculate, this is done by subtracting the camera position by the world matrix, this
	// vector is normalised and sent to the pixel shader to calculate specular
	worldPos = mul(inputVertex.position, world);

	// Set the cameras view direction and then send it to the Pixel Shader
	outputPixel.viewDir = cameraPosition.xyz - worldPos.xyz;
	outputPixel.viewDir = normalize(outputPixel.viewDir);

	return outputPixel;
}

