
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
