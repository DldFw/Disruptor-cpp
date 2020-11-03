#include "stdafx.h"
#include "ThroughputSessionResult.h"

#include <iomanip>
#include <sstream>

#include "DurationHumanizer.h"
#include "HumanNumberFacet.h"

ThroughputSessionResult::ThroughputSessionResult(std::int64_t totalOperationsInRun, Disruptor::ClockConfig::Duration duration)
	: m_totalOperationsInRun(totalOperationsInRun)
	  , m_duration(duration)
{}

ThroughputSessionResult::ThroughputSessionResult(const std::exception& exception)
	: m_exception(exception)
{}

void ThroughputSessionResult::appendDetailedHtmlReport(std::int32_t runId, std::ostringstream& os)
{
	if (m_exception)
	{
		os
			<< " <tr>" << std::endl
			<< "     <td>" << runId << "</td>" << std::endl
			<< "     <td>FAILED</td>" << std::endl
			<< "     <td>" << m_exception.get().what() << "</td>" << std::endl
			<< "</tr>" << std::endl;
	}
	else
	{
		os
			<< "<tr>" << std::endl
			<< "     <td>" << runId << "</td>" << std::endl
			<< "     <td>" << ops() << "</td>" << std::endl
			<< "     <td>" << durationInMilliseconds() << "</td>" << std::endl
			<< "</tr>" << std::endl;
	}
}

std::string ThroughputSessionResult::toString()
{
	std::ostringstream result;
	result.imbue(std::locale(result.getloc(), new HumanNumberFacet()));

	if (m_exception)
		result << "Run: FAILED: " << m_exception.get().what();
	else
	{
		auto humanDuration = DurationHumanizer::deduceHumanDuration(duration());

		result << "Run: Ops: " << ops()
			<< " - Duration (" << humanDuration.shortUnitName << "): " << std::setprecision(3) << humanDuration.value;
	}

	return result.str();
}

std::int64_t ThroughputSessionResult::totalOperationsInRun() const
{
	return m_totalOperationsInRun;
}

Disruptor::ClockConfig::Duration ThroughputSessionResult::duration() const
{
	return m_duration;
}

std::int64_t ThroughputSessionResult::ops() const
{
	auto elapsed = std::chrono::duration_cast< std::chrono::microseconds >(m_duration).count();
	return static_cast< std::int64_t >(static_cast< double >(1000 * 1000 * m_totalOperationsInRun) / elapsed);
}

std::int64_t ThroughputSessionResult::durationInMilliseconds() const
{
	auto totalMilliseconds = std::chrono::duration_cast< std::chrono::milliseconds >(m_duration);
	return static_cast< std::int64_t >(totalMilliseconds.count());
}

