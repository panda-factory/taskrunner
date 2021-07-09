//
// Created by admin on 2021-07-03.
//

#include "delayed_task.h"
namespace wtf {

DelayedTask::DelayedTask(size_t order,
                         const wtf::Task& task,
                         wtf::TimePoint target_time)
        : order_(order),
          task_(task),
          target_time_(target_time) {}

const wtf::Task& DelayedTask::GetTask() const {
    return task_;
}

wtf::TimePoint DelayedTask::GetTargetTime() const {
    return target_time_;
}

bool DelayedTask::operator>(const DelayedTask& other) const {
    if (target_time_ == other.target_time_) {
        return order_ > other.order_;
    }
    return target_time_ > other.target_time_;
}
} // namespace wtf