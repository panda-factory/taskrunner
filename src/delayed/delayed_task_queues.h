//
// Created by guozhenxiong on 2021-07-18.
//

#ifndef TASKRUNNER_DELAYED_TASK_QUEUES_H
#define TASKRUNNER_DELAYED_TASK_QUEUES_H

#include "task_queues.h"

namespace wtf {
class DelayedTaskQueuesImpl;

class DelayedTaskQueues final : public TaskQueues {
public:
    static TaskQueues *GetInstance();

    DelayedTaskQueues(std::unique_ptr<DelayedTaskQueuesImpl> task_queues);
};
} // namespace wtf

#endif //TASKRUNNER_DELAYED_TASK_QUEUES_H
