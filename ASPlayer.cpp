
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

	m_rotX = 0.0f;
	m_rotY = 0.0f;
	m_rotZ = 0.0f;

	m_jumpHeight  = 0.f;
	m_runSpeed    = 15.0f;
	m_lTurnSpeed  = 0.f;
	m_rTurnSpeed  = 0.f;
	m_climbSpeed  = 0.f;
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
* METHOD: Set Frame Time
*******************************************************************
* Sets the frame time so movements can be synchronised to what the
* graphics card is currently rendering at.  This will be used to 
* calculate how fast to rotate the camera in 3D Space (called at the 
* beginning of each frame), the time quantum that is passed in comes
* from the high precision ASFrameTimer class and is passed in from
* ASEngine
*/

void ASPlayer::SetFrameTime(float time)
{
	m_frameTime = time;
}

/*
*******************************************************************
* METHOD: Get Position
*******************************************************************
* Gets the players current position in the world, this is used to
* set the position of ASCamera.
*
* @param float& - output parameter for X position
* @param float& - output parameter for Y position
* @param float& - output parameter for Z position
*/

void ASPlayer::GetPosition(float& posX, float& posY, float& posZ)
{
	posX = m_posX;
	posY = m_posY;
	posZ = m_posZ;
}

/*
*******************************************************************
* METHOD: Get Rotation 
*******************************************************************
* Gets the players current rotation in the world, this is used to
* set the rotation for of ASCamera.
*
* @param float& - output parameter for X position
* @param float& - output parameter for Y position
* @param float& - output parameter for Z position
*/

void ASPlayer::GetRotation(float& rotX, float& rotY, float& rotZ)
{
	rotX = m_rotX;
	rotY = m_rotY;
	rotZ = m_rotZ;
}

/*
*******************************************************************
* METHOD: Set Position
*******************************************************************
* Sets the players current position in the world
*
* @param float - input parameter for X position
* @param float - input parameter for Y position
* @param float - input parameter for Z position
*/

void ASPlayer::SetPosition(float posX, float posY, float posZ)
{
	m_posX = posX;
	m_posY = posY;
	m_posZ = posZ;
}

/*
*******************************************************************
* METHOD: Set Rotation
*******************************************************************
* Sets the players current position in the world
*
* @param float - input parameter for X position
* @param float - input parameter for Y position
* @param float - input parameter for Z position
*/

void ASPlayer::SetRotation(float rotX, float rotY, float rotZ)
{
	m_rotX = rotX;
	m_rotY = rotY;
	m_rotZ = rotZ;
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
		m_lTurnSpeed += m_frameTime * 0.01f;

		if(m_lTurnSpeed > (m_frameTime * 0.15f))
			m_lTurnSpeed = m_frameTime * 0.15f;
	}
	// If the mouse is not panning, decelerate the move speed, improves realism
	else 
	{
		m_lTurnSpeed -= m_frameTime * 0.005f;
		if(m_lTurnSpeed < 0.0f)
			m_lTurnSpeed = 0.0f;
	}

	// Update the players rotation
	m_rotY -= m_lTurnSpeed;
	if(m_rotY < 0.0f)
		m_rotY += 360.0f;
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
		m_rTurnSpeed += m_frameTime * 0.01f;

		if(m_rTurnSpeed > (m_frameTime * 0.15f))
			m_rTurnSpeed = m_frameTime * 0.15f;
	}
	// If the mouse is not panning, decelerate the move speed, improves realism
	else 
	{
		m_rTurnSpeed -= m_frameTime * 0.005f;
		if(m_rTurnSpeed < 0.0f)
			m_rTurnSpeed = 0.0f;
	}

	// Update the players rotation
	m_rotY += m_rTurnSpeed;
	if(m_rotY > 360.0f)
		m_rotY -= 360.0f;
}

/*
*******************************************************************
* METHOD: Move Forward
*******************************************************************
* Advances the camera forward on its X axis
*/

void ASPlayer::MoveForward(bool moving)
{
	// If the forward key is pressed, then gradually accelerate the player
	if(moving)
	{
		m_runSpeed += m_frameTime * 0.01f;

		// Set the max speed for the user
		if(m_runSpeed > (m_frameTime * MAX_SPEED_MULTIPLIER))
			m_runSpeed = m_frameTime * MAX_SPEED_MULTIPLIER;
	}
	// If forward key is not pressed, then decelerate the player to 0.0f
	else 
	{
		m_runSpeed -= m_frameTime * 0.005f;
		if(m_runSpeed < 0.0f)
			m_runSpeed = 0.0f;
	}

	// Update the players position
	float radians = m_rotY * 0.0174532925f;
	m_posX += sinf(radians) * m_runSpeed;
	m_posZ += cosf(radians) * m_runSpeed;
}

/*
*******************************************************************
* METHOD: Move Backward
*******************************************************************
* Moves the camera backward on its X axis
*/

