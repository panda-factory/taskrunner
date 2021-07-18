//
// Created by guozhenxiong on 2021-07-18.
//

#include "delayed_task_queues.h"
#include "delayed_message_loop_impl.h"

namespace wtf {

TaskQueues *DelayedTaskQueues::GetInstance()
{
    static DelayedTaskQueues* instance = nullptr;
    if (!instance) {
        instance = new DelayedTaskQueues(std::make_unique<DelayedTaskQueuesImpl>());
    }
    return instance;
}

DelayedTaskQueues::DelayedTaskQueues(std::unique_ptr<DelayedTaskQueuesImpl> task_queues)
    : TaskQueues(std::move(task_queues))
{}
} // namespace wtf