//
// Created by guozhenxiong on 2021-07-18.
//

#include "task_queues_impl.h"
#include "task_queues.h"

#include "logging/logging.h"

namespace wtf {

TaskQueuesImpl::TaskQueuesImpl()
        : task_queue_id_counter_(0), order_(0)
{}

TaskQueueId TaskQueuesImpl::CreateTaskQueue()
{
    std::scoped_lock locker(queue_mutex_);
    TaskQueueId loop_id = TaskQueueId(task_queue_id_counter_);
    ++task_queue_id_counter_;
    queue_entries_[loop_id] = std::make_unique<TaskQueue>(loop_id);
    return loop_id;
}

std::function<void()> TaskQueuesImpl::GetNextTask(TaskQueueId queue_id)
{
    std::scoped_lock locker(queue_mutex_);
    if (!HasPendingTasks(queue_id)) {
        return nullptr;
    }

    const auto &queue_entry = queue_entries_.at(queue_id);

    auto task = queue_entry->task_queue.front();
    queue_entry->task_queue.pop();

    return task.GetTask();
}

void TaskQueuesImpl::Dispose(TaskQueueId queue_id)
{
    std::scoped_lock locker(queue_mutex_);
    const auto &queue_entry = queue_entries_.at(queue_id);
    queue_entries_.erase(queue_id);
}

void TaskQueuesImpl::DisposeTasks(TaskQueueId queue_id)
{
    std::scoped_lock locker(queue_mutex_);
    const auto &queue_entry = queue_entries_.at(queue_id);
    queue_entry->task_queue = {};
}

std::vector<std::function<void()>> TaskQueuesImpl::GetObserversToNotify(TaskQueueId queue_id) const
{
    std::scoped_lock locker(queue_mutex_);
    std::vector<std::function<void()>> observers;

    for (const auto &observer : queue_entries_.at(queue_id)->task_observers) {
        observers.push_back(observer.second);
    }

    return observers;
}

bool TaskQueuesImpl::HasPendingTasks(TaskQueueId queue_id) const
{
    if (queue_entries_.count(queue_id) != 0) {
        const auto &entry = queue_entries_.at(queue_id);
        return !entry->task_queue.empty();
    }

    return false;
}

size_t TaskQueuesImpl::GetNumPendingTasks(TaskQueueId queue_id) const
{
    std::scoped_lock locker(queue_mutex_);
    size_t total_tasks = 0;
    if (queue_entries_.count(queue_id) != 0) {
        const auto &queue_entry = queue_entries_.at(queue_id);

        total_tasks += queue_entry->task_queue.size();
    }

    return total_tasks;
}

void TaskQueuesImpl::RegisterTask(TaskQueueId queue_id,
                                  const std::function<void()> &task)
{
    std::scoped_lock locker(queue_mutex_);
    size_t order = order_++;
    const auto &queue_entry = queue_entries_.at(queue_id);
    queue_entry->task_queue.push({order, task});

    if (HasPendingTasks(queue_id)) {
        WakeUpUnlocked(queue_id, std::chrono::steady_clock::time_point::max());
    }
}

void TaskQueuesImpl::AddTaskObserver(TaskQueueId queue_id,
                                     intptr_t key,
                                     const std::function<void()> &callback)
{
    std::scoped_lock locker(queue_mutex_);
    WTF_DCHECK(callback != nullptr) << "Observer callback must be non-null.";
    queue_entries_.at(queue_id)->task_observers[key] = callback;
}

void TaskQueuesImpl::RemoveTaskObserver(TaskQueueId queue_id,
                                        intptr_t key)
{
    std::scoped_lock locker(queue_mutex_);
    queue_entries_.at(queue_id)->task_observers.erase(key);
}

void TaskQueuesImpl::SetWakeable(TaskQueueId queue_id, wtf::Wakeable *wakeable)
{
    std::scoped_lock locker(queue_mutex_);
    WTF_CHECK(!queue_entries_.at(queue_id)->wakeable)
    << "Wakeable can only be set once.";
    queue_entries_.at(queue_id)->wakeable = wakeable;
}

void TaskQueuesImpl::WakeUpUnlocked(TaskQueueId queue_id,
                                    const std::chrono::steady_clock::time_point &time) const
{
    if (queue_entries_.at(queue_id)->wakeable) {
        queue_entries_.at(queue_id)->wakeable->WakeUp(time);
    }
}
} // namespace wtf