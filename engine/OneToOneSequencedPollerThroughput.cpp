#include "stdafx.h"
#include "OneToOneSequencedPollerThroughput.h"

#include <Disruptor/BasicExecutor.h>
#include <Disruptor/RoundRobinThreadAffinedTaskScheduler.h>
#include <Disruptor/YieldingWaitStrategy.h>

#include "ScopeExitFunctor.h"
#include "ManualResetEvent.h"


OneToOneSequencedPollerThroughput::OneToOneSequencedPollerThroughput()
{
	m_scheduler = std::make_shared< RoundRobinThreadAffinedTaskScheduler >();
	m_executor = std::make_shared< BasicExecutor >(m_scheduler);

	m_ringBuffer = RingBuffer< ValueEvent >::createSingleProducer(ValueEvent::eventFactory(), m_bufferSize, std::make_shared< YieldingWaitStrategy >());
	m_poller = m_ringBuffer->newPoller();
	m_ringBuffer->addGatingSequences({ m_poller->sequence() });
	m_pollRunnable = std::make_shared< PollRunnable >(m_poller);
}

std::int64_t OneToOneSequencedPollerThroughput::run(Stopwatch& stopwatch)
{
	m_scheduler->start(requiredProcessorCount());
	Tools::ScopeExitFunctor atScopeExit([this] { m_scheduler->stop(); });

	auto latch = std::make_shared< ManualResetEvent >(false);
	auto expectedCount = m_poller->sequence()->value() + m_iterations;
	m_pollRunnable->reset(latch, expectedCount);
	auto processorTask = m_executor->execute([this] { m_pollRunnable->run(); });
	stopwatch.start();

	auto& rb = *m_ringBuffer;
	for (auto i = 0; i < m_iterations; ++i)
	{
		auto next = rb.next();
		rb[next].value = i;
		rb.publish(next);
	}

	latch->waitOne();
	stopwatch.stop();
	waitForEventProcessorSequence(expectedCount);
	m_pollRunnable->halt();
	processorTask.wait_for(std::chrono::milliseconds(2000));

	PerfUtil::failIfNot(m_expectedResult, m_pollRunnable->value(),
			"Poll runnable should have processed " + std::to_string(m_expectedResult) + ", but was: " + std::to_string(m_pollRunnable->value()));

	return m_iterations;
}

std::int32_t OneToOneSequencedPollerThroughput::requiredProcessorCount() const
{
	return 2;
}

void OneToOneSequencedPollerThroughput::waitForEventProcessorSequence(std::int64_t expectedCount)
{
	while (m_poller->sequence()->value() != expectedCount)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

OneToOneSequencedPollerThroughput::PollRunnable::PollRunnable(const std::shared_ptr< EventPoller< ValueEvent > >& poller)
	: m_poller(poller)
{
	m_eventHandler = [this](ValueEvent& event, std::int64_t sequence, bool endOfBatch)
	{
		return this->onEvent(event, sequence, endOfBatch);
	};
}

std::int64_t OneToOneSequencedPollerThroughput::PollRunnable::value() const
{
	return m_value->value;
}

void OneToOneSequencedPollerThroughput::PollRunnable::run()
{
	try
	{
		while (m_running == 1)
		{
			if (PollState::Processing != m_poller->poll(m_eventHandler))
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(0));
			}
		}
	}
	catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}

void OneToOneSequencedPollerThroughput::PollRunnable::halt()
{
	std::atomic_exchange(&m_running, 0);
}

void OneToOneSequencedPollerThroughput::PollRunnable::reset(const std::shared_ptr< ManualResetEvent >& signal, std::int64_t expectedCount)
{
	m_value->value = 0L;
	m_signal = signal;
	m_count = expectedCount;
	m_running = 1;
}

bool OneToOneSequencedPollerThroughput::PollRunnable::onEvent(ValueEvent& event, std::int64_t sequence, bool /*endOfBatch*/)
{
	m_value->value = m_value->value + event.value;

	if (m_count == sequence)
	{
		m_signal->set();
	}

	return true;
}

