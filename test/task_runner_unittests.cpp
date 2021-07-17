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