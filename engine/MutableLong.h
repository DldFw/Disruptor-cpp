#pragma once

#include <cstdint>
class MutableLong
{
	public:
		explicit MutableLong(std::int64_t value);

		std::int64_t value;
};

