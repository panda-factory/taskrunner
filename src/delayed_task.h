//
// Created by admin on 2021-07-03.
//

#ifndef TEST_DELAYED_TASK_H
#define TEST_DELAYED_TASK_H

#include <queue>

#include "task.h"
#include "time_point/time_point.h"

namespace wtf {

class DelayedTask {
public:
    DelayedTask(size_t order,
                const wtf::Task& task,
                wtf::TimePoint target_time);

    DelayedTask(const DelayedTask& other) = default;

    ~DelayedTask() = default;

    const wtf::Task& GetTask() const;

    wtf::TimePoint GetTargetTime() const;

    bool operator>(const DelayedTask& other) const;

private:
    size_t order_;
    wtf::Task task_;
    wtf::TimePoint target_time_;

};

using DelayedTaskQueue = std::priority_queue<DelayedTask,
                                             std::deque<DelayedTask>,
                                             std::greater<DelayedTask>>;

} // namespace wtf

#endif //TEST_DELAYED_TASK_H
