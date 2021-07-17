//
// Created by guozhenxiong on 2021-07-02.
//

#include "message_loop_task_queues.h"

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

MessageLoopTaskQueues *MessageLoopTaskQueues::GetInstance()
{
    static MessageLoopTaskQueues *instance = nullptr;
    if (!instance) {
        instance = new MessageLoopTaskQueues();
    }
    return instance;
}

void MessageLoopTaskQueues::AddTaskObserver(TaskQueueId queue_id,
                                            intptr_t key,
                                            const std::function<void ()> &callback)
{
    std::lock_guard guard(queue_mutex_);
    WTF_DCHECK(callback != nullptr) << "Observer callback must be non-null.";
    queue_entries_.at(queue_id)->task_observers[key] = callback;
}

void MessageLoopTaskQueues::RemoveTaskObserver(TaskQueueId queue_id,
                                               intptr_t key) {
    std::lock_guard guard(queue_mutex_);
    queue_entries_.at(queue_id)->task_observers.erase(key);
}

TaskQueueId MessageLoopTaskQueues::CreateTaskQueue()
{
    std::scoped_lock locker(queue_mutex_);
    TaskQueueId loop_id = TaskQueueId(task_queue_id_counter_);
    ++task_queue_id_counter_;
    queue_entries_[loop_id] = std::make_unique<TaskQueueEntry>(loop_id);
    return loop_id;
}

void MessageLoopTaskQueues::Dispose(TaskQueueId queue_id)
{
    std::scoped_lock locker(queue_mutex_);
    const auto &queue_entry = queue_entries_.at(queue_id);
    TaskQueueId subsumed = queue_entry->owner_of;
    queue_entries_.erase(queue_id);
    if (subsumed != g_unmerged) {
        queue_entries_.erase(subsumed);
    }
}

void MessageLoopTaskQueues::DisposeTasks(TaskQueueId queue_id)
{
    std::lock_guard guard(queue_mutex_);
    const auto &queue_entry = queue_entries_.at(queue_id);
    queue_entry->delayed_task_queue = {};
}

std::vector<std::function<void ()>> MessageLoopTaskQueues::GetObserversToNotify(
        TaskQueueId queue_id) const
{
    std::lock_guard guard(queue_mutex_);
    std::vector<std::function<void ()>> observers;

    for (const auto &observer : queue_entries_.at(queue_id)->task_observers) {
        observers.push_back(observer.second);
    }

    return observers;
}

std::function<void ()> MessageLoopTaskQueues::GetNextTaskToRun(TaskQueueId queue_id,
                                                  const std::chrono::steady_clock::time_point& from_time)
{
    std::lock_guard guard(queue_mutex_);
    if (!HasPendingTasksUnlocked(queue_id)) {
        return nullptr;
    }
    wtf::DelayedTask top_task = PeekNextTaskUnlocked(queue_id);

    if (!HasPendingTasksUnlocked(queue_id)) {
        WakeUpUnlocked(queue_id, std::chrono::steady_clock::time_point::max());
    } else {
        WakeUpUnlocked(queue_id, GetNextWakeTimeUnlocked(queue_id));
    }

    if (top_task.GetTargetTime() > from_time) {
        return nullptr;
    }
    std::function<void ()> invocation = top_task.GetTask();
    queue_entries_.at(queue_id)
            ->delayed_task_queue.pop();
    return invocation;
}

std::chrono::steady_clock::time_point MessageLoopTaskQueues::GetNextWakeTimeUnlocked(
        TaskQueueId queue_id) const
{
    return PeekNextTaskUnlocked(queue_id).GetTargetTime();
}

bool MessageLoopTaskQueues::HasPendingTasksUnlocked(
        TaskQueueId queue_id) const
{
    const auto &entry = queue_entries_.at(queue_id);
    bool is_owner_of = entry->owner_of == g_unmerged;
    if (is_owner_of) {
        return false;
    }

    return !entry->delayed_task_queue.empty();
}

wtf::DelayedTask MessageLoopTaskQueues::PeekNextTaskUnlocked(
        TaskQueueId owner) const
{
    WTF_DCHECK(HasPendingTasksUnlocked(owner));
    const auto &entry = queue_entries_.at(owner);

    return entry->delayed_task_queue.top();
}

void MessageLoopTaskQueues::RegisterTask(
        TaskQueueId queue_id,
        const std::function<void ()> &task,
        const std::chrono::steady_clock::time_point& target_time)
{
    std::lock_guard guard(queue_mutex_);
    size_t order = order_++;
    const auto &queue_entry = queue_entries_.at(queue_id);
    queue_entry->delayed_task_queue.push({order, task, target_time});
    TaskQueueId loop_to_wake = queue_id;

    if (HasPendingTasksUnlocked(loop_to_wake)) {
        WakeUpUnlocked(loop_to_wake, GetNextWakeTimeUnlocked(loop_to_wake));
    }
}

void MessageLoopTaskQueues::SetWakeable(TaskQueueId queue_id,
                                        wtf::Wakeable *wakeable)
{
    std::lock_guard guard(queue_mutex_);
    WTF_CHECK(!queue_entries_.at(queue_id)->wakeable)
    << "Wakeable can only be set once.";
    queue_entries_.at(queue_id)->wakeable = wakeable;
}

void MessageLoopTaskQueues::WakeUpUnlocked(TaskQueueId queue_id,
                                           const std::chrono::steady_clock::time_point& time) const
{
    if (queue_entries_.at(queue_id)->wakeable) {
        queue_entries_.at(queue_id)->wakeable->WakeUp(time);
    }
}
} // namespace wtf