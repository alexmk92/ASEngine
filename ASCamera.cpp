
/*
******************************************************************
* ASCamera.cpp
******************************************************************
* Implements all methods prototyped in ASCamera.h
******************************************************************
*/

#include "ASCamera.h"

/*
******************************************************************
* Constructor
******************************************************************
* Set all member values to 0.0f so we know our matrix has some
* default values
******************************************************************
*/

ASCamera::ASCamera() 
{
	m_posX = 0.0f;
	m_posY = 0.0f;
	m_posZ = 0.0f;
	
	m_rotX = 0.0f;
	m_rotY = 0.0f;
	m_rotZ = 0.0f;
}

/*
******************************************************************
* Empty Constructor
******************************************************************
*/

ASCamera::ASCamera(const ASCamera& camera)
{}

/*
******************************************************************
* Destructor
******************************************************************
* 
******************************************************************
*/

ASCamera::~ASCamera()
{}

/*
******************************************************************
* METHOD: Set Position
******************************************************************
* Sets the position of the camera in our 3D world (this essentially
* provides the X, Y, Z coordinates that the camera will be located
* in)
******************************************************************
*/

void ASCamera::SetPosition(float x, float y, float z)
{
	m_posX = x;
	m_posY = y;
	m_posZ = z;
}

/*
******************************************************************
* METHOD: Set Rotation
******************************************************************
* Sets the angle of which the camera is to be located at, this
* will define where the camera is looking, from this we can derive
* a view matrix to output what we are viewing to the screen
******************************************************************
*/

void ASCamera::SetRotation(float x, float y, float z)
{
	m_rotX = x;
	m_rotY = y;
	m_rotZ = z;
}

/*
******************************************************************
* METHOD: Get Position
******************************************************************
* Returns the Camera Position matrix
*
* @return D3DXVECTOR3 - the position matrix
******************************************************************
*/

D3DXVECTOR3 ASCamera::GetPosition()
{
	return D3DXVECTOR3(m_posX, m_posY, m_posZ);
}

/*
******************************************************************
* METHOD: Get Rotation
******************************************************************
* Returns the Cameras Rotation matrix
*
* @return D3DXVECTOR3 - The rotation matrix
******************************************************************
*/

D3DXVECTOR3 ASCamera::GetRotation()
{
	return D3DXVECTOR3(m_rotX, m_rotY, m_rotZ);
}

/*
******************************************************************
* METHOD: Render Camera View
******************************************************************
* Draws the world relative to the camera, allowing us to see 
* what the camera can see.  This is called the View matrix.
******************************************************************
*/

void ASCamera::RenderCameraView()
{
	D3DXMATRIX cameraRot;

	// Set the "up" vector to point upward
	D3DXVECTOR3 up = D3DXVECTOR3(0.0f, 
								 1.0f, 
								 0.0f);

	// Set the cameras position relative to the world
	D3DXVECTOR3 pos = D3DXVECTOR3(m_posX, 
								  m_posY, 
								  m_posZ);

	// Set the direction the camera is looking at
	D3DXVECTOR3 eye = D3DXVECTOR3(0.0f,
								  0.0f, 
								  1.0f);

	// Set rotation of the camera on its axis
	float pitch = m_rotX * 0.0174532925f; // X Axis Rotation
	float yaw   = m_rotY * 0.0174532925f; // Y Axis Rotation
	float roll  = m_rotZ * 0.0174532925f; // Z Axis Rotation

	// Set the yaw pitch and roll rotations for the camera, and write it to the output
	// matrix (cameraRot)
	D3DXMatrixRotationYawPitchRoll(&cameraRot, yaw, pitch, roll); 

	// Transform eye by the rotation matrix to get the correct view
	D3DXVec3TransformCoord(&eye, &eye, &cameraRot);
	D3DXVec3TransformCoord(&up, &up, &cameraRot);

	// Translate the rotated camera to the viewer location
	eye = pos + eye;

	// Create the view matrix by multiplying the cameras eye, pos and up matrices together (this
	// gives the cameras position relative to the world)
	D3DXMatrixLookAtLH(&m_cameraView, &pos, &eye, &up);
}

/*
******************************************************************
* METHOD: Get View Matrix
******************************************************************
* Return a copy of the current view matrix to an output var
*
* @param D3DXMATRIX& - The output variable to write the current view matrix
******************************************************************
*/

void ASCamera::GetViewMatrix(D3DXMATRIX& view)
{
	view = m_cameraView;
	return;
}

