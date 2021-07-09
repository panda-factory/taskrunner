
SET(TASK_RUNNER_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/delayed_task.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/message_loop.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/message_loop_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/message_loop_task_queues.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/task_runner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/task_runner_checker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/thread.cpp
    )

IF(WIN32)
    SET(TASK_RUNNER_SRC ${TASK_RUNNER_SRC}
        ${CMAKE_CURRENT_SOURCE_DIR}/platform/win/message_loop_win.cpp)
ENDIF()

ADD_LIBRARY(taskrunner STATIC ${TASK_RUNNER_SRC})
TARGET_LINK_LIBRARIES(taskrunner PRIVATE logging timepoint sync)
TARGET_INCLUDE_DIRECTORIES(taskrunner PRIVATE ${PROJECT_ROOT_PATH}/src)
TARGET_INCLUDE_DIRECTORIES(taskrunner PRIVATE ${PROJECT_ROOT_PATH}/third-party/logging/include)
TARGET_INCLUDE_DIRECTORIES(taskrunner PRIVATE ${PROJECT_ROOT_PATH}/third-party/timepoint/include)
TARGET_INCLUDE_DIRECTORIES(taskrunner PRIVATE ${PROJECT_ROOT_PATH}/third-party/logging/include)