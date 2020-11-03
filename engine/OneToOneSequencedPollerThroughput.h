#pragma once

#include <Disruptor/IExecutor.h>
#include <Disruptor/ITaskScheduler.h>
#include <Disruptor/RingBuffer.h>

#include "ManualResetEvent.h"

#include "IThroughput.h"
#include "PaddedLong.h"
#include "PerfUtil.h"
#include "ValueEvent.h"

class OneToOneSequencedPollerThroughput : public IThroughput
{
	class PollRunnable;

	public:
	OneToOneSequencedPollerThroughput();

	std::int64_t run(Stopwatch& stopwatch) override;

	std::int32_t requiredProcessorCount() const override;

	private:
	void waitForEventProcessorSequence(std::int64_t expectedCount);

	const std::int32_t m_bufferSize = 1024 * 64;

#ifdef _DEBUG
	const std::int64_t m_iterations = 100L * 100L * 10L;
#else
	const std::int64_t m_iterations = 1000L * 1000L * 100L;
#endif

	std::shared_ptr< IExecutor > m_executor;
	std::shared_ptr< ITaskScheduler > m_scheduler;

	std::shared_ptr< RingBuffer< ValueEvent > > m_ringBuffer;
	std::shared_ptr< EventPoller< ValueEvent > > m_poller;
	std::shared_ptr< PollRunnable > m_pollRunnable;
	const std::int64_t m_expectedResult = PerfUtil::accumulatedAddition(m_iterations);
};


class OneToOneSequencedPollerThroughput::PollRunnable
{
	public:
		explicit PollRunnable(const std::shared_ptr< EventPoller< ValueEvent > >& poller);

		std::int64_t value() const;

		void run();
		void halt();
		void reset(const std::shared_ptr< ManualResetEvent >& signal, std::int64_t expectedCount);

	private:
		bool onEvent(ValueEvent& event, std::int64_t sequence, bool endOfBatch);

		std::shared_ptr< EventPoller< ValueEvent > > m_poller;
		std::function< bool(ValueEvent&, std::int64_t, bool) > m_eventHandler;
		std::atomic< std::int32_t > m_running {1};
		std::shared_ptr< PaddedLong > m_value = std::make_shared< PaddedLong >();
		std::shared_ptr< ManualResetEvent > m_signal;
		std::int64_t m_count;
};

