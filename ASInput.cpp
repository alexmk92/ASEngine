
/*
******************************************************************
* ASInput.cpp
*******************************************************************
* Implements all method prototyped in header
*******************************************************************
*/

#include "ASInput.h"

/*
******************************************************************
* ASInput Constructors and Destructors
*******************************************************************
*/

ASInput::ASInput(){}

ASInput::ASInput(const ASInput& inp){}

ASInput::~ASInput(){}

/*
******************************************************************
* Method: Init()
*******************************************************************
* Initialises a new instance of the ASInput class; this 
* initialisation sets all keys to being in a releaed state.
*
* Note: DXInput will be implemented at a later date for better performance
*******************************************************************
*/

void ASInput::Init()
{
	for(int i = 0; i < 256; i++)
		m_keys[i] = false;

	return;
}

/*
******************************************************************
* Method: Key Down
*******************************************************************
* Updates the m_keys array to determine which key has been pressed
*******************************************************************
*/

void ASInput::KeyDown(unsigned int inputKey)
{
	m_keys[inputKey] = true;
	return;
}

/*
******************************************************************
* Method: Key Up
*******************************************************************
* Updates the m_keys array to determine which key has been releaed
*******************************************************************
*/

void ASInput::KeyUp(unsigned int inputKey)
{
	m_keys[inputKey] = false;
	return;
}

/*
******************************************************************
* Method: LeftMBDown
*******************************************************************
* Updates the flag to say left click is down
*******************************************************************
*/

void ASInput::LeftMBDown()
{
	m_lMbDown = true;
	return;
}

/*
******************************************************************
* Method: LeftMBUp
*******************************************************************
* Updates the flag to say left click is up
*******************************************************************
*/

void ASInput::LeftMBUp()
{
	m_lMbDown = false;
	return;
}

/*
******************************************************************
* Method: IsKeyDown
*******************************************************************
* Returns true if the requested key is currently press down, 
* else returns false
*******************************************************************
*/

bool ASInput::IsKeyDown(unsigned int inputKey)
{
	return m_keys[inputKey];
}

/*
******************************************************************
* Method: IsMBDown
*******************************************************************
* Returns true if the m_lbDown flag is true, else returns false
*******************************************************************
*/

bool ASInput::IsMBDown()
{
	return m_lMbDown;
}