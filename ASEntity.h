
/*
******************************************************************
* ASEntity.h
*******************************************************************
* Describes a "living thing" that exists in the world, ASEntity
* is the base class for any player or creature object 
* that live in the world 
*******************************************************************
*/

#ifndef _ASENTITY_H_
#define _ASENTITY_H_

/*
*******************************************************************
* Includes:
*******************************************************************
*/

#include <D3DX10math.h>

/*
*******************************************************************
* Class definition
*******************************************************************
*/

class ASEntity 
{
public:
	// Constructors and destructor
	ASEntity();
	ASEntity(const ASEntity&);
	~ASEntity();

	// Other methods
	bool Init(int);

	// Parent methods - can be overriden by inherited children
	virtual void Die();
	virtual void TurnLeft(bool);
	virtual void TurnRight(bool);
	virtual void StrafeLeft(bool);
	virtual void StrafeRight(bool);
	virtual void MoveForward(bool);
	virtual void MoveBackward(bool);


private:
	// Member variables
	float       m_health;
	float       m_speed;
	float       m_scale;
	D3DXVECTOR3 m_pos;
	//ASGun*      m_weapon;
};

#endif