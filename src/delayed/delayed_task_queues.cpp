//
// Created by guozhenxiong on 2021-07-02.
//

#include "delayed_task_queues.h"

#include <memory>

#include "logging/logging.h"
#include "thread_local.h"

namespace wtf {

const size_t TaskQueueId::unmerged = ULONG_MAX;
static const TaskQueueId g_unmerged = TaskQueueId(TaskQueueId::unmerged);

TaskQueueEntry::TaskQueueEntry(TaskQueueId owner_of)
        : owner_of(owner_of)
{
    wakeable = NULL;
    task_observers = TaskObservers();
}

void TaskQueueEntry::Erase()
{
    task_queue_ = {};
}

bool TaskQueueEntry::IsEmpty()
{
    return task_queue_.empty();
}

size_t TaskQueueEntry::Size()
{
    return task_queue_.size();
}

void TaskQueueEntry::Pop()
{
    task_queue_.pop();
}

void TaskQueueEntry::Push(const DelayedTask& task)
{
    task_queue_.push(task);
}

DelayedTask TaskQueueEntry::Top()
{
    return task_queue_.top();
}

DelayedTaskQueues *DelayedTaskQueues::GetInstance()
{
    static DelayedTaskQueues *instance = nullptr;
    if (!instance) {
        instance = new DelayedTaskQueues();
    }
    return instance;
}

void DelayedTaskQueues::AddTaskObserver(TaskQueueId queue_id,
                                            intptr_t key,
                                            const std::function<void ()> &callback)
{
    std::scoped_lock locker(queue_mutex_);
    WTF_DCHECK(callback != nullptr) << "Observer callback must be non-null.";
    queue_entries_.at(queue_id)->task_observers[key] = callback;
}

void DelayedTaskQueues::RemoveTaskObserver(TaskQueueId queue_id,
                                               intptr_t key)
{
    std::scoped_lock locker(queue_mutex_);
    queue_entries_.at(queue_id)->task_observers.erase(key);
}

TaskQueueId DelayedTaskQueues::CreateTaskQueue()
{
    std::scoped_lock locker(queue_mutex_);
    TaskQueueId loop_id = TaskQueueId(task_queue_id_counter_);
    ++task_queue_id_counter_;
    queue_entries_[loop_id] = std::make_unique<TaskQueueEntry>(loop_id);
    return loop_id;
}

void DelayedTaskQueues::Dispose(TaskQueueId queue_id)
{
    std::scoped_lock locker(queue_mutex_);
    const auto &queue_entry = queue_entries_.at(queue_id);
    TaskQueueId subsumed = queue_entry->owner_of;
    queue_entries_.erase(queue_id);
    if (subsumed != g_unmerged) {
        queue_entries_.erase(subsumed);
    }
}

void DelayedTaskQueues::DisposeTasks(TaskQueueId queue_id)
{
    std::scoped_lock locker(queue_mutex_);
    const auto &queue_entry = queue_entries_.at(queue_id);
    queue_entry->Erase();
}

std::vector<std::function<void ()>> DelayedTaskQueues::GetObserversToNotify(
        TaskQueueId queue_id) const
{
    std::scoped_lock locker(queue_mutex_);
    std::vector<std::function<void ()>> observers;

    for (const auto &observer : queue_entries_.at(queue_id)->task_observers) {
        observers.push_back(observer.second);
    }

    return observers;
}

std::function<void ()> DelayedTaskQueues::GetNextTaskToRun(TaskQueueId queue_id,
                                                  const std::chrono::steady_clock::time_point& from_time)
{
    std::scoped_lock locker(queue_mutex_);
    if (!HasPendingTasks(queue_id)) {
        return nullptr;
    }
    wtf::DelayedTask top_task = PeekNextTaskUnlocked(queue_id);

    if (!HasPendingTasks(queue_id)) {
        WakeUpUnlocked(queue_id, std::chrono::steady_clock::time_point::max());
    } else {
        WakeUpUnlocked(queue_id, GetNextWakeTimeUnlocked(queue_id));
    }

    if (top_task.GetTargetTime() > from_time) {
        return nullptr;
    }
    std::function<void ()> invocation = top_task.GetTask();
    queue_entries_.at(queue_id)->Pop();
    return invocation;
}

std::chrono::steady_clock::time_point DelayedTaskQueues::GetNextWakeTimeUnlocked(
        TaskQueueId queue_id) const
{
    return PeekNextTaskUnlocked(queue_id).GetTargetTime();
}

size_t DelayedTaskQueues::GetNumPendingTasks(TaskQueueId queue_id) const
{
    std::scoped_lock locker(queue_mutex_);
    const auto& queue_entry = queue_entries_.at(queue_id);

    size_t total_tasks = 0;
    total_tasks += queue_entry->Size();

    return total_tasks;
}

bool DelayedTaskQueues::HasPendingTasks(
        TaskQueueId queue_id) const
{
    const auto &entry = queue_entries_.at(queue_id);
    bool is_owner_of = entry->owner_of == g_unmerged;
    if (is_owner_of) {
        return false;
    }

    return !entry->IsEmpty();
}

wtf::DelayedTask DelayedTaskQueues::PeekNextTaskUnlocked(
        TaskQueueId owner) const
{
    WTF_DCHECK(HasPendingTasks(owner));
    const auto &entry = queue_entries_.at(owner);

    return entry->Top();
}

void DelayedTaskQueues::RegisterTask(
        TaskQueueId queue_id,
        const std::function<void ()> &task,
        const std::chrono::steady_clock::time_point& target_time)
{
    std::scoped_lock locker(queue_mutex_);
    size_t order = order_++;
    const auto &queue_entry = queue_entries_.at(queue_id);
    queue_entry->Push({order, task, target_time});
    TaskQueueId loop_to_wake = queue_id;

    if (HasPendingTasks(loop_to_wake)) {
        WakeUpUnlocked(loop_to_wake, GetNextWakeTimeUnlocked(loop_to_wake));
    }
}

void DelayedTaskQueues::SetWakeable(TaskQueueId queue_id,
                                        wtf::Wakeable *wakeable)
{
    std::scoped_lock locker(queue_mutex_);
    WTF_CHECK(!queue_entries_.at(queue_id)->wakeable)
    << "Wakeable can only be set once.";
    queue_entries_.at(queue_id)->wakeable = wakeable;
}

void DelayedTaskQueues::WakeUpUnlocked(TaskQueueId queue_id,
                                           const std::chrono::steady_clock::time_point& time) const
{
    if (queue_entries_.at(queue_id)->wakeable) {
        queue_entries_.at(queue_id)->wakeable->WakeUp(time);
    }
}
} // namespace wtf