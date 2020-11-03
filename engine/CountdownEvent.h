#pragma once

#include <atomic>

#include <Disruptor/ClockConfig.h>
#include "ResetEvent.h"


class CountdownEvent : public ResetEvent
{
	public:
		explicit CountdownEvent(std::uint32_t initialCount);

		std::uint32_t initialCount() const;
		std::uint32_t currentCount() const;

		void signal();

		void reset() override;
		void reset(std::uint32_t initialCount);

		bool wait(Disruptor::ClockConfig::Duration timeDuration = Disruptor::ClockConfig::Duration()) override;

	private:
		mutable std::atomic< std::uint32_t > m_initialCount;
		mutable std::atomic< std::uint32_t > m_currentCount;
};


