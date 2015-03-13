
/*
******************************************************************
* ASLight.h
******************************************************************
* Track the direction and color of lights acting on the scene
******************************************************************
*/

#ifndef _ASLIGHT_H
#define _ASLIGHT_H_

/*
******************************************************************
* Includes
******************************************************************
*/

#include <d3dx10math.h>

/*
******************************************************************
* Class declaration
******************************************************************
*/

class ASLight
{
public:
	// Constructors and Destructors
	ASLight();
	ASLight(const ASLight&);
	~ASLight();

	// Methods to manage the models buffers
	void SetDiffuse(float, float, float, float);
	void SetDirection(float, float, float);

	// Getters to retrieve the current diffuse color of light and its direction
	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetLightDirection();

private:
	// Private member variables
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR3 m_lightDir;

};

#endif