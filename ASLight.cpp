
/*
******************************************************************
* ASLight.cpp
******************************************************************
* Implementation of all methods defined in ASLight.h
******************************************************************
*/

#include "ASLight.h"

/*
******************************************************************
* Constructor
******************************************************************
*/

ASLight::ASLight()
{}

/*
******************************************************************
* Empty Constructor
******************************************************************
*/

ASLight::ASLight(const ASLight& light)
{}

/*
******************************************************************
* Destructor
******************************************************************
*/

ASLight::~ASLight() 
{}

/*
******************************************************************
* METHOD: Set Diffuse
******************************************************************
* Set the diffuse color of the light, this will be the "warmth" of
* the light, for example an orange tinted light will provide a 
* warm orange diffuse on the illuminated surface
*
* @param float - Red diffuse color
* @param float - Green diffuse color
* @param float - Blue diffuse color
* @param float - Alpha diffuse color 
******************************************************************
*/

void ASLight::SetDiffuse(float r, float g, float b, float a)
{
	m_diffuseColor = D3DXVECTOR4(r,g,b,a);
	return;
}

/*
******************************************************************
* METHOD: Set Ambient
******************************************************************
* Set the abmient color of the light, this will be the "warmth" of
* the light, for example an orange tinted light will provide a 
* warm orange diffuse on the illuminated surface
*
* @param float - Red abmient color
* @param float - Green abmient color
* @param float - Blue abmient color
* @param float - Alpha abmient color 
******************************************************************
*/

void ASLight::SetAmbient(float r, float g, float b, float a)
{
	m_ambientColor = D3DXVECTOR4(r,g,b,a);
	return;
}

/*
******************************************************************
* METHOD: Set Specular
******************************************************************
* Set the specular color of the light, this will be the "warmth" of
* the light.
*
* @param float - Red specular color
* @param float - Green specular color
* @param float - Blue specular color
* @param float - Alpha specular color 
******************************************************************
*/

void ASLight::SetSpecular(float r, float g, float b, float a)
{
	m_specularColor = D3DXVECTOR4(r,g,b,a);
	return;
}

/*
******************************************************************
* METHOD: Set Specular Intensity
******************************************************************
* Sets the intensity of the specular, large num results in larger
* reflection area on surface
*
* @param float - describes the intensity of the specular 0.0f to 1.0f
******************************************************************
*/

void ASLight::SetSpecularIntensity(float intensity)
{
	m_specularIntensity = intensity;
	return;
}

/*
******************************************************************
* METHOD: Set Direction
******************************************************************
* Sets the direction that the light is pointing relative to the
* world. This will set a vector pointing in dir (x,y,z) 
*
* @param float - The x direction
* @param float - The y direction
* @param float - The z direction
******************************************************************
*/

void ASLight::SetDirection(float x, float y, float z)
{
	m_lightDir = D3DXVECTOR3(x,y,z);
	return;
}

/*
******************************************************************
* METHOD: Get Diffuse Color
******************************************************************
* Returns the current set diffuse color of the light
*
* @return D3DXVECTOR4 - Vector holding the current set color 
******************************************************************
*/

D3DXVECTOR4 ASLight::GetDiffuseColor()
{
	return m_diffuseColor;
}

/*
******************************************************************
* METHOD: Get Ambient Color
******************************************************************
* Returns the current set ambient color of the light
*
* @return D3DXVECTOR4 - Vector holding the current set color 
******************************************************************
*/

D3DXVECTOR4 ASLight::GetAmbientColor()
{
	return m_ambientColor;
}

/*
******************************************************************
* METHOD: Get Specular Color
******************************************************************
* Returns the current set specular color of the light
*
* @return D3DXVECTOR4 - Vector holding the current set color 
******************************************************************
*/

D3DXVECTOR4 ASLight::GetSpecularColor()
{
	return m_specularColor;
}

/*
******************************************************************
* METHOD: Get Specular Intensity
******************************************************************
* Returns the current intensity of the specular
*
* @return float - the intnesity of the light (between 0.0f and 1.0f)
******************************************************************
*/

float ASLight::GetSpecularIntensity()
{
	return m_specularIntensity;
}

/*
******************************************************************
* METHOD: Get Light Direction
******************************************************************
* Returns the current direction the light is pointing
*
* @return D3DXVECTOR3 - Vector holding the direction of the light
******************************************************************
*/

D3DXVECTOR3 ASLight::GetLightDirection()
{
	return m_lightDir;
}
