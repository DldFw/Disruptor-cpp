#include "stdafx.h"
#include "LatencySession.h"
#include "LatencyRecorder.h"
#include "Stopwatch.h"

LatencySession::LatencySession(const LatencyInfo& testInfo)
{
	m_Info = testInfo;
}

void LatencySession::run()
{
	auto testInstance = m_Info.factory();

	checkProcessorsRequirements(testInstance);

	std::cout << "Latency  to run => " << m_Info.name << ", Runs => " << m_runs << std::endl;
	std::cout << "Starting latency tests" << std::endl;;

	Stopwatch stopwatch;
	for (auto i = 0; i < m_runs; ++i)
	{
		stopwatch.reset();
		auto histogram = std::make_shared< LatencyRecorder >(100000000L);

		try
		{
			testInstance->run(stopwatch, histogram);
		}
		catch (std::exception& ex)
		{
			LatencySessionResult result(ex);
			std::cout << result.toString() << std::endl;
			m_results.push_back(result);
			continue;
		}

		LatencySessionResult result(histogram, stopwatch.elapsed());
		std::cout << result.toString() << std::endl;
		m_results.push_back(result);
	}
}

void LatencySession::checkProcessorsRequirements(const std::shared_ptr< ILatency >& test)
{
	auto availableProcessors = static_cast< std::int32_t >(std::thread::hardware_concurrency());
	if (test->requiredProcessorCount() <= availableProcessors)
		return;

	std::cout << "*** Warning ***: your system has insufficient processors to execute the test efficiently. " << std::endl;
	std::cout << "Processors required = " << test->requiredProcessorCount() << ", available = " << availableProcessors << std::endl;
}

std::int64_t LatencySession::convertStopwatchTicksToNano(double durationInTicks)
{
	auto clockFrequency = static_cast< double >(Disruptor::ClockConfig::Duration::period::den) / static_cast< double >(Disruptor::ClockConfig::Duration::period::num);
	auto durationInNano = (durationInTicks / clockFrequency) * std::pow(10, 9);
	return static_cast< std::int64_t >(durationInNano);
}

double LatencySession::convertNanoToStopwatchTicks(std::int64_t pauseDurationInNanos)
{
	auto clockFrequency = static_cast< double >(Disruptor::ClockConfig::Duration::period::den) / static_cast< double >(Disruptor::ClockConfig::Duration::period::num);
	return pauseDurationInNanos * std::pow(10, -9) * clockFrequency;
}

