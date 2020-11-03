#include "stdafx.h"
#include "OneToThreeReleasingWorkerPoolThroughput.h"

#include <Disruptor/BasicExecutor.h>
#include <Disruptor/FatalExceptionHandler.h>
#include <Disruptor/RoundRobinThreadAffinedTaskScheduler.h>

#include "ScopeExitFunctor.h"

#include "Util.h"

OneToThreeReleasingWorkerPoolThroughput::OneToThreeReleasingWorkerPoolThroughput()
{
	for (auto i = 0; i < m_numWorkers; ++i)
	{
		m_counters[i] = std::make_shared< PaddedLong >();
	}

	for (auto i = 0; i < m_numWorkers; ++i)
	{
		m_handlers[i] = std::make_shared< EventCountingAndReleasingWorkHandler >(m_counters, i);
	}

	m_workerPool = std::make_shared< WorkerPool< ValueEvent > >(m_ringBuffer, m_ringBuffer->newBarrier(), std::make_shared< FatalExceptionHandler< ValueEvent > >(), m_handlers);

	m_ringBuffer->addGatingSequences({ m_workerPool->getWorkerSequences() });
}

std::int64_t OneToThreeReleasingWorkerPoolThroughput::run(Stopwatch& stopwatch)
{
	auto scheduler = std::make_shared< RoundRobinThreadAffinedTaskScheduler >();
	scheduler->start(requiredProcessorCount());
	Tools::ScopeExitFunctor atScopeExit([scheduler] { scheduler->stop(); });

	resetCounters();
	auto& ringBuffer = *m_workerPool->start(std::make_shared< BasicExecutor >(scheduler));
	stopwatch.start();

	for (std::int64_t i = 0; i < m_iterations; ++i)
	{
		std::int64_t sequence = ringBuffer.next();
		ringBuffer[sequence].value = i;
		ringBuffer.publish(sequence);
	}

	m_workerPool->drainAndHalt();

	// Workaround to ensure that the last worker(s) have completed after releasing their events
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	stopwatch.stop();

	PerfUtil::failIfNot(m_iterations, sumCounters());

	return m_iterations;
}

std::int32_t OneToThreeReleasingWorkerPoolThroughput::requiredProcessorCount() const
{
	return 4;
}

void OneToThreeReleasingWorkerPoolThroughput::resetCounters()
{
	for (auto i = 0; i < m_numWorkers; ++i)
	{
		m_counters[i]->value = 0L;
	}
}

std::int64_t OneToThreeReleasingWorkerPoolThroughput::sumCounters()
{
	std::int64_t sumJobs = 0L;
	for (auto i = 0; i < m_numWorkers; ++i)
	{
		sumJobs += m_counters[i]->value;
	}

	return sumJobs;
}

OneToThreeReleasingWorkerPoolThroughput::EventCountingAndReleasingWorkHandler::EventCountingAndReleasingWorkHandler(const std::vector< std::shared_ptr< PaddedLong > >& counters, std::int32_t index)
	: m_counters(counters)
	  , m_index(index)
{
}

void OneToThreeReleasingWorkerPoolThroughput::EventCountingAndReleasingWorkHandler::onEvent(ValueEvent& /*evt*/)
{
	m_eventReleaser->release();
	m_counters[m_index]->value = m_counters[m_index]->value + 1L;
}

void OneToThreeReleasingWorkerPoolThroughput::EventCountingAndReleasingWorkHandler::setEventReleaser(const std::shared_ptr< IEventReleaser >& eventReleaser)
{
	m_eventReleaser = eventReleaser;
}

