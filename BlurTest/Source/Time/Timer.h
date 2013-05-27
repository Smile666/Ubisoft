#pragma once

class Timer
{
private:
		//Amount of CPU cycles per second 
		unsigned int	m_uCountsPerSec;

		//Fraction of second per cycle
		double m_dSecondsPerCount;

		//Previous Game Time
		__int64	m_i64PrevTime;

		//Current Game Time
		__int64  m_i64CurrTime;

		//Time passed since game started
		__int64	m_i64GameTime;

		//Elapsed time
		double m_dDeltaTime;

public:

		
		Timer();	//constructor
		~Timer() {} //destructor

		//Tick
		void Tick();

		/***
			//Getters
						***/
		double GetGameTime() const { return m_i64GameTime * m_dSecondsPerCount * 1000.0; }
		double GetDeltaTime() const { return m_dDeltaTime * 1000.0; }
};