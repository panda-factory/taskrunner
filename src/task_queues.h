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
#include "task_queues_impl.h"
#include "wakeable.h"
#include "task_type.h"

namespace wtf {
class TaskQueue {
public:
    TaskQueue(TaskQueueId queue_id);

    using TaskObservers = std::map<intptr_t, std::function<void ()>>;

    TaskObservers task_observers;

    Wakeable* wakeable;

    TaskQueueId owner_of;

    std::queue<Task> task_queue;
};

class TaskQueues {
public:
    static TaskQueues *GetInstance();

    void Dispose(TaskQueueId queue_id);

    void DisposeTasks(TaskQueueId queue_id);

    void AddTaskObserver(TaskQueueId queue_id,
                         intptr_t key,
                         const std::function<void ()>& callback);

    size_t GetNumPendingTasks(TaskQueueId queue_id) const;

    bool HasPendingTasks(TaskQueueId queue_id) const;

    void RemoveTaskObserver(TaskQueueId queue_id, intptr_t key);

    std::vector<std::function<void ()>> GetObserversToNotify(TaskQueueId queue_id) const;

    TaskQueueId CreateTaskQueue();

    std::function<void ()> GetNextTask(TaskQueueId queue_id);

    void RegisterTask(const TaskQueueId& queue_id,
                              const std::function<void ()>& task);

    void SetWakeable(TaskQueueId queue_id, wtf::Wakeable *wakeable);

    TaskQueues(std::unique_ptr<TaskQueuesImpl> task_queues);

private:

    std::unique_ptr<TaskQueuesImpl> task_queues_impl_;
};

enum class FlushType {
    kSingle,
    kAll,
};
} // namespace wtf

#endif //TASKRUNNER_TASK_QUEUES_H
