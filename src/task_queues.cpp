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
    if (!HasPendingTasks(queue_id)) {
        return nullptr;
    }

    size_t order = order_++;
    const auto &queue_entry = queue_entries_.at(queue_id);

    auto task = queue_entry->task_queue.front();
    queue_entry->task_queue.pop();

    return task;
}

void TaskQueues::Dispose(TaskQueueId queue_id)
{
    std::scoped_lock locker(queue_mutex_);
    const auto &queue_entry = queue_entries_.at(queue_id);
    queue_entries_.erase(queue_id);
}

void TaskQueues::DisposeTasks(TaskQueueId queue_id)
{
    std::lock_guard guard(queue_mutex_);
    const auto &queue_entry = queue_entries_.at(queue_id);
    queue_entry->task_queue = {};
}

std::vector<std::function<void ()>> TaskQueues::GetObserversToNotify(TaskQueueId queue_id) const
{
    std::lock_guard guard(queue_mutex_);
    std::vector<std::function<void ()>> observers;

    for (const auto &observer : queue_entries_.at(queue_id)->task_observers) {
        observers.push_back(observer.second);
    }

    return observers;
}

bool TaskQueues::HasPendingTasks(TaskQueueId queue_id) const
{
    const auto &entry = queue_entries_.at(queue_id);

    return !entry->task_queue.empty();
}

size_t TaskQueues::GetNumPendingTasks(TaskQueueId queue_id) const {
    std::lock_guard guard(queue_mutex_);
    const auto& queue_entry = queue_entries_.at(queue_id);

    size_t total_tasks = 0;
    total_tasks += queue_entry->task_queue.size();

    return total_tasks;
}

void TaskQueues::RegisterTask(TaskQueueId queue_id,
                              const std::function<void ()>& task)
{
    std::lock_guard guard(queue_mutex_);
    size_t order = order_++;
    const auto &queue_entry = queue_entries_.at(queue_id);
    queue_entry->task_queue.push(task);

    if (HasPendingTasks(queue_id)) {
        WakeUpUnlocked(queue_id, std::chrono::steady_clock::time_point::max());
    }
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

void TaskQueues::SetWakeable(TaskQueueId queue_id, wtf::Wakeable *wakeable)
{
    std::lock_guard guard(queue_mutex_);
    WTF_CHECK(!queue_entries_.at(queue_id)->wakeable)
        << "Wakeable can only be set once.";
    queue_entries_.at(queue_id)->wakeable = wakeable;
}

void TaskQueues::WakeUpUnlocked(TaskQueueId queue_id,
                                           const std::chrono::steady_clock::time_point& time) const
{
    if (queue_entries_.at(queue_id)->wakeable) {
        queue_entries_.at(queue_id)->wakeable->WakeUp(time);
    }
}

} // namespace wtf