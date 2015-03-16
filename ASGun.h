
/*
******************************************************************
* ASGun.h
*******************************************************************
* Base class for a gun object
*******************************************************************
*/

#ifndef _ASGUN_H_
#define _ASGUN_H_

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

class ASGun 
{
private:
	// Struct to hold all info on bullet
	struct ASBullet
	{
		D3DXVECTOR3  pos;
		float        lifeTime;
		unsigned int speed;
	};
public:
	// Constructors and destructor
	ASGun();
	ASGun(const ASGun&);
	~ASGun();

	// Other methods
	bool Init(int);

	virtual void Fire();
	virtual void Release();

private:
	// Member variables
	ASGun*      m_weapon;
	ASBullet*   m_bullet;
};

#endif