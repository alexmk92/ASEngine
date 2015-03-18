
/*
******************************************************************
* ASCamera.h
******************************************************************
* Tells the GPU the view from which we will draw our scene from.
* The camera keeps track of what is in its view, and shall also
* allow players to navigate the world 
*/

#ifndef _ASCAMERA_H_
#define _ASCAMERA_H_

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

class ASCamera
{
public:
	// Constructors and Destructors
	ASCamera();
	ASCamera(const ASCamera&);
	~ASCamera();

	// Public methods
	void SetRotation(float, float, float);
	void SetPosition(float, float, float);

	// Getters to retrieve the rotation and position Matrices
	D3DXVECTOR3 GetRotation();	
	D3DXVECTOR3 GetPosition();

	// Render the view
	void RenderCameraView();

	// Write the View matrix to a given D3DXMATRIX object
	void GetViewMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);

private:
	// X, Y, Z Positions of the projection matrix
	D3DXVECTOR3 m_pos;
	// X, Y, Z Positions of the rotation matrix
	D3DXVECTOR3 m_rot;
	//  Matrix to describe what we are looking at (the eye of the camera)
	D3DXMATRIX m_cameraView;
};

#endif