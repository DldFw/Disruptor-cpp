#pragma once

#include <boost/thread/barrier.hpp>
#include <Disruptor/IEventHandler.h>

#include "Operation.h"
#include "PaddedLong.h"
#include "ValueEvent.h"

class ValueMutationEventHandler : public Disruptor::IEventHandler< ValueEvent >
{
	public:
		explicit ValueMutationEventHandler(Operation operation);

		std::int64_t value() const;

		void reset(const std::shared_ptr< boost::barrier >& latch, std::int64_t expectedCount);

		void onEvent(ValueEvent& data, std::int64_t sequence, bool endOfBatch);

	private:
		Operation m_operation;
		PaddedLong m_value;
		std::int64_t m_iterations = 0;
		std::shared_ptr< boost::barrier > m_latch;
};


