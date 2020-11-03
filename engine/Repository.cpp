#include "stdafx.h"
#include "Repository.h"

#include <boost/algorithm/string.hpp>

// Raw
#include "OneToOneRawBatchThroughput.h"
//#include "OneToOneRawThroughput.h"

// Sequenced
/*#include "OneToOneSequencedBatchThroughput.h"
#include "OneToOneSequencedLongArrayThroughput.h"
#include "OneToOneSequencedPollerThroughput.h"
#include "OneToOneSequencedThroughput.h"
#include "OneToThreeDiamondSequencedThroughput.h"
#include "OneToThreePipelineSequencedThroughput.h"
#include "OneToThreeSequencedThroughput.h"
#include "PingPongSequencedLatency.h"
#include "ThreeToOneSequencedBatchThroughput.h"
#include "ThreeToOneSequencedThroughput.h"
#include "ThreeToThreeSequencedThroughput.h"

// Translator
#include "OneToOneTranslatorThroughput.h"

// WorkHandler
#include "OneToThreeReleasingWorkerPoolThroughput.h"
#include "OneToThreeWorkerPoolThroughput.h"
#include "TwoToTwoWorkProcessorThroughput.h"
*/
Repository::Repository()
{
	// Raw
	registerJob< OneToOneRawBatchThroughput >();
/*	registerJob< OneToOneRawThroughput >();

	// Sequenced
	registerJob< OneToOneSequencedBatchThroughput >();
	registerJob< OneToOneSequencedLongArrayThroughput >();
	registerJob< OneToOneSequencedPollerThroughput >();
	registerJob< OneToOneSequencedThroughput >();
	registerJob< OneToThreeDiamondSequencedThroughput >();
	registerJob< OneToThreePipelineSequencedThroughput >();
	registerJob< OneToThreeSequencedThroughput >();
	registerJob< PingPongSequencedLatency >();
	registerJob< ThreeToOneSequencedBatchThroughput >();
	registerJob< ThreeToOneSequencedThroughput >();
	registerJob< ThreeToThreeSequencedThroughput >();

	// Translator
	registerJob< OneToOneTranslatorThroughput >();

	// WorkHandler
	registerJob< OneToThreeReleasingWorkerPoolThroughput >();
	registerJob< OneToThreeWorkerPoolThroughput >();
	registerJob< TwoToTwoWorkProcessorThroughput >();*/
}

void Repository::registerJob(const Disruptor::TypeInfo& typeInfo, const std::function<std::shared_ptr< IThroughput >()>& testFactory)
{
	ThroughputInfo info{ typeInfo.name(), testFactory };

	m_throughputInfosByName.insert(std::make_pair(boost::algorithm::to_lower_copy(typeInfo.fullyQualifiedName()), info));
	m_throughputInfosByName.insert(std::make_pair(boost::algorithm::to_lower_copy(typeInfo.name()), info));
}

void Repository::registerJob(const Disruptor::TypeInfo& typeInfo, const std::function<std::shared_ptr< ILatency >()>& testFactory)
{
	LatencyInfo info{ typeInfo.name(), testFactory };

	m_latencyInfosByName.insert(std::make_pair(boost::algorithm::to_lower_copy(typeInfo.fullyQualifiedName()), info));
	m_latencyInfosByName.insert(std::make_pair(boost::algorithm::to_lower_copy(typeInfo.name()), info));
}

const Repository& Repository::instance()
{
	static Repository instance;
	return instance;
}

std::vector< ThroughputInfo > Repository::allThrougputs() const
{
	std::vector< ThroughputInfo > result;
	std::set< std::string > testNames;

	for (auto&& x : m_throughputInfosByName)
	{
		if (testNames.count(x.second.name) > 0)
			continue;

		testNames.insert(x.second.name);
		result.push_back(x.second);
	}
	return result;
}

bool Repository::tryGetThroughput(const std::string& testName, ThroughputInfo& testInfo) const
{
	auto it = m_throughputInfosByName.find(boost::algorithm::to_lower_copy(testName));
	if (it == m_throughputInfosByName.end())
		return false;

	testInfo = it->second;
	return true;
}

std::vector< LatencyInfo > Repository::allLatencys() const
{
	std::vector< LatencyInfo > result;
	std::set< std::string > testNames;

	for (auto&& x : m_latencyInfosByName)
	{
		if (testNames.count(x.second.name) > 0)
			continue;

		testNames.insert(x.second.name);
		result.push_back(x.second);
	}
	return result;
}

bool Repository::tryGetLatency(const std::string& testName, LatencyInfo& testInfo) const
{
	auto it = m_latencyInfosByName.find(boost::algorithm::to_lower_copy(testName));
	if (it == m_latencyInfosByName.end())
		return false;

	testInfo = it->second;
	return true;
}
