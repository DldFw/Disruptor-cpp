#pragma once

#include <cstdint>
#include <string>
#include <Disruptor/IEventProcessor.h>

//class Disruptor::IEventProcessor;

std::int64_t accumulatedAddition(std::int64_t iterations);

void failIf(std::int64_t a, std::int64_t b, const std::string& message = std::string());

void failIfNot(std::int64_t a, std::int64_t b, const std::string& message = std::string());

void waitForEventProcessorSequence(std::int64_t expectedCount, const std::shared_ptr< Disruptor::IEventProcessor >& batchEventProcessor);

std::string utcDateToString();

