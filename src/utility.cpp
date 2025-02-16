#include "utility.h"
#include <chrono>
#include <ctime>
#include <ratio>
#include <thread>

namespace util
{

	void SleepFor(double seconds)
	{
		using namespace std;
		using namespace std::chrono;

		static double estimate = 5e-3;
		static double mean = 5e-3;
		static double m2 = 0;
		static int64_t count = 1;

		while (seconds > estimate) {
			auto start = high_resolution_clock::now();
			std::this_thread::sleep_for(milliseconds(1));
			auto end = high_resolution_clock::now();

			double observed = (end - start).count() / 1e9;
			seconds -= observed;

			++count;
			double delta = observed - mean;
			mean += delta / count;
			m2   += delta * (observed - mean);
			double stddev = sqrt(m2 / (count - 1));
			estimate = mean + stddev;
		}

		// spin lock
		auto start = high_resolution_clock::now();
		while ((high_resolution_clock::now() - start).count() / 1e9 < seconds);
	}

	uint32_t szudzik(uint32_t a, uint32_t b)
	{
		return a >= b ? a * a + a + b : a + b * b;
	}
	bool IsValidCharForName(char c)
    {
		const std::string whitelist = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_()";
		if (whitelist.find(c) != std::string::npos)
			return false;
		else
			return true;
    }
}