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

    virtual void Dispose(TaskQueueId queue_id);

    virtual void DisposeTasks(TaskQueueId queue_id);

    virtual void AddTaskObserver(TaskQueueId queue_id,
                         intptr_t key,
                         const std::function<void ()>& callback);

    virtual size_t GetNumPendingTasks(TaskQueueId queue_id) const;

    virtual bool HasPendingTasks(TaskQueueId queue_id) const;

    virtual void RemoveTaskObserver(TaskQueueId queue_id, intptr_t key);

    virtual std::vector<std::function<void ()>> GetObserversToNotify(TaskQueueId queue_id) const;

    virtual TaskQueueId CreateTaskQueue();

    virtual std::function<void ()> GetNextTask(TaskQueueId queue_id,
                                               const std::chrono::steady_clock::time_point& from_time =
                                                       std::chrono::steady_clock::time_point::max());

    virtual void RegisterTask(const TaskQueueId& queue_id,
                              const std::function<void ()>& task,
                              const std::chrono::steady_clock::time_point& =
                                      std::chrono::steady_clock::time_point::max());

    virtual void SetWakeable(TaskQueueId queue_id, wtf::Wakeable *wakeable);

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
