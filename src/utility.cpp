#include "utility.h"

namespace util
{
	uint32_t szudzik(uint32_t a, uint32_t b)
	{
		return a >= b ? a * a + a + b : a + b * b;
	}
	bool IsValidCharForName(char c)
    {
		const std::string whitelist = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890_()";
		if (whitelist.find(c) != std::string::npos)
			return true;
		else
			return false;
    }
}