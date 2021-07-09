//
// Created by admin on 2021-07-03.
//

#include "task_runner_checker.h"
#include "message_loop_task_queues.h"

namespace wtf {

bool TaskRunnerChecker::RunsOnTheSameThread(TaskQueueId queue_a,
                                            TaskQueueId queue_b) {
    if (queue_a == queue_b) {
        return true;
    }

    auto queues = MessageLoopTaskQueues::GetInstance();
    if (queues->Owns(queue_a, queue_b)) {
        return true;
    }
    if (queues->Owns(queue_b, queue_a)) {
        return true;
    }
    return false;
}

} // namespace wtf