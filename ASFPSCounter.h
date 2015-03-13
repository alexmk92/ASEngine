
/*
******************************************************************
* ASFPSCounter.h
*******************************************************************
* Keeps track of how many frames per second the application is
* ticking over at
*******************************************************************
*/

#ifndef _ASFPSCOUNTER_H_
#define _ASFPSCOUNTER_H_

/*
******************************************************************
* Pre-processing directives
******************************************************************
*/

#pragma comment(lib, "winmm.lib")

/*
******************************************************************
* Includes
******************************************************************
*/

#include <windows.h>
#include <mmsystem.h>

/*
******************************************************************
* Class declaration
******************************************************************
*/

class ASFPSCounter 
{
public:
	// Constructors / Destructor prototype
	ASFPSCounter();
	ASFPSCounter(const ASFPSCounter&);
	~ASFPSCounter();

	// Public methods
	void Init();
	void IncrementFrameCount();
	int  GetFps();

private:
	// Private members
	int m_fps;
	int m_count;
	unsigned long m_startTime;
};

#endif