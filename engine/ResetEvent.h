#pragma once

#include <mutex>
#include <condition_variable>

#include <Disruptor/ClockConfig.h>

class ResetEvent
{
	public:
		explicit ResetEvent(bool signaled = false);
		virtual ~ResetEvent() = default;

		ResetEvent(const ResetEvent&) = delete;

	protected:
		virtual bool isSet() const;

		virtual void set(bool ensureWaitIsTriggered = false);
		virtual void reset();

		virtual bool wait(Disruptor::ClockConfig::Duration timeDuration);

	private:
		bool internalWait(Disruptor::ClockConfig::Duration timeDuration);

		bool m_signaled;
		mutable std::mutex m_mutex;
		std::condition_variable m_conditionVariable;
		int m_waitCount;
};

