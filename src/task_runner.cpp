//
// Created by admin on 2021-07-02.
//

#include "task_runner.h"

#include "message_loop.h"
#include "task_runner_checker.h"
#include "logging/logging.h"

namespace wtf {

TaskRunner::TaskRunner(wtf::MessageLoopImpl* loop)
        : loop_(loop) {}

TaskRunner::~TaskRunner() = default;

TaskQueueId TaskRunner::GetTaskQueueId() {
    WTF_DCHECK(loop_);
    return loop_->GetTaskQueueId();
}

void TaskRunner::PostTask(const wtf::Task& task) {
    loop_->PostTask(task, wtf::TimePoint::Now());
}

void TaskRunner::PostTaskForTime(const wtf::Task& task,
                                 wtf::TimePoint target_time) {
    loop_->PostTask(task, target_time);
}

void TaskRunner::PostDelayedTask(const wtf::Task& task,
                                 wtf::TimeDelta delay) {
    loop_->PostTask(task, wtf::TimePoint::Now() + delay);
}

bool TaskRunner::RunsTasksOnCurrentThread() {
    if (!wtf::MessageLoop::IsInitializedForCurrentThread()) {
        return false;
    }

    const auto current_queue_id = MessageLoop::GetCurrentTaskQueueId();
    const auto loop_queue_id = loop_->GetTaskQueueId();

    return TaskRunnerChecker::RunsOnTheSameThread(current_queue_id,
                                                  loop_queue_id);
}

} // namespace wtf