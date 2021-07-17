//
// Created by guozhenxiong on 2021-07-16.
//

#include "task_type.h"

namespace wtf {
Task::Task(size_t order, const std::function<void ()>& task)
        : order_(order),
          task_(task)
{
}

DelayedTask::DelayedTask(size_t order,
                         const std::function<void ()>& task,
                         const std::chrono::steady_clock::time_point& target_time)
        : order_(order),
          task_(task),
          target_time_(target_time) {}

const std::function<void ()>& DelayedTask::GetTask() const {
    return task_;
}

std::chrono::steady_clock::time_point DelayedTask::GetTargetTime() const {
    return target_time_;
}

bool DelayedTask::operator>(const DelayedTask& other) const {
    if (target_time_ == other.target_time_) {
        return order_ > other.order_;
    }
    return target_time_ > other.target_time_;
}
} // namespace wtf