
/*
******************************************************************
* ASEnemies.h
*******************************************************************
* Maintains a list of all enemies and their position within the 
* world.
*******************************************************************
*/

#ifndef _ASENEMIES_H_
#define _ASENEMIES_H_

/*
*******************************************************************
* Includes:
*******************************************************************
*/

#include <D3DX10math.h>
#include <stdlib.h>
#include <time.h>

/*
*******************************************************************
* Includes:
*******************************************************************
*/

class ASEnemies 
{
private:
	// Structure to hold information on an enemy entity
	struct ASEnemyInfo	
	{
		D3DXVECTOR4 color;
		float posX;
		float posY;
		float posZ;
	};
public:
	// Constructors and destructor
	ASEnemies();
	ASEnemies(const ASEnemies&);
	~ASEnemies();

	// Other methods
	bool Init(int);
	void Release();

	int GetEnemyCount();
	void GetData(int, float&, float&, float&, D3DXVECTOR4&);

private:
	// Member variables
	int m_numEnemies;
	ASEnemyInfo* m_enemyInfoList;
};

#endif