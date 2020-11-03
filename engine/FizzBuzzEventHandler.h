#pragma once

#include <Disruptor/IEventHandler.h>

#include "ManualResetEvent.h"

#include "FizzBuzzEvent.h"
#include "FizzBuzzStep.h"
#include "PaddedLong.h"



class FizzBuzzEventHandler : public Disruptor::IEventHandler< FizzBuzzEvent >
{
	public:
		explicit FizzBuzzEventHandler(FizzBuzzStep fizzBuzzStep);

		std::int64_t fizzBuzzCounter() const;

		void reset(const std::shared_ptr< ManualResetEvent >& latch, std::int64_t iterations);

		void onEvent(FizzBuzzEvent& data, std::int64_t sequence, bool /*endOfBatch*/) override;

	private:
		FizzBuzzStep m_fizzBuzzStep;
		std::int64_t m_iterations = 0;
		std::shared_ptr< ManualResetEvent > m_latch;
		PaddedLong m_fizzBuzzCounter;
};

