
/*
******************************************************************
* ASEngine.cpp
*******************************************************************
* Implements the prototype methods from ASEngine.h
*******************************************************************
*/

#include "ASFPSCounter.h"

/*
******************************************************************
* Default constructor - sets input and graphic pointers to null
* in the event that the Release function is called on a invalid
* initialise, this will clean the program up gracefully
******************************************************************
*/

ASFPSCounter::ASFPSCounter()
{}

/*
******************************************************************
* ASEngine Empty constructor
******************************************************************
*/

ASFPSCounter::ASFPSCounter(const ASFPSCounter& engine) 
{}

/*
******************************************************************
* ASEngine destructor 
******************************************************************
*/

ASFPSCounter::~ASFPSCounter()
{}

/*
******************************************************************
* METHOD: Init 
******************************************************************
* Initialise the ASFPSCounter object, setting all member vars to 0
*/

void ASFPSCounter::Init()
{
	m_fps = 0;
	m_count = 0;
	m_startTime = timeGetTime();
}

/*
******************************************************************
* METHOD: Increment Frame Count 
******************************************************************
* Called each frame to increment the frame count by 1, we can then
* calculate how many frames have elapsed.  Once a second has passed 
* the count is reset (because we are only concerned with the number
* of frames elapsed in one second)
*/

void ASFPSCounter::IncrementFrameCount()
{
	m_count++;

	// Check if we need to reset the timer (1sec passed)
	if(timeGetTime() >= (m_startTime + 1000))
	{
		m_fps = m_count;
		m_count = 0;

		m_startTime = timeGetTime();
	}
}

/*
******************************************************************
* METHOD: Get FPS
******************************************************************
* Return the amount of frames processed this second
*/

int ASFPSCounter::GetFps()
{
	return m_fps;
}