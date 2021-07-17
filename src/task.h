//
// Created by guozhenxiong on 2021-07-16.
//

#ifndef TASKRUNNER_TASK_H
#define TASKRUNNER_TASK_H

#include <functional>

#include "macros.h"

class Task {
public:
    Task(size_t order, const std::function<void ()>& task);

    ~Task() = default;

    const std::function<void ()>& GetTask() const;
private:
    size_t order_;
    std::function<void ()> task_;

    WTF_DISALLOW_COPY_ASSIGN_AND_MOVE(Task);
};


#endif //TASKRUNNER_TASK_H
