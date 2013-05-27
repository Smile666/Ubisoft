//========================================================================
// Timer.cpp
//
// This code is part of Ubisoft Programmer Test 
//
// Coded by Muralev Evgeny
//========================================================================

#include "BlurTestStd.h"
#include "Timer.h"

Timer::Timer() : m_i64GameTime(0)
{
	//get cpu frequency
	__int64 freq;
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);

	//calculate number of ticks in one ms
	m_fTicksInMs = (float)( (float)freq / 1000.0);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_i64StartTime);
}

void Timer::Tick()
{
	__int64 i64CurrTime;
	float fDeltaTime;

	//get current time
	QueryPerformanceCounter((LARGE_INTEGER*)&i64CurrTime);

	//calculate delta
	fDeltaTime = (float)(i64CurrTime - m_i64StartTime);

	//convert to milliseconds
	m_fFrameTime = fDeltaTime / m_fTicksInMs;

	//update time of previous frame
	m_i64StartTime = i64CurrTime;

	//sum up total time
	m_i64GameTime += fDeltaTime;
}