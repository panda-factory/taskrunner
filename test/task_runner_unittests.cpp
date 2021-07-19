//
// Created by guozhenxiong on 2021-07-16.
//

#include "task_runner/task_runner.h"
#include "gtest/gtest.h"

TEST(TaskRunner, CreateAndTerminate)
{
    auto task_runner = wtf::TaskRunner::Create("aaa");
    ASSERT_NE(task_runner.get(), nullptr);
}

TEST(TaskRunner, CanRunAndTerminate)
{
    bool started = false;
    auto task_runner = wtf::TaskRunner::Create();
    ASSERT_NE(task_runner.get(), nullptr);

    task_runner->PostTask([&started]() {
        started = true;
    });
    task_runner->Terminate();
    task_runner->Join();
    ASSERT_TRUE(started);
}

TEST(TaskRunner, TasksAreRunInOrder)
{
    const size_t count = 100;
    size_t current = 0;
    auto task_runner = wtf::TaskRunner::Create();

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

TEST(TaskRunner, TasksAreRunInOrderUsedTaskRunnerPtr)
{
    const size_t count = 100;
    size_t current = 0;
    auto task_runner = wtf::TaskRunner::Create();

    for (size_t i = 0; i < count; i++) {
        PostTask(task_runner.get(), current, i);
    }
    task_runner->Terminate();
    task_runner->Join();
    ASSERT_EQ(current, count);
}