#pragma once

#include <chrono>
#include <boost/optional.hpp>

#include <Disruptor/ClockConfig.h>
class Stopwatch
{
	public:
		Stopwatch();

		Disruptor::ClockConfig::Duration elapsed() const;

		bool isRunning() const;

		static Stopwatch startNew();

		static std::int64_t getTimestamp();

		void start();
		void stop();

		void reset();

		Disruptor::ClockConfig::TimePoint restart();

	private:
		boost::optional< bool > m_isRunning;
		Disruptor::ClockConfig::TimePoint m_begin;
		mutable Disruptor::ClockConfig::TimePoint m_end;
};


