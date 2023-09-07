#include "timer.h"

Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::Start(double time, bool auto_restart)
{
	started = true;
	this->auto_restart = auto_restart;
	start_point = std::chrono::high_resolution_clock::now();
	this->time = time;
}

bool Timer::Tick()
{
	if (!started)
		return false;

	auto current_time = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double, std::milli> time_span = current_time - start_point;
	if (time_span.count() >= time)
	{
		if (auto_restart)
		{
			Start(time, auto_restart);
		}
		else
		{
			started = false;
		}
		return true;
	}
	else
	{
		return false;
	}
}

void Timer::Restart()
{
	Start(time, auto_restart);
}
