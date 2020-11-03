#include "stdafx.h"
#include "Stopwatch.h"

Stopwatch::Stopwatch()
	: m_isRunning(false)
{
	m_begin = m_end = Disruptor::ClockConfig::Clock::now();
}

Disruptor::ClockConfig::Duration Stopwatch::elapsed() const
{
	if (isRunning())
		m_end = Disruptor::ClockConfig::Clock::now();

	return m_end - m_begin;
}

bool Stopwatch::isRunning() const
{
	return !!m_isRunning && m_isRunning.get();
}

void Stopwatch::reset()
{
	stop();
	m_begin = m_end;
	m_isRunning.reset();
}

Disruptor::ClockConfig::TimePoint Stopwatch::restart()
{
	reset();
	start();

	return m_begin;
}

void Stopwatch::start()
{
	// Should we resume or start the StopWatch?
	if (!m_isRunning)
	{
		m_isRunning = true;
		m_begin = Disruptor::ClockConfig::Clock::now();
	}
	else
		m_isRunning = true;
}

Stopwatch Stopwatch::startNew()
{
	Stopwatch stopwatch;
	stopwatch.start();

	return stopwatch;
}

std::int64_t Stopwatch::getTimestamp()
{
	return static_cast< std::int64_t >(Disruptor::ClockConfig::Clock::now().time_since_epoch().count());
}

void Stopwatch::stop()
{
	m_end = Disruptor::ClockConfig::Clock::now();
	m_isRunning = false;
}

