#pragma once
#include <chrono>

class Timer
{
public:
	Timer();
	~Timer();

	void Start(double time, bool auto_restart);
	bool Tick();
	void Restart();
private:
	bool started = false;
	bool auto_restart = false;
	std::chrono::high_resolution_clock::time_point start_point;
	double time = 0.0;
};