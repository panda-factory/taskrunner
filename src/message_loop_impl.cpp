//
// Created by guozhenxiong on 2021-07-16.
//

#include "message_loop_impl.h"
#include "message_loop.h"
#include "logging/logging.h"

namespace wtf {

TaskQueues *TaskQueues::GetInstance()
{    static TaskQueues *instance = nullptr;
    if (!instance) {
        instance = new TaskQueues();
    }
    return instance;
}

TaskQueueId TaskQueues::CreateTaskQueue()
{
    std::scoped_lock locker(queue_mutex_);
    TaskQueueId loop_id = TaskQueueId(task_queue_id_counter_);
    ++task_queue_id_counter_;
    queue_entries_[loop_id] = std::make_unique<TaskQueue>(loop_id);
    return loop_id;
}

std::function<void ()> TaskQueues::GetNextTask(TaskQueueId queue_id)
{
    std::lock_guard guard(queue_mutex_);
    size_t order = order_++;
    const auto &queue_entry = queue_entries_.at(queue_id);

    auto task = queue_entry->task_queue.front();
    queue_entry->task_queue.pop();

    return task;
}

void TaskQueues::RegisterTask(TaskQueueId queue_id,
                  const std::function<void ()>& task)
{
    std::lock_guard guard(queue_mutex_);
    size_t order = order_++;
    const auto &queue_entry = queue_entries_.at(queue_id);
    queue_entry->task_queue.push(task);
}


MessageLoopImpl::MessageLoopImpl() :
        task_queues_(TaskQueues::GetInstance()),
        queue_id_(task_queues_->CreateTaskQueue()),
        terminated_(false)
{
}

MessageLoopImpl::~MessageLoopImpl()
{

}

void MessageLoopImpl::AddTaskObserver(intptr_t key,
                                      const std::function<void ()> &callback)
{
    WTF_DCHECK(callback != nullptr);
    WTF_DCHECK(MessageLoop::GetCurrent().GetLoopImpl() == this)
    << "Message loop task observer must be added on the same thread as the "
       "loop.";
    if (callback != nullptr) {
        task_queues_->AddTaskObserver(queue_id_, key, callback);
    } else {
        WTF_LOG(ERROR) << "Tried to add a null TaskObserver.";
    }
}

void MessageLoopImpl::RemoveTaskObserver(intptr_t key) {
    WTF_DCHECK(MessageLoop::GetCurrent().GetLoopImpl() == this)
    << "Message loop task observer must be removed from the same thread as "
       "the loop.";
    task_queues_->RemoveTaskObserver(queue_id_, key);
}

void MessageLoopImpl::PostTask(const std::function<void ()>& task,
                               const std::chrono::steady_clock::time_point&)
{
    WTF_DCHECK(task != nullptr);
    if (terminated_) {
        // If the message loop has already been terminated, PostTask should destruct
        // |task| synchronously within this function.
        return;
    }
    task_queues_->RegisterTask(queue_id_, task);

    task_condition_.notify_one();
}

void MessageLoopImpl::DoRun()
{
    if (terminated_) {
        return;
    }

    Run();
    terminated_ = true;
}

void MessageLoopImpl::Terminate()
{
}

void MessageLoopImpl::Run()
{
    while (!terminated_) {
        std::unique_lock lock(task_mutex_);
        task_condition_.wait(lock);

        std::function<void ()> task = task_queues_->GetNextTask(queue_id_);

        task();
    }
}

void MessageLoopImpl::DoTerminate()
{
    terminated_ = true;
    Terminate();
}
} // namespace wtf