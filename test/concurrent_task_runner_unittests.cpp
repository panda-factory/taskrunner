//
// Created by guozhenxiong on 2021-07-14.
//

#include "task_runner/concurrent_task_runner.h"
#include "gtest/gtest.h"

TEST(ConcurrentTaskRunner, CreateOneWorkerAndTerminate)
{
    auto task_runner = wtf::ConcurrentTaskRunner::Create(1);
    ASSERT_NE(task_runner.get(), nullptr);
}

TEST(ConcurrentTaskRunner, CreateDefaultWorkerAndTerminate)
{
    auto task_runner = wtf::ConcurrentTaskRunner::Create();
    ASSERT_NE(task_runner.get(), nullptr);
}

TEST(ConcurrentTaskRunner, CanRunAndTerminate)
{
    bool started = false;
    auto task_runner = wtf::ConcurrentTaskRunner::Create(1);
    ASSERT_NE(task_runner.get(), nullptr);

    task_runner->PostTask([&started]() {
        started = true;
    });
    task_runner.reset();
    ASSERT_TRUE(started);
}

TEST(ConcurrentTaskRunner, CanRunCountWorkerAndTerminate)
{
    size_t count = 0;
    auto task_runner = wtf::ConcurrentTaskRunner::Create(10);
    ASSERT_NE(task_runner.get(), nullptr);

    task_runner->PostTaskToAllWorkers([&count]() {
        count++;
    });
    task_runner.reset();
    ASSERT_EQ(count, 10);
}

TEST(ConcurrentTaskRunner, ConcurrentTasksAreRunInOrder)
{
    const size_t count = 100;
    size_t current = 0;
    auto task_runner = wtf::ConcurrentTaskRunner::Create();

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

static void PostTask(wtf::TaskRunner *task_runner, size_t &current, size_t& i)
{
    task_runner->PostTask([i, &current]() {
        ASSERT_EQ(current, i);
        current++;
    });
}

TEST(ConcurrentTaskRunner, ConcurrentTasksAreRunInOrderUsedTaskRunnerPtr)
{
    const size_t count = 100;
    size_t current = 0;
    auto task_runner = wtf::ConcurrentTaskRunner::Create();

    for (size_t i = 0; i < count; i++) {
        PostTask(task_runner.get(), current, i);
    }
    task_runner->Terminate();
    task_runner->Join();
    ASSERT_EQ(current, count);
}
