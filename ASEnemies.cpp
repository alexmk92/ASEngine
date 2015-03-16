
/*
******************************************************************
* ASEnemies.cpp
*******************************************************************
* Implements all methods from ASEnemies.h
*******************************************************************
*/
	
#include "ASEnemies.h"

/*
*******************************************************************
* Constructor
*******************************************************************
* Init base objects
*/

ASEnemies::ASEnemies()
{
	m_enemyInfoList = 0;
	m_numEnemies = 0;
}

/*
*******************************************************************
* Empty Constructor
*******************************************************************
*/

ASEnemies::ASEnemies(const ASEnemies&)
{}

/*
*******************************************************************
* Destructor
*******************************************************************
*/

ASEnemies::~ASEnemies()
{}

/*
*******************************************************************
* METHOD: Init 
*******************************************************************
* Initialise the ASEnemies object
*/

bool ASEnemies::Init(int numEnemies)
{
	float r;
	float g;
	float b;

	// store the number of models
	m_numEnemies = numEnemies;

	// Create a list of the menemy info
	m_enemyInfoList = new ASEnemyInfo[m_numEnemies];
	if(!m_enemyInfoList)
		return false;

	// Randomly generate the position of each enemy in the scene
	for(unsigned int i = 0; i < m_numEnemies; i++)
	{
		// Generate a random color for the enemy
		r = (float)rand() / RAND_MAX;
		g = (float)rand() / RAND_MAX;
		b = (float)rand() / RAND_MAX;

		m_enemyInfoList[i].color = D3DXVECTOR4(r, g, b, 1.0f);

		// Generate a random position for the enemy to appear in the world
		m_enemyInfoList[i].posX = (((float)rand()-(float)rand())/RAND_MAX) * 10.0f;
		m_enemyInfoList[i].posY = (((float)rand()-(float)rand())/RAND_MAX) * 10.0f;
		m_enemyInfoList[i].posZ = ((((float)rand()-(float)rand())/RAND_MAX) * 10.0f) + 5.0f;
	}

	return true;
}

/*
*******************************************************************
* METHOD: Get Enemy Count
*******************************************************************
* Returns the number of enemies in the list
*/

int ASEnemies::GetEnemyCount()
{
	return m_numEnemies;
}

/*
*******************************************************************
* METHOD: Get Data
*******************************************************************
* Extracts all information on enemy at a given position, here we
* can check for collisions with the enemy and call its destructor
* method if need be
*/

void ASEnemies::GetData(int index, float& posX, float& posY, float& posZ, D3DXVECTOR4& color)
{
	posX = m_enemyInfoList[index].posX;
	posY = m_enemyInfoList[index].posY;
	posZ = m_enemyInfoList[index].posZ;

	color = m_enemyInfoList[index].color;
}

/*
*******************************************************************
* METHOD: Release
*******************************************************************
* Dispose of all enemies existing in the world
*/

void ASEnemies::Release()
{
	if(m_enemyInfoList)
	{
		delete [] m_enemyInfoList;
		m_enemyInfoList = 0;
	}
}
