
/*
******************************************************************
* ASCPUMonitor.h
*******************************************************************
* Keeps track of how many resources the CPU is using each second -
* used to track performance
*******************************************************************
*/

#ifndef _ASCPUMONITOR_H_
#define _ASCPUMONITOR_H_

/*
******************************************************************
* Pre-processing directives
******************************************************************
*/

#pragma comment(lib, "pdh.lib")

/*
******************************************************************
* Includes
******************************************************************
* + pdh - Win lib to query CPU usage
*/

#include <pdh.h>

/*
******************************************************************
* Class declaration
******************************************************************
*/

class ASCPUMonitor 
{
public:
	// Constructors / Destructor prototype
	ASCPUMonitor();
	ASCPUMonitor(const ASCPUMonitor&);
	~ASCPUMonitor();

	// Public methods
	void Init();
	void Release();
	void UpdateCPUUsage();
	int  GetCPUUsage();

private:
	// Private members
	bool		  m_canReadCPU;
	HQUERY		  m_queryHandle;
	HCOUNTER	  m_counterHandle;
	unsigned long m_lastSampleTime;
	long		  m_cpuUsage;
};

#endif