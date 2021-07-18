//
// Created by guozhenxiong on 2021-07-16.
//

#include "task_queues.h"

#include "logging/logging.h"

namespace wtf {

TaskQueue::TaskQueue(TaskQueueId owner_of)
        : owner_of(owner_of)
{
    wakeable = NULL;
    task_observers = TaskObservers();
}

// | static |
TaskQueues *TaskQueues::GetInstance()
{
    static TaskQueues* instance = nullptr;
    if (!instance) {
        instance = new TaskQueues(std::make_unique<TaskQueuesImpl>());
    }
    return instance;
}

TaskQueues::TaskQueues(std::unique_ptr<TaskQueuesImpl> task_queues)
        : task_queues_impl_(std::move(task_queues))
{}

TaskQueueId TaskQueues::CreateTaskQueue()
{
    return task_queues_impl_->CreateTaskQueue();
}

std::function<void()> TaskQueues::GetNextTask(TaskQueueId queue_id,
                                              const std::chrono::steady_clock::time_point& for_time)
{
    return task_queues_impl_->GetNextTask(queue_id, for_time);
}

void TaskQueues::Dispose(TaskQueueId queue_id)
{
    task_queues_impl_->Dispose(queue_id);
}

void TaskQueues::DisposeTasks(TaskQueueId queue_id)
{
    task_queues_impl_->DisposeTasks(queue_id);
}

bool TaskQueues::HasPendingTasks(TaskQueueId queue_id) const
{
    return task_queues_impl_->HasPendingTasks(queue_id);
}

size_t TaskQueues::GetNumPendingTasks(TaskQueueId queue_id) const
{
    return task_queues_impl_->GetNumPendingTasks(queue_id);
}

void TaskQueues::RegisterTask(const TaskQueueId &queue_id,
                              const std::function<void()> &task,
                              const std::chrono::steady_clock::time_point& target_time)
{
    task_queues_impl_->RegisterTask(queue_id, task, target_time);
}

void TaskQueues::AddTaskObserver(TaskQueueId queue_id,
                                 intptr_t key,
                                 const std::function<void()> &callback)
{
    task_queues_impl_->AddTaskObserver(queue_id, key, callback);
}

void TaskQueues::RemoveTaskObserver(TaskQueueId queue_id,
                                    intptr_t key)
{
    task_queues_impl_->RemoveTaskObserver(queue_id, key);
}

std::vector<std::function<void()>> TaskQueues::GetObserversToNotify(TaskQueueId queue_id) const
{
    return task_queues_impl_->GetObserversToNotify(queue_id);
}

void TaskQueues::SetWakeable(TaskQueueId queue_id, wtf::Wakeable *wakeable)
{
    task_queues_impl_->SetWakeable(queue_id, wakeable);
}

} // namespace wtf