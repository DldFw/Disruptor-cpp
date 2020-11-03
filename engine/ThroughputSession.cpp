#include "stdafx.h"
#include "ThroughputSession.h"

#include <numeric>
#include <sstream>

#include <boost/filesystem.hpp>

#include "Stopwatch.h"

#include "Util.h"
#include "ThroughputSessionResult.h"

ThroughputSession::ThroughputSession(const ThroughputInfo& testInfo)
{
	m_testInfo = testInfo;
}

void ThroughputSession::run()
{
	auto testInstance = m_testInfo.factory();

	checkProcessorsRequirements(testInstance);

	std::cout << "Throughput  to run => " << m_testInfo.name << ", Runs => " << m_runs << std::endl;
	std::cout << "Starting throughput tests" << std::endl;

	Stopwatch stopwatch;

	for (auto i = 0; i < m_runs; i++)
	{
		stopwatch.reset();

		std::int64_t totalOperationsInRun;

		try
		{
			totalOperationsInRun = testInstance->run(stopwatch);
		}
		catch (std::exception& ex)
		{
			ThroughputSessionResult result(ex);
			std::cout << result.toString() << std::endl;

			m_results.push_back(result);
			continue;
		}

		ThroughputSessionResult result(totalOperationsInRun, stopwatch.elapsed());
		std::cout << result.toString() << std::endl;

		m_results.push_back(result);
	}
}

std::int64_t ThroughputSession::getAverageThroughput()
{
	double averageThroughput = 0;
	for (auto&& x : m_results)
	{
		auto totalSeconds = std::chrono::duration_cast< std::chrono::seconds >(x.duration());
		averageThroughput += static_cast< double >(x.totalOperationsInRun()) / totalSeconds.count();
	}

	return static_cast< std::int64_t >(averageThroughput / static_cast< double >(m_results.size()));
}


void ThroughputSession::checkProcessorsRequirements(const std::shared_ptr<IThroughput>& test)
{
	auto availableProcessors = static_cast< std::int32_t >(std::thread::hardware_concurrency());
	if (test->requiredProcessorCount() <= availableProcessors)
		return;

	std::cout << "*** Warning ***: your system has insufficient processors to execute the test efficiently. " << std::endl;
	std::cout << "Processors required = " << test->requiredProcessorCount() << ", available = " << availableProcessors << std::endl;
}