void ASPlayer::MoveBackward(bool moving)
{
	// If the down key is pressed, then gradually accelerate the player
	if(moving)
	{
		m_runSpeed += m_frameTime * 0.01f;

		// Set the max speed for the user
		if(m_runSpeed > (m_frameTime * MAX_SPEED_MULTIPLIER))
			m_runSpeed = m_frameTime * MAX_SPEED_MULTIPLIER;
	}
	// If down key is not pressed, then decelerate the player to 0.0f
	else 
	{
		m_runSpeed -= m_frameTime * 0.005f;
		if(m_runSpeed < 0.0f)
			m_runSpeed = 0.0f;
	}

	// Update the players position
	float radians = m_rotY * 0.0174532925f;
	m_posX -= sinf(radians) * m_runSpeed;
	m_posZ -= cosf(radians) * m_runSpeed;
}

/*
*******************************************************************
* METHOD: Move Upward
*******************************************************************
* Describes how the player will climb the Y axis, this will be used
* for scaling terrain etc
*/

void ASPlayer::MoveUpward(bool moving)
{
	// If the down key is pressed, then gradually accelerate the player
	if(moving)
	{
		m_climbSpeed += m_frameTime * 0.01f;

		// Set the max speed for the user
		if(m_climbSpeed > (m_frameTime * MAX_SPEED_MULTIPLIER))
			m_climbSpeed = m_frameTime * MAX_SPEED_MULTIPLIER;
	}
	// If down key is not pressed, then decelerate the player to 0.0f
	else 
	{
		m_climbSpeed -= m_frameTime * 0.05f;
		if(m_climbSpeed < 0.0f)
			m_climbSpeed = 0.0f;
	}

	// Update the players position
	m_posY += m_climbSpeed;
}

/*
*******************************************************************
* METHOD: Move Downward
*******************************************************************
* Describes how the player will descend off the Y axis, this will
* be used for when the player climbs down mountain terrain
*/

void ASPlayer::MoveDownward(bool moving)
{
	// If the down key is pressed, then gradually accelerate the player
	if(moving)
	{
		m_climbSpeed += m_frameTime * 0.01f;

		// Set the max speed for the user
		if(m_climbSpeed > (m_frameTime * MAX_SPEED_MULTIPLIER))
			m_climbSpeed = m_frameTime * MAX_SPEED_MULTIPLIER;
	}
	// If down key is not pressed, then decelerate the player to 0.0f
	else 
	{
		m_climbSpeed -= m_frameTime * 0.05f;
		if(m_climbSpeed < 0.0f)
			m_climbSpeed = 0.0f;
	}

	if(m_posY > 1.0f)
		m_posY -= m_climbSpeed;
}


/*
*******************************************************************
* METHOD: Look Upward
*******************************************************************
* allows the camera to look upward into the sky, this is capped at
* a 90 deg angle 
*/

void ASPlayer::LookUpward(bool moving)
{
	// If the down key is pressed, then gradually accelerate the player
	if(moving)
	{
		m_lookSpeed += m_frameTime * 0.01f;

		// Set the max speed for the user
		if(m_lookSpeed > (m_frameTime * MAX_LOOK_MULTIPLIER))
			m_lookSpeed = m_frameTime * MAX_LOOK_MULTIPLIER;
	}
	// If down key is not pressed, then decelerate the player to 0.0f
	else 
	{
		m_lookSpeed -= m_frameTime * 0.05f;
		if(m_lookSpeed < 0.0f)
			m_lookSpeed = 0.0f;
	}

	// Update the players look axis
	m_rotX -= m_lookSpeed;

	// Cap the look axis to 90 deg
	if(m_rotX > 90.0f)
		m_rotX = 90.0f;
}

/*
*******************************************************************
* METHOD: Look Downward
*******************************************************************
* allows the camera to look downward into the ground, this is capped at
* a 90 deg angle 
*/

void ASPlayer::LookDownward(bool moving)
{
	// If the down key is pressed, then gradually accelerate the player
	if(moving)
	{
		m_lookSpeed += m_frameTime * 0.01f;

		// Set the max speed for the user
		if(m_lookSpeed > (m_frameTime * MAX_LOOK_MULTIPLIER))
			m_lookSpeed = m_frameTime * MAX_LOOK_MULTIPLIER;
	}
	// If down key is not pressed, then decelerate the player to 0.0f
	else 
	{
		m_lookSpeed -= m_frameTime * 0.05f;
		if(m_lookSpeed < 0.0f)
			m_lookSpeed = 0.0f;
	}

	// Update the players look axis
	m_rotX += m_lookSpeed;

	// Cap the look axis to 90 deg
	if(m_rotX < -90.0f)
		m_rotX = -90.0f;
}

/*
*******************************************************************
* METHOD: Release
*******************************************************************
* Dispose of any objects
*/

void ASPlayer::Release()
{}
