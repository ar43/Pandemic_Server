#include "utility.h"

namespace util
{
	uint32_t szudzik(uint32_t a, uint32_t b)
	{
		return a >= b ? a * a + a + b : a + b * b;
	}
}