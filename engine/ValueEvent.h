#pragma once

#include <cstdint>
#include <functional>

class ValueEvent
{
	public:
		static const std::function< ValueEvent() >& eventFactory();

		std::int64_t value;
};


