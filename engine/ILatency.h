#pragma once

#include <cstdint>

#include "LatencyRecorder.h"
#include "Stopwatch.h"

class ILatency
{
	public:
		virtual ~ILatency() = default;

		virtual void run(Stopwatch& stopwatch, const std::shared_ptr< LatencyRecorder >& latencyRecorder) = 0;

		virtual std::int32_t requiredProcessorCount() const = 0;
};

