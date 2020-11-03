project(Disruptor.MatchEngine)
cmake_minimum_required(VERSION 2.6)

find_package(Boost COMPONENTS system)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

include_directories("..")

set(libutil_sources
    CountdownEvent.cpp
    ManualResetEvent.cpp
    ResetEvent.cpp
    ScopeExitFunctor.cpp
    Stopwatch.cpp
    LatencyRecorder.cpp
)

add_library(util STATIC ${libutil_sources})
