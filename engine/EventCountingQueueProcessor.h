#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>

#include <Disruptor/BlockingQueue.h>

#include "PaddedLong.h"



class EventCountingQueueProcessor
{
	public:
		EventCountingQueueProcessor(const std::shared_ptr< Disruptor::BlockingQueue< std::int64_t > >& blockingQueue,
				const std::vector< std::shared_ptr< PaddedLong > >& counters,
				std::int32_t index);

		void halt();

		void run();

	private:
		std::atomic< bool > m_running;
		std::shared_ptr<Disruptor::BlockingQueue< std::int64_t > > m_blockingQueue;
		std::vector< std::shared_ptr< PaddedLong > > m_counters;
		std::int32_t m_index;
};

