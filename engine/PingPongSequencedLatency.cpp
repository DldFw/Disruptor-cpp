#include "stdafx.h"
#include "PingPongSequencedLatency.h"

#include <Disruptor/BasicExecutor.h>
#include <Disruptor/RoundRobinThreadAffinedTaskScheduler.h>

#include "ScopeExitFunctor.h"
#include "LatencySession.h"

PingPongSequencedLatency::PingPongSequencedLatency()
{
	m_taskScheduler = std::make_shared< Disruptor::RoundRobinThreadAffinedTaskScheduler >();
	m_executor = std::make_shared< Disruptor::BasicExecutor >(m_taskScheduler);

	m_pingBuffer = Disruptor::RingBuffer< ValueEvent >::createSingleProducer(ValueEvent::eventFactory(), m_bufferSize, std::make_shared< Disruptor::BlockingWaitStrategy >());
	m_pongBuffer = Disruptor::RingBuffer< ValueEvent >::createSingleProducer(ValueEvent::eventFactory(), m_bufferSize, std::make_shared< Disruptor::BlockingWaitStrategy >());

	m_pingBarrier = m_pingBuffer->newBarrier();
	m_pongBarrier = m_pongBuffer->newBarrier();

	m_pinger = std::make_shared< Pinger >(m_pingBuffer, m_iterations, m_pauseDurationInNanos);
	m_ponger = std::make_shared< Ponger >(m_pongBuffer);

	m_pingProcessor = std::make_shared< Disruptor::BatchEventProcessor< ValueEvent > >(m_pongBuffer, m_pongBarrier, m_pinger);
	m_pongProcessor = std::make_shared< Disruptor::BatchEventProcessor< ValueEvent > >(m_pingBuffer, m_pingBarrier, m_ponger);

	m_pingBuffer->addGatingSequences({ m_pongProcessor->sequence() });
	m_pongBuffer->addGatingSequences({ m_pingProcessor->sequence() });
}

std::int32_t PingPongSequencedLatency::requiredProcessorCount() const
{
	return 2;
}

void PingPongSequencedLatency::run(Stopwatch& stopwatch, const std::shared_ptr< LatencyRecorder >& latencyRecorder)
{
	m_taskScheduler->start(requiredProcessorCount());
	ScopeExitFunctor atScopeExit([this] { m_taskScheduler->stop(); });

	auto globalSignal = std::make_shared< CountdownEvent >(3);
	auto signal = std::make_shared< ManualResetEvent >(false);
	m_pinger->reset(globalSignal, signal, latencyRecorder);
	m_ponger->reset(globalSignal);

	auto processorTask1 = m_executor->execute([this] { m_pongProcessor->run(); });
	auto processorTask2 = m_executor->execute([this] { m_pingProcessor->run(); });

	globalSignal->signal();
	globalSignal->wait();
	stopwatch.start();
	// running here
	signal->waitOne();
	stopwatch.stop();

	m_pingProcessor->halt();
	m_pongProcessor->halt();

	processorTask1.wait();
	processorTask2.wait();
}


PingPongSequencedLatency::Pinger::Pinger(const std::shared_ptr< Disruptor::RingBuffer< ValueEvent > >& buffer, std::int64_t maxEvents, std::int32_t pauseDurationInNanos)
	: m_buffer(buffer)
	, m_maxEvents(maxEvents)
	  , m_pauseDurationInNanos(pauseDurationInNanos)
{
	m_pauseDurationInStopwatchTicks = (std::int64_t)LatencySession::convertNanoToStopwatchTicks(pauseDurationInNanos);
}

void PingPongSequencedLatency::Pinger::onEvent(ValueEvent& data, std::int64_t /*sequence*/, bool /*endOfBatch*/)
{
	auto t1 = Stopwatch::getTimestamp();

	m_latencyRecorder->record(LatencySession::convertStopwatchTicksToNano((double)(t1 - m_t0)));

	if (data.value < m_maxEvents)
	{
		while (m_pauseDurationInNanos > (Stopwatch::getTimestamp() - t1))
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(0));
		}

		send();
	}
	else
	{
		m_signal->set();
	}
}

void PingPongSequencedLatency::Pinger::onStart()
{
	m_globalSignal->signal();
	m_globalSignal->wait();

	send();
}

void PingPongSequencedLatency::Pinger::onShutdown()
{
}

void PingPongSequencedLatency::Pinger::reset(const std::shared_ptr< CountdownEvent >& globalSignal,
		const std::shared_ptr< ManualResetEvent >& signal,
		const std::shared_ptr< LatencyRecorder >& latencyRecorder)
{
	m_latencyRecorder = latencyRecorder;
	m_globalSignal = globalSignal;
	m_signal = signal;

	m_counter = 0;
}

void PingPongSequencedLatency::Pinger::send()
{
	m_t0 = Stopwatch::getTimestamp();
	auto next = m_buffer->next();
	(*m_buffer)[next].value = m_counter;
	m_buffer->publish(next);

	++m_counter;
}


PingPongSequencedLatency::Ponger::Ponger(const std::shared_ptr< Disruptor::RingBuffer< ValueEvent > >& buffer)
	: m_buffer(buffer)
{
}

void PingPongSequencedLatency::Ponger::onEvent(ValueEvent& data, std::int64_t /*sequence*/, bool /*endOfBatch*/)
{
	auto next = m_buffer->next();
	(*m_buffer)[next].value = data.value;
	m_buffer->publish(next);
}

void PingPongSequencedLatency::Ponger::onStart()
{
	m_globalSignal->signal();
	m_globalSignal->wait();
}

void PingPongSequencedLatency::Ponger::onShutdown()
{
}

void PingPongSequencedLatency::Ponger::reset(const std::shared_ptr< CountdownEvent >& globalSignal)
{
	m_globalSignal = globalSignal;
}

