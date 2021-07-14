

SET(TASK_RUNNER_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/concurrent_message_loop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/concurrent_task_runner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/delayed_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/delayed_message_loop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/delayed_message_loop_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/message_loop_task_queues.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/task_runner.cpp
    )

IF(WIN32)
    SET(TASK_RUNNER_SRC ${TASK_RUNNER_SRC}
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/win/delayed_message_loop_win.cpp)
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