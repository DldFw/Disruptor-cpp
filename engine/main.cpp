#include "stdafx.h"

#include <boost/algorithm/string.hpp>

#include "LatencySession.h"
#include "Repository.h"
#include "ThroughputSession.h"

void runAlls(const Repository& testRepository);
void runOne(const Repository& testRepository, const std::string& testName);

int main(int, char**)
{
    auto& testRepository = Repository::instance();

    std::string testName;

    std::cout << " name (ALL by default):  " << testName << " ?" << std::endl;

    std::getline(std::cin, testName);

    boost::algorithm::trim(testName);

    if (boost::algorithm::iequals(testName, "ALL") || testName.empty())
    {
        runAlls(testRepository);
    }
    else
    {
        runOne(testRepository, testName);
    }

    return 0;
}

void runAlls(const Repository& testRepository)
{
    for (auto&& info : testRepository.allLatencys())
    {
        LatencySession session(info);
        session.run();
    }

    for (auto&& info : testRepository.allThrougputs())
    {
        ThroughputSession session(info);
        session.run();
    }
}

void runOne(const Repository& testRepository, const std::string& testName)
{
    LatencyInfo latencyInfo;
    if (testRepository.tryGetLatency(testName, latencyInfo))
    {
        LatencySession session(latencyInfo);
        session.run();
    }
    else
    {
        ThroughputInfo throughputInfo;
        if (testRepository.tryGetThroughput(testName, throughputInfo))
        {
            ThroughputSession session(throughputInfo);
            session.run();
        }
    }
}
