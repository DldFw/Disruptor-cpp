#pragma once

#include <memory>
#include <string>

#include "Factory.h"
#include "LatencySessionResult.h"

class ILatency;


class LatencySession
{
	public:
		explicit LatencySession(const LatencyInfo& Info);

		void run();

		static std::int64_t convertStopwatchTicksToNano(double durationInTicks);
		static double convertNanoToStopwatchTicks(std::int64_t pauseDurationInNanos);

	private:
		static void checkProcessorsRequirements(const std::shared_ptr< ILatency >& );

		const std::int32_t m_runs = 3;

		LatencyInfo m_Info;
		std::vector< LatencySessionResult > m_results;
};
