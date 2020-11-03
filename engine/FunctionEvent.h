#pragma once

#include <functional>
#include <memory>

class FunctionEvent
{
	public:
		static const std::function< FunctionEvent() >& eventFactory();

		std::int64_t operandOne = 0;
		std::int64_t operandTwo = 0;
		std::int64_t stepOneResult = 0;
		std::int64_t stepTwoResult = 0;
};


