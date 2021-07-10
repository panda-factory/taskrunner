//
// Created by guozhenxiong on 2021-07-02.
//

#include "task_runner/task_runner.h"

#include "task_runner/message_loop.h"
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

void TaskRunner::PostTask(const std::function<void ()>& task) {
    loop_->PostTask(task, std::chrono::steady_clock::now());
}

void TaskRunner::PostTaskForTime(const std::function<void ()>& task,
                                 const std::chrono::steady_clock::time_point& target_time) {
    loop_->PostTask(task, target_time);
}

void TaskRunner::PostDelayedTask(const std::function<void ()>& task,
                                 const std::chrono::milliseconds& delay) {
    loop_->PostTask(task, std::chrono::steady_clock::now() + delay);
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