
/*
******************************************************************
* ASPlayer.h
*******************************************************************
* Describes a player, a player in the world extends the ASEntity
* object, here it can override properties such as speed, an ASPlayer
* also has an ASCamera object attached to it
*******************************************************************
*/

#ifndef _ASPLAYER_H_
#define _ASPLAYER_H_

/*
*******************************************************************
* Includes:
*******************************************************************
*/

//#include "ASEntity.h"
#include <math.h>

/*
*******************************************************************
* Class definition
*******************************************************************
*/

class ASPlayer
{
public:
	// Constructors and destructor
	ASPlayer();
	ASPlayer(const ASPlayer&);
	~ASPlayer();

	// Other methods
	void Init();
	void Release();

	// Methods to control camera
	void SetFrameTime(float);					// syncs movement with the frame time regardless of FPS

	void SetPosition(float, float, float);		// set position of camera rel to world
	void SetRotation(float, float, float);		// set rotation of camera rel to world

	void GetPosition(float&, float&, float&);   // get cameras position in world
	void GetRotation(float&, float&, float&);   // get cameras rotation in world

	// Directional methods
	void TurnLeft(bool);
	void TurnRight(bool);
	void StrafeLeft(bool);
	void StrafeRight(bool);
	void MoveForward(bool);
	void MoveUpward(bool);
	void MoveDownward(bool);
	void MoveBackward(bool);
	void LookUpward(bool);
	void LookDownward(bool);
	void LookLeft(bool);
	void LookRight(bool);
	void Die(bool);

	// 
	void Fire();

private:
	// Member variables
	float m_frameTime;

	float m_posX;
	float m_posY;
	float m_posZ;

	float m_rotX;
	float m_rotY;
	float m_rotZ;

	float m_runSpeed;
	float m_climbSpeed;
	float m_lookSpeed;
	float m_lTurnSpeed;
	float m_rTurnSpeed;
	float m_jumpHeight;

	float m_weight;
};

const float GRAVITY = 9.82;
const float MAX_SPEED_MULTIPLIER = 0.15;
const float MAX_TURN_MULTIPLIER = 0.15f;
const float MAX_LOOK_MULTIPLIER = 0.25;

#endif