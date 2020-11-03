#pragma once

#include <map>
#include <memory>
#include <string>

#include <Disruptor/TypeInfo.h>

#include "Factory.h"

class Repository
{
	public:
		static const Repository& instance();

		std::vector< ThroughputInfo > allThrougputs() const;
		bool tryGetThroughput(const std::string& testName, ThroughputInfo& testInfo) const;

		std::vector< LatencyInfo > allLatencys() const;
		bool tryGetLatency(const std::string& testName, LatencyInfo& testInfo) const;

	private:
		Repository();

		template <class T>
		void registerJob()
		{
			static_assert(std::is_base_of< IThroughput, T >::value || std::is_base_of< ILatency, T >::value,
					"T should implement IThroughput or ILatency");

			std::function<std::shared_ptr<T>()>&& factory = []() -> std::shared_ptr<T>
			{
				return std::make_shared<T>();
			};

			registerJob(Disruptor::Utils::getMetaTypeInfo< T >(), factory);
		}

		void registerJob(const Disruptor::TypeInfo& typeInfo, const std::function<std::shared_ptr< IThroughput >()>& testFactory);
		void registerJob(const Disruptor::TypeInfo& typeInfo, const std::function<std::shared_ptr< ILatency >()>& testFactory); 
		std::map< std::string, ThroughputInfo > m_throughputInfosByName;
		std::map< std::string, LatencyInfo > m_latencyInfosByName;
};
