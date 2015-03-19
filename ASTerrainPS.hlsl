
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

// Textures for different areas of the heightmap
Texture2D foliage   : register(t0);
Texture2D rockFace  : register(t1);
Texture2D rockFloor : register(t2);

SamplerState sampleType;

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
	float2 texCoord : TEXCOORD0;
	float3 normal   : NORMAL;		// Vertex normal to calculate lighting
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
	float blend;			// How much should the current pixel sample be blended by with its neighbouring samples (0.0 to 1.0f)
	float slopeGradient;	// How steep is the current terrain face

	/*
	* Sample the four textures used for height based texture mapping
	*/

	foliageColor  = foliage.Sample(sampleType, inputPixel.texCoord);
	rockColor     = rockFloor.Sample(sampleType, inputPixel.texCoord);
	rockFaceColor = rockFace.Sample(sampleType, inputPixel.texCoord);

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

	// Invert the light direction for the calculations, and then calculate the intensity of light
	// which is being emitted to the current pixel.  Intensity is the dot product of both the normal vector and 
	// the light direction vector
	color = ambientColor;

	lightDir = -lightDirection;
	lightIntensity = saturate(dot(inputPixel.normal, lightDir));

	if(lightIntensity > 0.0f)
	{
		color += (diffuseColor * lightIntensity);
	}

	// Saturate the color to get the final value
	color = saturate(color);
	color = color * texColor;

	return color;
}

