//
// Created by admin on 2021-07-03.
//

#ifndef TEST_TASK_RUNNER_CHECKER_H
#define TEST_TASK_RUNNER_CHECKER_H

#include "task_queue_id.h"

namespace wtf {

class TaskRunnerChecker final {
public:
    TaskRunnerChecker();

    ~TaskRunnerChecker();

    bool RunsOnCreationTaskRunner() const;

    static bool RunsOnTheSameThread(TaskQueueId queue_a, TaskQueueId queue_b);

private:
    TaskQueueId initialized_queue_id_;
    TaskQueueId subsumed_queue_id_;

    TaskQueueId InitTaskQueueId();

};
} // namespace wtf

#endif //TEST_TASK_RUNNER_CHECKER_H
