#include "stdafx.h"
#include "OneToThreeWorkerPoolThroughput.h"

#include<Disruptor/BasicExecutor.h>
#include<Disruptor/FatalExceptionHandler.h>
#include<Disruptor/RoundRobinThreadAffinedTaskScheduler.h>

#include "ScopeExitFunctor.h"

#include "Util.h"

OneToThreeWorkerPoolThroughput::OneToThreeWorkerPoolThroughput()
{
	for (auto i = 0; i < m_numWorkers; ++i)
	{
		m_counters[i] = std::make_shared< PaddedLong >();
	}

	for (auto i = 0; i < m_numWorkers; ++i)
	{
		m_queueWorkers[i] = std::make_shared< EventCountingQueueProcessor >(m_blockingQueue, m_counters, i);
	}

	for (auto i = 0; i < m_numWorkers; ++i)
	{
		m_handlers[i] = std::make_shared< EventCountingWorkHandler >(m_counters, i);
	}

	m_workerPool = std::make_shared< WorkerPool< ValueEvent > >(m_ringBuffer, m_ringBuffer->newBarrier(), std::make_shared< FatalExceptionHandler< ValueEvent > >(), m_handlers);

	m_ringBuffer->addGatingSequences({ m_workerPool->getWorkerSequences() });
}

std::int64_t OneToThreeWorkerPoolThroughput::run(Stopwatch& stopwatch)
{
	auto scheduler = std::make_shared< RoundRobinThreadAffinedTaskScheduler >();
	scheduler->start(requiredProcessorCount());
	ToolScopeExitFunctor atScopeExit([scheduler] { scheduler->stop(); });

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

std::int32_t OneToThreeWorkerPoolThroughput::requiredProcessorCount() const
{
	return 4;
}

void OneToThreeWorkerPoolThroughput::resetCounters()
{
	for (auto i = 0; i < m_numWorkers; ++i)
	{
		m_counters[i]->value = 0L;
	}
}

std::int64_t OneToThreeWorkerPoolThroughput::sumCounters()
{
	std::int64_t sumJobs = 0L;
	for (auto i = 0; i < m_numWorkers; ++i)
	{
		sumJobs += m_counters[i]->value;
	}

	return sumJobs;
}

