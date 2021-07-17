//
// Created by guozhenxiong on 2021-07-16.
//

#include "task.h"

Task::Task(size_t order, const std::function<void ()>& task)
        : order_(order),
          task_(task)
{
}