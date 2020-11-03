#pragma once

#include <cstdint>

struct PaddedLong
{
	std::int64_t value = 0;
	char _padding[54];
};


