//========================================================================
// Timer.h
//
// This code is part of Ubisoft Programmer Test 
//
// Coded by Muralev Evgeny
//========================================================================
#pragma once

class Timer
{
private:

	//Time of the last frame (in ticks)
	__int64 m_i64StartTime;

	//Number of CPU ticks in millisecond
	float m_fTicksInMs;

	//Time taken for frame(in ms)
	float m_fFrameTime;

	//Time passed since game started
	__int64	m_i64GameTime;

public:
		
	Timer();	//constructor
	~Timer() {} //destructor

	//Tick
	void Tick();

	//Getters
	float GetGameTime() const { return m_i64GameTime / m_fTicksInMs / 1000.0; }
	float GetDeltaTime() const { return m_fFrameTime / 1000.0; }
};