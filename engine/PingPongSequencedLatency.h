#pragma once

#include <Disruptor/BatchEventProcessor.h>
#include <Disruptor/IEventHandler.h>
#include <Disruptor/IExecutor.h>
#include <Disruptor/RingBuffer.h>
#include <Disruptor/ILifecycleAware.h>
#include <Disruptor/ITaskScheduler.h>

#include "CountdownEvent.h"
#include "ManualResetEvent.h"
#include "Stopwatch.h"

#include "ILatency.h"
#include "ValueEvent.h"

class PingPongSequencedLatency : public ILatency
{
	class Pinger;
	class Ponger;

	public:
	PingPongSequencedLatency();

	std::int32_t requiredProcessorCount() const override;

	void run(Stopwatch& stopwatch, const std::shared_ptr< LatencyRecorder >& latencyRecorder) override;

	private:
	const std::int32_t m_bufferSize = 1024;

#ifdef _DEBUG
	const std::int64_t m_iterations = 10L * 100L * 3L;
#else
	const std::int64_t m_iterations = 100L * 1000L * 30L;
#endif

	const std::int32_t m_pauseDurationInNanos = 1000;

	std::shared_ptr< Disruptor::RingBuffer< ValueEvent > > m_pingBuffer;
	std::shared_ptr< Disruptor::RingBuffer< ValueEvent > > m_pongBuffer;

	std::shared_ptr< Disruptor::ISequenceBarrier > m_pongBarrier;
	std::shared_ptr< Pinger > m_pinger;
	std::shared_ptr< Disruptor::BatchEventProcessor< ValueEvent > > m_pingProcessor;

	std::shared_ptr< Disruptor::ISequenceBarrier > m_pingBarrier;
	std::shared_ptr< Ponger > m_ponger;
	std::shared_ptr< Disruptor::BatchEventProcessor< ValueEvent > > m_pongProcessor;

	std::shared_ptr< Disruptor::IExecutor > m_executor;
	std::shared_ptr< Disruptor::ITaskScheduler > m_taskScheduler;
};


class PingPongSequencedLatency::Pinger : public Disruptor::IEventHandler< ValueEvent >, public Disruptor::ILifecycleAware
{
	public:
		Pinger(const std::shared_ptr< Disruptor::RingBuffer< ValueEvent > >& buffer, std::int64_t maxEvents, std::int32_t pauseDurationInNanos);

		void onEvent(ValueEvent& data, std::int64_t sequence, bool endOfBatch) override;

		void onStart() override;
		void onShutdown() override;

		void reset(const std::shared_ptr< CountdownEvent >& globalSignal,
				const std::shared_ptr< ManualResetEvent >& signal,
				const std::shared_ptr< LatencyRecorder >& latencyRecorder);

	private:
		void send();

		std::shared_ptr< Disruptor::RingBuffer< ValueEvent > > m_buffer;
		std::int64_t m_maxEvents;
		std::int32_t m_pauseDurationInNanos;
		std::int64_t m_pauseDurationInStopwatchTicks;
		std::shared_ptr< LatencyRecorder > m_latencyRecorder;
		std::int64_t m_t0;
		std::int64_t m_counter;
		std::shared_ptr< CountdownEvent > m_globalSignal;
		std::shared_ptr< ManualResetEvent > m_signal;
};


class PingPongSequencedLatency::Ponger : public Disruptor::IEventHandler< ValueEvent >, public Disruptor::ILifecycleAware
{
	public:
		explicit Ponger(const std::shared_ptr< Disruptor::RingBuffer< ValueEvent > >& buffer);

		void onEvent(ValueEvent& data, std::int64_t sequence, bool endOfBatch) override;

		void onStart() override;
		void onShutdown() override;

		void reset(const std::shared_ptr< CountdownEvent >& globalSignal);

	private:
		std::shared_ptr< Disruptor::RingBuffer< ValueEvent > > m_buffer;
		std::shared_ptr< CountdownEvent > m_globalSignal;
};



