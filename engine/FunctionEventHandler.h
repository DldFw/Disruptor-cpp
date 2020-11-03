#pragma once

#include <memory>

#include <Disruptor/IEventHandler.h>

#include "ManualResetEvent.h"

#include "FunctionEvent.h"
#include "FunctionStep.h"
#include "PaddedLong.h"

class FunctionEventHandler : public Disruptor::IEventHandler< FunctionEvent >
{
	public:
		explicit FunctionEventHandler(FunctionStep functionStep);

		std::int64_t stepThreeCounter() const;

		void reset(const std::shared_ptr< ManualResetEvent >& latch, std::int64_t iterations);

		void onEvent(FunctionEvent& data, std::int64_t sequence, bool endOfBatch);

	private:
		FunctionStep m_functionStep;
		PaddedLong m_counter;
		std::int64_t m_iterations{0};
		std::shared_ptr< ManualResetEvent > m_latch;
};

