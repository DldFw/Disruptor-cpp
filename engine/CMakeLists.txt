project(engine)
cmake_minimum_required(VERSION 2.6)


find_package(Boost COMPONENTS system thread date_time)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
endif()

include_directories("..")


set(engine_sources
    main.cpp
    EventCountingQueueProcessor.cpp
    EventCountingWorkHandler.cpp
    FizzBuzzEvent.cpp
    FizzBuzzEventHandler.cpp
    FunctionEvent.cpp
    FunctionEventHandler.cpp
    LatencySession.cpp
    LatencySessionResult.cpp
    LongArrayEventHandler.cpp
    MutableLong.cpp
    OneToOneRawBatchThroughput.cpp
	#   OneToOneRawThroughput.cpp
	#OneToOneSequencedBatchThroughput.cpp
	#OneToOneSequencedLongArrayThroughput.cpp
	#OneToOneSequencedPollerThroughput.cpp
	#OneToOneSequencedThroughput.cpp
	#OneToOneTranslatorThroughput.cpp
	#OneToThreeDiamondSequencedThroughput.cpp
	#OneToThreePipelineSequencedThroughput.cpp
	#OneToThreeReleasingWorkerPoolThroughput.cpp
	#OneToThreeSequencedThroughput.cpp
	#OneToThreeWorkerPoolThroughput.cpp
	PingPongSequencedLatency.cpp
    Operation.cpp
    Util.cpp
    PingPongSequencedLatency.cpp
    Repository.cpp
	#ThreeToOneSequencedBatchThroughput.cpp
	#ThreeToOneSequencedThroughput.cpp
	#ThreeToThreeSequencedThroughput.cpp
    ThroughputSession.cpp
    ThroughputSessionResult.cpp
	#TwoToTwoWorkProcessorThroughput.cpp
    ValueAdditionEventHandler.cpp
    ValueEvent.cpp
    ValueMutationEventHandler.cpp
	Stopwatch.cpp
	ScopeExitFunctor.cpp
	LatencyRecorder.cpp
	PingPongSequencedLatency.cpp
	ManualResetEvent.cpp	
	ResetEvent.cpp
	CountdownEvent.cpp
)

add_definitions(-DBOOST_TEST_DYN_LINK)

add_executable(engine ${engine_sources})
target_link_libraries(engine DisruptorStatic ${Boost_LIBRARIES} pthread)

#add_custom_target(performance_test ${CMAKE_CURRENT_BINARY_DIR}/Disruptor.Perfs)
