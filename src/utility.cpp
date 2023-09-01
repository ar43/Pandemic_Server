#include "utility.h"

namespace util
{
	uint32_t szudzik(uint32_t a, uint32_t b)
	{
		return a >= b ? a * a + a + b : a + b * b;
	}
    uint64_t NameToLong(std::string name)
    {
		uint64_t longName = 0L;
		for (int c = 0; c < name.length() && c < 12; c++) {
			char character = name.at(c);
			longName *= 37L;
			if (character >= 'A' && character <= 'Z') {
				longName += (1 + character) - 65;
			} else if (character >= 'a' && character <= 'z') {
				longName += (1 + character) - 97;
			} else if (character >= '0' && character <= '9') {
				longName += (27 + character) - 48;
			}
		}

		for (; longName % 37L == 0L && longName != 0L; longName /= 37L) {
		}
		return longName;
    }
}