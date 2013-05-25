#include "BlurTestStd.h"
#include "Timer.h"

Timer::Timer() : m_i64GameTime(0), m_i64PrevTime(0), m_i64CurrTime(0), m_dDeltaTime(0)
{
	//get amount of cycles per second
	QueryPerformanceCounter((LARGE_INTEGER*)&m_uCountsPerSec); 

	//calculate amount of seconds per cycle
	m_dSecondsPerCount = 1.0 / (double)m_uCountsPerSec;
}

void Timer::Tick()
{
	//Assign current time to previous
	m_i64PrevTime = m_i64CurrTime;

	//Update current time
	QueryPerformanceCounter((LARGE_INTEGER*)&m_i64CurrTime);

	//Calculate time difference
	__int64 diff = m_i64CurrTime - m_i64PrevTime;
	m_dDeltaTime = diff * m_dSecondsPerCount;

	//Increment game time
	m_i64GameTime += diff;

	//if overflow still happened...
	if (m_dDeltaTime < 0) m_dDeltaTime = 0;
}