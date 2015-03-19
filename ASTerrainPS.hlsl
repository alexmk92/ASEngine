
/*
******************************************************************
* ASTerrain.ps
******************************************************************
* Pixel shader to map each pixel to the polygons that are to
* be rendered to the screen.  
*
* The pixel shader recieves its input from the output of the
* vertex shader
******************************************************************
*/

// Depth texture, used for close high pixel quality mapping
Texture2D depthTex  : register(t0);

// Textures for different areas of the heightmap
Texture2D foliage   : register(t1);
Texture2D rockFace  : register(t2);
Texture2D rockFloor : register(t3);

SamplerState ASSampler;

// This buffer described lighting on a pixel 
// @member float3 - The direction the light is hitting the pixel from
// @member float4 - The difuse lighting color 
// @member float4 - The intensity of ambient color in the scene (global illumination)
// @member float  - Padding
//
// @note : memory allocated in 4byte chunks, failure to do so will effect the mapping
cbuffer LightBuffer 
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDirection;
	float  padding;
};

// TYPE DEFS
struct ASPixel
{
	float4 position : SV_POSITION;	// The position of the pixel rel to the world
	float4 texCoord : TEXCOORD0;	// Increased buffer for ZW coordinates (used for depthPos mapping)
	float3 normal   : NORMAL;		// Vertex normal to calculate lighting
	float4 color    : COLOR;		// Color map for the texture
	float4 depthPos : TEXCOORD1;	// Holds the sampled color coordinate for the depth pixel
};

/*
******************************************************************
* PIXEL SHADER
******************************************************************
* @return float4 - The final color of the pixel at this location
*/

float4 TerrainPixelShader(ASPixel inputPixel) : SV_TARGET
{
	// local vars
	float4 texColor;		// color of the texture, sampled from the given texture at the current pixel
	float4 rockColor;		// Color of stone
	float4 rockFaceColor;   // Color of the stone face (side of mountains)
	float4 foliageColor;    // Color of grass and other foliage
	float4 color;			// the diffuse color
	float3 lightDir;		// the direction the light is hitting the surface
	float  lightIntensity;	// the brightness of the light 0.0f to 1.0f
	float  blend;			// How much should the current pixel sample be blended by with its neighbouring samples (0.0 to 1.0f)
	float  slopeGradient;	// How steep is the current terrain face
	float  depth;			// The current depth of the viewers position
	float4 depthColor;		// The color of the depth texture (sampled from depth texture coord)

	/*
	* Sample the four textures used for height based texture mapping
	*/

	foliageColor  = foliage.Sample(ASSampler,   inputPixel.texCoord);
	rockColor     = rockFloor.Sample(ASSampler, inputPixel.texCoord);
	rockFaceColor = rockFace.Sample(ASSampler,  inputPixel.texCoord);

	// Calculate the height of the slope at this point, this will determine which texture is to be 
	// mapped to the certain piece of terrain
	slopeGradient = 1.0f - inputPixel.normal.y;

	/*
	* Determine which texture pixel should be mapped to the current surface on the terrain,
	* for each case, interpolate a new color for the texture based on the base neighbouring 
	* texture type, for example, foliage neighbour the rock face, therefore blend foliage with rock face
	*/

	// Grass
	if(slopeGradient < 0.175)
	{
		blend = slopeGradient / 0.175f;
		texColor = lerp(foliageColor, rockFaceColor, blend);
	} 
	// Rock face
	if(slopeGradient < 0.65 && slopeGradient >= 0.175)
	{
		blend = (slopeGradient - 0.175f) * (1.0f / (0.65f - 0.175f));
		texColor = lerp(rockFaceColor, rockColor, blend);
	}
	// Rocks
	if(slopeGradient >= 0.65)
	{
		texColor = rockColor;
	} 

	/*
	*  Invert the light direction for the calculations, and then calculate the intensity of light
	*  which is being emitted to the current pixel.  Intensity is the dot product of both the normal vector and 
	*  the light direction vector
	*/

	color = ambientColor;

	lightDir = -lightDirection;

	// Set the color based on the intensity of the light
	lightIntensity = saturate(dot(inputPixel.normal, lightDir));
	if(lightIntensity > 0.0f)
		color += (diffuseColor * lightIntensity);

	/*
	* Calculate the value of depth stencil, and then determine whether to map the detail texture to the
	* current pixel or not
	*/

	depth = inputPixel.depthPos.z / inputPixel.depthPos.w;
	if(depth < 10.f)
	{
		// Sample the extra two bytes of coord information from the buffer (made this 4Bytes instead of 2Byes
		// in ASTextureShader.cpp)
		depthColor = depthTex.Sample(ASSampler, inputPixel.texCoord.zw);
		texColor = (texColor * depthColor * lightIntensity) * 1.5f;
	}

	/*
	*  Produce the output color
	*/ 

	color = saturate(color);
	color = color * texColor;
	color = saturate(color * inputPixel.color * 2.0f);

	return color;
}

