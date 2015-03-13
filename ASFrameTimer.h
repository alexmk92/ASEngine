
/*
******************************************************************
* ASFrameTimer.h
*******************************************************************
* High precision timer to keep track of how many seconds have elapsed
* between each frame - this is used to synchronise movements of
* objects in the world regardless of framerate
*******************************************************************
*/

#ifndef _ASFRAMETIMER_H_
#define _ASFRAMETIMER_H_

/*
******************************************************************
* Includes
******************************************************************
* + pdh - Win lib to query CPU usage
*/

#include <windows.h>

/*
******************************************************************
* Class declaration
******************************************************************
*/

class ASFrameTimer 
{
public:
	// Constructors / Destructor prototype
	ASFrameTimer();
	ASFrameTimer(const ASFrameTimer&);
	~ASFrameTimer();

	// Public methods
	bool  Init();
	void  GetFrame();
	float GetTime();

private:
	// Private members
	INT64 m_freq;
	INT64 m_startTime;
	float m_tickPerMs;
	float m_frameTime;
};

#endif