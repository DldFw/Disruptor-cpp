#pragma once

#include <memory>

#include <Disruptor/IEventHandler.h>

#include "ManualResetEvent.h"

#include "PaddedLong.h"
#include "ValueEvent.h"

class ValueAdditionEventHandler : public Disruptor::IEventHandler< ValueEvent >
{
	public:
		void reset(const std::shared_ptr< ManualResetEvent >& latch, std::int64_t expectedCount);

		void onEvent(ValueEvent& value, std::int64_t sequence, bool /*endOfBatch*/) override;

		std::int64_t count() const;

		std::int64_t value() const;

	private:
		PaddedLong m_value;
		std::int64_t m_count = 0;
		std::shared_ptr< ManualResetEvent > m_latch;
};

