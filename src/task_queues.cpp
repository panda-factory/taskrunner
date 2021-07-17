//
// Created by guozhenxiong on 2021-07-16.
//

#include "task_queues.h"

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

void TaskQueues::AddTaskObserver(TaskQueueId queue_id,
                                 intptr_t key,
                                 const std::function<void ()> &callback)
{
    std::lock_guard guard(queue_mutex_);
    WTF_DCHECK(callback != nullptr) << "Observer callback must be non-null.";
    queue_entries_.at(queue_id)->task_observers[key] = callback;
}

void TaskQueues::RemoveTaskObserver(TaskQueueId queue_id,
                                               intptr_t key) {
    std::lock_guard guard(queue_mutex_);
    queue_entries_.at(queue_id)->task_observers.erase(key);
}

} // namespace wtf