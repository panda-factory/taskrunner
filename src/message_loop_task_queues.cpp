//
// Created by admin on 2021-07-02.
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
                                            const wtf::Task &callback)
{
    std::lock_guard guard(queue_mutex_);
    WTF_DCHECK(callback != nullptr) << "Observer callback must be non-null.";
    queue_entries_.at(queue_id)->task_observers[key] = callback;
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

std::vector<wtf::Task> MessageLoopTaskQueues::GetObserversToNotify(
        TaskQueueId queue_id) const
{
    std::lock_guard guard(queue_mutex_);
    std::vector<wtf::Task> observers;

    for (const auto &observer : queue_entries_.at(queue_id)->task_observers) {
        observers.push_back(observer.second);
    }

    return observers;
}

wtf::Task MessageLoopTaskQueues::GetNextTaskToRun(TaskQueueId queue_id,
                                                  wtf::TimePoint from_time)
{
    std::lock_guard guard(queue_mutex_);
    if (!HasPendingTasksUnlocked(queue_id)) {
        return nullptr;
    }
    wtf::DelayedTask top_task = PeekNextTaskUnlocked(queue_id);

    if (!HasPendingTasksUnlocked(queue_id)) {
        WakeUpUnlocked(queue_id, wtf::TimePoint::Max());
    } else {
        WakeUpUnlocked(queue_id, GetNextWakeTimeUnlocked(queue_id));
    }

    if (top_task.GetTargetTime() > from_time) {
        return nullptr;
    }
    wtf::Task invocation = top_task.GetTask();
    queue_entries_.at(queue_id)
            ->delayed_task_queue.pop();
    return invocation;
}

wtf::TimePoint MessageLoopTaskQueues::GetNextWakeTimeUnlocked(
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

bool MessageLoopTaskQueues::Owns(TaskQueueId owner,
                                 TaskQueueId subsumed) const
{
    std::lock_guard guard(queue_mutex_);
    return owner != g_unmerged && subsumed != g_unmerged &&
           subsumed == queue_entries_.at(owner)->owner_of;
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
        const wtf::Task &task,
        wtf::TimePoint target_time)
{
    std::lock_guard guard(queue_mutex_);
    size_t order = order_++;
    const auto &queue_entry = queue_entries_.at(queue_id);
    queue_entry->delayed_task_queue.push({order, task, target_time});
    TaskQueueId loop_to_wake = queue_id;

    // This can happen when the secondary tasks are paused.
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
                                           wtf::TimePoint time) const
{
    if (queue_entries_.at(queue_id)->wakeable) {
        queue_entries_.at(queue_id)->wakeable->WakeUp(time);
    }
}
} // namespace wtf