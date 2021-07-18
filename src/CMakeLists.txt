

SET(TASK_RUNNER_SRC
    concurrent/concurrent_message_loop.cpp
    concurrent/concurrent_task_runner.cpp
    delayed/delayed_task_runner.cpp
    delayed/delayed_message_loop.cpp
    delayed/delayed_message_loop_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/message_loop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/message_loop_impl.cpp
    delayed/delayed_task_queues.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/task_type.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/task_queues.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/task_queues_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/task_runner.cpp
    )

IF(WIN32)
    SET(TASK_RUNNER_SRC ${TASK_RUNNER_SRC}
        delayed/platform/win/delayed_message_loop_win.cpp)
ENDIF()

IF(BUILDING_SHARED)
    ADD_LIBRARY(taskrunner SHARED ${TASK_RUNNER_SRC})
    TARGET_COMPILE_DEFINITIONS(taskrunner PRIVATE BUILDING_SHARED=1)
ELSE()
    ADD_LIBRARY(taskrunner STATIC ${TASK_RUNNER_SRC})
ENDIF()

if (WIN32)
    TARGET_COMPILE_DEFINITIONS(taskrunner PRIVATE OS_WIN=1)
endif()

TARGET_LINK_LIBRARIES(taskrunner PRIVATE logging)
TARGET_INCLUDE_DIRECTORIES(taskrunner PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
TARGET_INCLUDE_DIRECTORIES(taskrunner PRIVATE ${PROJECT_ROOT_PATH}/third-party/logging/include)