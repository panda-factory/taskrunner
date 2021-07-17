//
// Created by guozhenxiong on 2021-07-16.
//

#include "task_queues.h"

#include "logging/logging.h"

namespace wtf {

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