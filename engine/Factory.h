#pragma once

#include <string>

#include "ILatency.h"
#include "IThroughput.h"

template <class T>
struct Factory
{
	std::string name;
	std::function<std::shared_ptr< T >()> factory;
};


using ThroughputInfo = Factory< IThroughput >;
using LatencyInfo = Factory< ILatency >;


