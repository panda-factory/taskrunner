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

TEST(TaskRunner, NonDelayedTasksAreRunInOrder)
{
    const size_t count = 100;
    size_t current = 0;
    auto task_runner = wtf::TaskRunner::CreateTaskRunner("bbb");

    for (size_t i = 0; i < count; i++) {
        task_runner->PostTask([i, &current]() {
            ASSERT_EQ(current, i);
            current++;
        });
    }
    task_runner->Join();
    ASSERT_EQ(current, count);
}
