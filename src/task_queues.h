//
// Created by guozhenxiong on 2021-07-16.
//

#ifndef TASKRUNNER_TASK_QUEUES_H
#define TASKRUNNER_TASK_QUEUES_H

#include <queue>
#include <functional>
#include <mutex>
#include <map>

#include "task_queue_id.h"

namespace wtf {
class TaskQueue {
public:
    TaskQueue(TaskQueueId queue_id) : owner_of(queue_id) {};

    using TaskObservers = std::map<intptr_t, std::function<void ()>>;

    TaskObservers task_observers;

    TaskQueueId owner_of;

    std::queue<std::function<void ()>> task_queue;
};

class TaskQueues {
public:
    static TaskQueues *GetInstance();

    void AddTaskObserver(TaskQueueId queue_id,
                         intptr_t key,
                         const std::function<void ()>& callback);

    void RemoveTaskObserver(TaskQueueId queue_id, intptr_t key);

    TaskQueueId CreateTaskQueue();

    std::function<void ()> GetNextTask(TaskQueueId queue_id);

    void RegisterTask(TaskQueueId queue_id,
                      const std::function<void ()>& task);

private:

    mutable std::mutex queue_mutex_;

    size_t task_queue_id_counter_ = 0;

    std::atomic_int order_ = 0;

    std::map<TaskQueueId, std::unique_ptr<TaskQueue>> queue_entries_;
};
} // namespace wtf

#endif //TASKRUNNER_TASK_QUEUES_H
