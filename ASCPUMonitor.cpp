
/*
******************************************************************
* ASCPUMonitor.cpp
*******************************************************************
* Implements the prototype methods from ASCPUMonitor.h
*******************************************************************
*/

#include "ASCPUMonitor.h"

/*
******************************************************************
* Default constructor
******************************************************************
*/

ASCPUMonitor::ASCPUMonitor()
{}

/*
******************************************************************
* ASEngine Empty constructor
******************************************************************
*/

ASCPUMonitor::ASCPUMonitor(const ASCPUMonitor& cpuMonitor) 
{}

/*
******************************************************************
* ASEngine destructor 
******************************************************************
*/

ASCPUMonitor::~ASCPUMonitor()
{}

/*
******************************************************************
* METHOD: Init 
******************************************************************
* Initialise the CPU Monitor, this method sets up the handle which
* is used to query the CPU on its total usage.  This is a combined
* CPU usage, and not individual units on the system.  If a handle
* can't be acquired then the cpu usage returned will always be 0%
*/

void ASCPUMonitor::Init()
{
	PDH_STATUS status;

	// Initialse the CPU flag to true by default - this will be set
	// to false if a handle cannot be acquired
	m_canReadCPU = true;

	// Check if a handle can be acquired on the CPU 
	status = PdhOpenQuery(NULL, 0, &m_queryHandle);
	if(status != ERROR_SUCCESS)
		m_canReadCPU = false;

	// Query to poll CPU usage of call CPUs in the system
	status = PdhAddCounter(m_queryHandle, TEXT("\\Processor(_Total)\\% processor time"), 0, &m_counterHandle);
	if(status != ERROR_SUCCESS)
		m_canReadCPU = false;

	// Keep updating the sample time on each tick 
	m_lastSampleTime = GetTickCount();
	m_cpuUsage = 0;
}

/*
******************************************************************
* METHOD: UpdateCPUUsage
******************************************************************
* Called each frame to update the total CPU usage, m_lastSampleTime
* is used to ensure to many queries are not run (this would introduce
* performance hits) - therefore a sample will only be taken each second
*/

void ASCPUMonitor::UpdateCPUUsage()
{
	PDH_FMT_COUNTERVALUE val;

	// Check if we have access to the CPU
	if(m_canReadCPU)
	{
		if((m_lastSampleTime + 1000) < GetTickCount())
		{
			m_lastSampleTime = GetTickCount();
			PdhCollectQueryData(m_queryHandle);
			PdhGetFormattedCounterValue(m_counterHandle, PDH_FMT_LONG, NULL, &val);

			// Update the CPU usage for the current tick
			m_cpuUsage = val.longValue;
		}
	}
}

/*
******************************************************************
* METHOD: Get CPU Usage
******************************************************************
* Returns the current (%) of CPU usage the application is using
* 
* @return int - the total amount of CPU usage to 0.dp
*/

int ASCPUMonitor::GetCPUUsage()
{
	int usage;
	if(m_canReadCPU)
		return (int)m_cpuUsage;
	else 
		return 0;
}

/*
******************************************************************
* METHOD: Release
******************************************************************
* Dispose of any resources held by the CPU monitor
*/

void ASCPUMonitor::Release()
{
	// Release the query handle, only if it is open
	if(m_canReadCPU)
	{
		PdhCloseQuery(m_queryHandle);
	}
}