#include "stdafx.h"
#include "LatencySessionResult.h"

#include <iomanip>
#include <sstream>

#include "DurationHumanizer.h"
#include "HumanNumberFacet.h"

LatencySessionResult::LatencySessionResult(const std::shared_ptr< LatencyRecorder >& latencyRecorder, const Disruptor::ClockConfig::Duration& duration)
	: m_latencyRecorder(latencyRecorder)
	  , m_duration(duration)
{
}

LatencySessionResult::LatencySessionResult(const std::exception& exception)
	: m_exception(exception)
{
}

std::string LatencySessionResult::toString()
{
	std::ostringstream result;
	result.imbue(std::locale(result.getloc(), new HumanNumberFacet()));

	if (m_exception)
		result << "Run: FAILED: " << m_exception.get().what();
	else
	{
		auto humanDuration = DurationHumanizer::deduceHumanDuration(duration());

		result << "Run: "
			<< "Duration (" << humanDuration.shortUnitName << "): " << std::setprecision(3) << humanDuration.value
			<< ", Latency: ";
		m_latencyRecorder->writeReport(result);
	}

	return result.str();
}

Disruptor::ClockConfig::Duration LatencySessionResult::duration() const
{
	return m_duration;
}

