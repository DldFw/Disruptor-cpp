#pragma once

#include <cstdint>
#include <exception>
#include <iosfwd>
#include <boost/optional.hpp>
#include <Disruptor/ClockConfig.h>
#include "LatencyRecorder.h"

class LatencySessionResult
{
	public:
		LatencySessionResult(const std::shared_ptr< LatencyRecorder >& latencyRecorder, const Disruptor::ClockConfig::Duration& duration);

		explicit LatencySessionResult(const std::exception& exception);

		std::string toString();

		Disruptor::ClockConfig::Duration duration() const;

	private:
		std::shared_ptr< LatencyRecorder > m_latencyRecorder;
		Disruptor::ClockConfig::Duration m_duration;
		boost::optional< std::exception > m_exception;
};

