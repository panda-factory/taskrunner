
SET(BENCHMARK_ROOT ${PROJECT_SOURCE_DIR}/third-party/benchmark)

FIND_PACKAGE(Benchmark REQUIRED)
IF(BENCHMARK_FOUND)
    SET(BENCHMARK_ENABLE_GTEST_TESTS 0)
    SET(BENCHMARK_LIBRARIES benchmark benchmark_main)
    ADD_SUBDIRECTORY(${BENCHMARK_SOURCE_DIR} ${CMAKE_BINARY_DIR}/third-party/benchmark)


    ADD_EXECUTABLE(task_runner_benchmark
        task_runner_benchmark.cpp)
    TARGET_INCLUDE_DIRECTORIES(task_runner_benchmark PRIVATE ${BENCHMARK_INCLUDE_DIR})
    TARGET_LINK_LIBRARIES(task_runner_benchmark PRIVATE ${BENCHMARK_LIBRARIES} taskrunner)
ENDIF()


