
/*
******************************************************************
* ASPlayer.cpp
*******************************************************************
* Implement all methods prototyped in ASPlayer.h
*******************************************************************
*/

#include "ASPlayer.h"

/*
*******************************************************************
* Constructor
*******************************************************************
* Set all member values for the player object so we know our 
* player (hero) has some sort of value!
*/

ASPlayer::ASPlayer()
{
	m_weight    = 180.0f;
	m_frameTime = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;

	m_jumpHeight = 0;
	m_runSpeed   = 0;
	m_turnSpeed  = 0;
}

/*
*******************************************************************
* Empty consturctor
*******************************************************************
*/

ASPlayer::ASPlayer(const ASPlayer&)
{}

/*
*******************************************************************
* Destructor
*******************************************************************
*/

ASPlayer::~ASPlayer()
{}

/*
*******************************************************************
* METHOD: Init
*******************************************************************
* Build the player object, setting its position in the world, its health
* as well as other traits 
*/

void ASPlayer::Init()
{}

/*
*******************************************************************
* METHOD: SetFrameTime
*******************************************************************
* Sets the frame speed so movements can be synchronised to what the
* graphics card is currently producing.  This will be used to 
* calculate how fast to rotate the camera in 3D Space (called at the 
* beginning of each frame)
*/

void ASPlayer::SetFrameTime(float time)
{
	m_frameTime = time;
}

/*
*******************************************************************
* METHOD: Get Rotation X
*******************************************************************
* Gets the players current rotation on the X axis (up/down)
*/

void ASPlayer::GetRotationX(float& rotX)
{
	rotX = m_rotationX;
}

/*
*******************************************************************
* METHOD: Get Rotation Y
*******************************************************************
* Gets the players current rotation on the Y axis (left/right)
*/

void ASPlayer::GetRotationY(float& rotY)
{
	rotY = m_rotationY;
}

/*
*******************************************************************
* METHOD: Turn Left
*******************************************************************
* Rotates the camera on its Y axis to the left
*/

void ASPlayer::TurnLeft(bool turning)
{
	// If the mouse is panning, increase the speed at which the rotation happens
	if(turning)
	{
		m_turnSpeed += m_frameTime * 0.01f;

		if(m_turnSpeed > (m_frameTime * 0.15f))
			m_turnSpeed = m_frameTime * 0.15f;
	}
	// If the mouse is not panning, decelerate the move speed, improves realism
	else 
	{
		m_turnSpeed -= m_frameTime * 0.005f;
		if(m_turnSpeed < 0.0f)
			m_turnSpeed = 0.0f;
	}

	// Update the players rotation
	m_rotationY -= m_turnSpeed;
	if(m_rotationY < 0.0f)
		m_rotationY += 360.0f;
}

/*
*******************************************************************
* METHOD: Turn Right
*******************************************************************
* Rotates the camera on its Y axis to the right
*/

void ASPlayer::TurnRight(bool turning)
{
	// If the mouse is panning, increase the speed at which the rotation happens
	if(turning)
	{
		m_turnSpeed += m_frameTime * 0.01f;

		if(m_turnSpeed > (m_frameTime * 0.15f))
			m_turnSpeed = m_frameTime * 0.15f;
	}
	// If the mouse is not panning, decelerate the move speed, improves realism
	else 
	{
		m_turnSpeed -= m_frameTime * 0.005f;
		if(m_turnSpeed < 0.0f)
			m_turnSpeed = 0.0f;
	}

	// Update the players rotation
	m_rotationY += m_turnSpeed;
	if(m_rotationY > 360.0f)
		m_rotationY -= 360.0f;
}

/*
*******************************************************************
* METHOD: Move Forward
*******************************************************************
* Advances the camera forward on its X axis
*/

void ASPlayer::MoveForward(bool moving)
{
	// If the mouse is panning, increase the speed at which the rotation happens
	if(moving)
	{
		m_runSpeed += m_frameTime * 0.01f;

		if(m_runSpeed > (m_frameTime * 0.15f))
			m_runSpeed = m_frameTime * 0.15f;
	}
	// If the mouse is not panning, decelerate the move speed, improves realism
	else 
	{
		m_runSpeed -= m_frameTime * 0.005f;
		if(m_runSpeed < 0.0f)
			m_runSpeed = 0.0f;
	}

	// Update the players rotation
	m_rotationX += m_turnSpeed;
	if(m_rotationX > 360.0f)
		m_rotationX -= 360.0f;
}

/*
*******************************************************************
* METHOD: Move Backward
*******************************************************************
* Moves the camera backward on its X axis
*/

void ASPlayer::MoveBackward(bool moving)
{
	// If the mouse is panning, increase the speed at which the rotation happens
	if(moving)
	{
		m_runSpeed += m_frameTime * 0.01f;

		if(m_runSpeed > (m_frameTime * 0.15f))
			m_runSpeed = m_frameTime * 0.15f;
	}
	// If the mouse is not panning, decelerate the move speed, improves realism
	else 
	{
		m_runSpeed -= m_frameTime * 0.005f;
		if(m_runSpeed < 0.0f)
			m_runSpeed = 0.0f;
	}

	// Update the players rotation
	m_rotationX += m_turnSpeed;
	if(m_rotationX > 360.0f)
		m_rotationX -= 360.0f;
}

/*
*******************************************************************
* METHOD: Get Pos X
*******************************************************************
* Gets the players X position
*/

void ASPlayer::GetPosX(float& posX)
{
	posX = 0.f;
}

/*
*******************************************************************
* METHOD: Get Pos Y
*******************************************************************
* Gets the players Y position
*/

void ASPlayer::GetPosY(float& posY)
{
	posY = 0.f;
}

/*
*******************************************************************
* METHOD: Strafe Left
*******************************************************************
* Strafes the camera left across its X axis
*/

/*
*******************************************************************
* METHOD: Strafe Right
*******************************************************************
* Strafes the camera right across its X axis
*/

/*
*******************************************************************
* METHOD: Release
*******************************************************************
* Dispose of any objects
*/

void ASPlayer::Release()
{}
