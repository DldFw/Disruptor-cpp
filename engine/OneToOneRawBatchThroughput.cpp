#include "stdafx.h"
#include "OneToOneRawBatchThroughput.h"

OneToOneRawBatchThroughput::OneToOneRawBatchThroughput()
{
	m_myRunnable = std::make_shared< MyRunnable >(m_sequencer);
	m_sequencer->addGatingSequences({ m_myRunnable->sequence });
}

std::int64_t OneToOneRawBatchThroughput::run(Stopwatch& stopwatch)
{
	std::int32_t batchSize = 10;
	auto latch = std::make_shared< ManualResetEvent >(false);
	auto expectedCount = m_myRunnable->sequence->value() + m_iterations * batchSize;
	m_myRunnable->reset(latch, expectedCount);
	std::thread([this] { m_myRunnable->run(); }).detach();
	stopwatch.start();

	auto sequencer = m_sequencer;

	for (std::int64_t i = 0; i < m_iterations; i++)
	{
		auto next = sequencer->next(batchSize);
		sequencer->publish(next - (batchSize - 1), next);
	}

	latch->waitOne();
	stopwatch.stop();
	waitForEventProcessorSequence(expectedCount);

	return m_iterations * batchSize;
}

std::int32_t OneToOneRawBatchThroughput::requiredProcessorCount() const
{
	return 2;
}

void OneToOneRawBatchThroughput::waitForEventProcessorSequence(std::int64_t expectedCount)
{
	while (m_myRunnable->sequence->value() != expectedCount)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

OneToOneRawBatchThroughput::MyRunnable::MyRunnable(const std::shared_ptr< Disruptor::ISequencer< boost::any > >& sequencer)
{
	m_barrier = sequencer->newBarrier({});
}

void OneToOneRawBatchThroughput::MyRunnable::reset(const std::shared_ptr< ManualResetEvent >& latch, std::int64_t expectedCount)
{
	m_latch = latch;
	m_expectedCount = expectedCount;
}

void OneToOneRawBatchThroughput::MyRunnable::run()
{
	auto expected = m_expectedCount;

	try
	{
		std::int64_t processed;
		do
		{
			processed = m_barrier->waitFor(sequence->value() + 1);
			sequence->setValue(processed);
		}
		while (processed < expected);

		m_latch->set();
		sequence->setValue(processed);
	}
	catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}
