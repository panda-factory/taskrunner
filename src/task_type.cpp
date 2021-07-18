//
// Created by guozhenxiong on 2021-07-16.
//

#include "task_type.h"

namespace wtf {
Task::Task(size_t order, const std::function<void()> &task)
        : order_(order),
          task_(task)
{
}

const std::function<void()> &Task::GetTask() const
{
    return task_;
}


DelayedTask::DelayedTask(size_t order,
                         const std::function<void()> &task,
                         const std::chrono::steady_clock::time_point &target_time)
        : target_time_(target_time),
          Task(order, task)
{}

std::chrono::steady_clock::time_point DelayedTask::GetTargetTime() const
{
    return target_time_;
}

bool DelayedTask::operator>(const DelayedTask &other) const
{
    if (target_time_ == other.target_time_) {
        return order_ > other.order_;
    }
    return target_time_ > other.target_time_;
}
} // namespace wtf