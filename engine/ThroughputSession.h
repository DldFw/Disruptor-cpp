#pragma once

#include "Factory.h"
#include "ThroughputSessionResult.h"

class IThroughput;

class ThroughputSession
{
	public:
		explicit ThroughputSession(const ThroughputInfo& testInfo);

		void run();

	private:
		static void checkProcessorsRequirements(const std::shared_ptr<IThroughput>& test);
		std::int64_t getAverageThroughput();

#ifdef _DEBUG
		const std::int32_t m_runs = 3;
#else
		const std::int32_t m_runs = 7;
#endif

		std::vector< ThroughputSessionResult > m_results;
		ThroughputInfo m_testInfo;
};


