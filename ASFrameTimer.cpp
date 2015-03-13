
/*
******************************************************************
* ASFrameTimer.cpp
*******************************************************************
* Implements the prototype methods from ASFrameTimer.h
*******************************************************************
*/

#include "ASFrameTimer.h"

/*
******************************************************************
* Default constructor
******************************************************************
*/

ASFrameTimer::ASFrameTimer()
{}

/*
******************************************************************
* ASEngine Empty constructor
******************************************************************
*/

ASFrameTimer::ASFrameTimer(const ASFrameTimer& cpuMonitor) 
{}

/*
******************************************************************
* ASEngine destructor 
******************************************************************
*/

ASFrameTimer::~ASFrameTimer()
{}

/*
******************************************************************
* METHOD: Init 
******************************************************************
* Initialise the Frame Timer, this will query the system to assertain
* if it supports high frq timers, if it does the systems frequency 
* will be used to determine how many ticks occur per ms.  
* 
* @return bool - true if highFreq is supported, else false
*/

bool ASFrameTimer::Init()
{
	// Check if the system supports high freq timers, if a value is
	// written to the output param (m_freq), then we have the systems freq
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_freq);
	if(m_freq == 0)
		return false;

	// Determine how many times per ms the counter should tick
	m_tickPerMs = (float)(m_freq / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return true;
}

/*
******************************************************************
* METHOD: UpdateCPUUsage
******************************************************************
* Called for every single loop of execution as the game runs, 
* allowing frames to remain synchronised regardless of FPS
*/

void ASFrameTimer::GetFrame()
{
	INT64 currTime;
	float timeDiff;

	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	timeDiff = (float)(currTime - m_startTime);
	m_frameTime = timeDiff / m_tickPerMs;

	m_startTime = currTime;
}

/*
******************************************************************
* METHOD: Get Time
******************************************************************
* Returns the current time
*/

float ASFrameTimer::GetTime()
{
	return m_frameTime;
}
