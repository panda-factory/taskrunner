//
// Created by admin on 2021-07-10.
//


#include "task_runner/task_runner.h"
#include "gtest/gtest.h"

TEST(TaskRunner, CreateAndTerminate)
{
    auto task_runner = wtf::TaskRunner::CreateTaskRunner("aaa");
    ASSERT_NE(task_runner.get(), nullptr);
}

TEST(TaskRunner, CanRunAndTerminate)
{
    bool started = false;
    auto task_runner = wtf::TaskRunner::CreateTaskRunner();
    ASSERT_NE(task_runner.get(), nullptr);

    task_runner->PostTask([&started]() {
        started = true;
    });
    task_runner->Terminate();
    task_runner->Join();
    ASSERT_TRUE(started);
}

TEST(TaskRunner, NonDelayedTasksAreRunInOrder)
{
    const size_t count = 100;
    size_t current = 0;
    auto task_runner = wtf::TaskRunner::CreateTaskRunner();

    for (size_t i = 0; i < count; i++) {
        task_runner->PostTask([i, &current]() {
            ASSERT_EQ(current, i);
            current++;
        });
    }
    task_runner->Terminate();
    task_runner->Join();
    ASSERT_EQ(current, count);
}

TEST(TaskRunner, DelayedTasksAtSameTimeAreRunInOrder)
{
    const size_t count = 100;
    size_t current = 0;
    bool terminated = false;
    auto task_runner = wtf::TaskRunner::CreateTaskRunner();

    const auto now_plus_some =
            std::chrono::steady_clock::now() + std::chrono::milliseconds(2);
    for (size_t i = 0; i < count; i++) {
        task_runner->PostTaskForTime([i, &current, task_runner = task_runner.get(), &terminated, count]() {
            ASSERT_EQ(current, i);
            current++;
            if (count == i + 1) {
                task_runner->Terminate();
                terminated = true;
            }
        }, now_plus_some);
    }

    task_runner->Join();
    ASSERT_EQ(current, count);
    ASSERT_TRUE(terminated);
}
