
SET(GTEST_ROOT ${PROJECT_SOURCE_DIR}/third-party/googletest/googletest)

FIND_PACKAGE(GTestSrc REQUIRED)

IF(GTESTSRC_FOUND)
    enable_testing()

    IF (WIN32 AND (NOT CYGWIN) AND (NOT MINGW))
        SET(gtest_disable_pthreads ON)
        SET(gtest_force_shared_crt ON)
    ENDIF()


    ADD_SUBDIRECTORY(${GTEST_SOURCE_DIR} ${CMAKE_BINARY_DIR}/googletest)
    SET(GTEST_LIBRARIES gtest gtest_main)

    INCLUDE_DIRECTORIES(${PROJECT_ROOT_PATH}/include)
    INCLUDE_DIRECTORIES(${PROJECT_ROOT_PATH}/third-party/timepoint/include)
    ADD_EXECUTABLE(unittests
        task_runner_unittests.cpp)
    TARGET_LINK_LIBRARIES(unittests PRIVATE ${GTEST_LIBRARIES} taskrunner)
ENDIF(GTESTSRC_FOUND)
