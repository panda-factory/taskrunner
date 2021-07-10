//
// Created by guozhenxiong on 2021-07-03.
//

#ifndef TASKRUNNER_DELAYED_TASK_H
#define TASKRUNNER_DELAYED_TASK_H

#include <queue>
#include <chrono>

#include "task_runner/task.h"

namespace wtf {

class DelayedTask {
public:
    DelayedTask(size_t order,
                const wtf::Task& task,
                const std::chrono::steady_clock::time_point& target_time);

    DelayedTask(const DelayedTask& other) = default;

    ~DelayedTask() = default;

    const wtf::Task& GetTask() const;

    std::chrono::steady_clock::time_point GetTargetTime() const;

    bool operator>(const DelayedTask& other) const;

private:
    size_t order_;
    wtf::Task task_;
    std::chrono::steady_clock::time_point target_time_;

};

using DelayedTaskQueue = std::priority_queue<DelayedTask,
                                             std::deque<DelayedTask>,
                                             std::greater<DelayedTask>>;

} // namespace wtf

#endif //TASKRUNNER_DELAYED_TASK_H
