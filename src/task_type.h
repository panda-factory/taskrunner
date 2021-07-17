//
// Created by guozhenxiong on 2021-07-16.
//

#ifndef TASKRUNNER_TASK_TYPE_H
#define TASKRUNNER_TASK_TYPE_H

#include <functional>
#include <queue>
#include <chrono>

#include "macros.h"

namespace wtf {
class Task {
public:
    Task(size_t order, const std::function<void()> &task);

    ~Task() = default;

    const std::function<void()> &GetTask() const;

private:
    size_t order_;
    std::function<void()> task_;

    WTF_DISALLOW_COPY_ASSIGN_AND_MOVE(Task);
};

class DelayedTask {
public:
    DelayedTask(size_t order,
                const std::function<void()> &task,
                const std::chrono::steady_clock::time_point &target_time);

    DelayedTask(const DelayedTask &other) = default;

    ~DelayedTask() = default;

    const std::function<void()> &GetTask() const;

    std::chrono::steady_clock::time_point GetTargetTime() const;

    bool operator>(const DelayedTask &other) const;

private:
    size_t order_;
    std::function<void()> task_;
    std::chrono::steady_clock::time_point target_time_;

};

using DelayedTaskQueue = std::priority_queue<DelayedTask,
        std::deque<DelayedTask>,
        std::greater<DelayedTask>>;
} // namespace wtf
#endif //TASKRUNNER_TASK_TYPE_H
