#pragma once

#include <Disruptor/BatchEventProcessor.h>
#include <Disruptor/BusySpinWaitStrategy.h>
#include <Disruptor/RingBuffer.h>

#include "CountdownEvent.h"

#include "IThroughput.h"
#include "ValueAdditionEventHandler.h"


/**
 * Sequence a series of events from multiple publishers going to one event processor.
 * +----+
 * | P1 |------+
 * +----+      |
 *             v
 * +----+    +-----+
 * | P1 |--->| EP1 |
 * +----+    +-----+
 *             ^
 * +----+      |
 * | P3 |------+
 * +----+
 * Disruptor:
 * ==========
 *             track to prevent wrap
 *             +--------------------+
 *             |                    |
 *             |                    v
 * +----+    +====+    +====+    +-----+
 * | P1 |--->| RB |/---| SB |    | EP1 |
 * +----+    +====+    +====+    +-----+
 *             ^   get    ^         |
 * +----+      |          |         |
 * | P2 |------+          +---------+
 * +----+      |            waitFor
 *             |
 * +----+      |
 * | P3 |------+
 * +----+
 * P1  - Publisher 1
 * P2  - Publisher 2
 * P3  - Publisher 3
 * RB  - RingBuffer
 * SB  - SequenceBarrier
 * EP1 - EventProcessor 1
 * 
 */
class ThreeToOneSequencedBatchThroughput : public IThroughput
{
	class ValueBatchPublisher
	{
		public:
			ValueBatchPublisher(const std::shared_ptr< s::CountdownEvent >& cyclicBarrier,
					const std::shared_ptr< RingBuffer< ValueEvent > >& ringBuffer,
					std::int64_t iterations,
					std::int32_t batchSize);

			void run();

		private:
			std::shared_ptr< s::CountdownEvent > m_cyclicBarrier;
			std::shared_ptr< RingBuffer< ValueEvent > > m_ringBuffer;
			std::int64_t m_iterations;
			std::int32_t m_batchSize;
	};

	public:
	ThreeToOneSequencedBatchThroughput();

	std::int64_t run(Stopwatch& stopwatch) override;

	std::int32_t requiredProcessorCount() const override;

	private:
	const std::int32_t m_numPublishers = 3;
	const std::int32_t m_bufferSize = 1024 * 64;

#ifdef _DEBUG
	const std::int64_t m_iterations = 100L * 100L * 10L;
#else
	const std::int64_t m_iterations = 1000L * 1000L * 100L;
#endif

	std::shared_ptr< s::CountdownEvent > m_cyclicBarrier = std::make_shared< s::CountdownEvent >(m_numPublishers + 1);

	std::shared_ptr< RingBuffer< ValueEvent > > m_ringBuffer = RingBuffer< ValueEvent >::createMultiProducer(ValueEvent::eventFactory(), m_bufferSize, std::make_shared< BusySpinWaitStrategy >());
	std::shared_ptr< ValueAdditionEventHandler > m_handler = std::make_shared< ValueAdditionEventHandler >();
	std::shared_ptr< BatchEventProcessor< ValueEvent > > m_batchEventProcessor;
	std::vector< std::shared_ptr< ValueBatchPublisher > > m_valuePublishers = std::vector< std::shared_ptr< ValueBatchPublisher > >(m_numPublishers);
};

