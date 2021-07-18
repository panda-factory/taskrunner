//
// Created by guozhenxiong on 2021-07-18.
//

#ifndef TASKRUNNER_TASK_QUEUES_IMPL_H
#define TASKRUNNER_TASK_QUEUES_IMPL_H

#include <functional>
#include <mutex>
#include <map>

#include "wakeable.h"
#include "task_queue_id.h"

namespace wtf {
class TaskQueue;

class TaskQueuesImpl {
public:
    static TaskQueuesImpl *GetInstance();

    virtual void Dispose(TaskQueueId queue_id);

    virtual void DisposeTasks(TaskQueueId queue_id);

    virtual void AddTaskObserver(TaskQueueId queue_id,
                                 intptr_t key,
                                 const std::function<void ()>& callback);

    virtual std::vector<std::function<void ()>> GetObserversToNotify(TaskQueueId queue_id) const;

    virtual size_t GetNumPendingTasks(TaskQueueId queue_id) const;

    virtual bool HasPendingTasks(TaskQueueId queue_id) const;

    virtual void RemoveTaskObserver(TaskQueueId queue_id, intptr_t key);

    virtual TaskQueueId CreateTaskQueue();

    virtual std::function<void ()> GetNextTask(TaskQueueId queue_id);

    virtual void RegisterTask(TaskQueueId queue_id,
                              const std::function<void ()>& task);

    virtual void SetWakeable(TaskQueueId queue_id, wtf::Wakeable *wakeable);

    TaskQueuesImpl();

protected:
    size_t task_queue_id_counter_ = 0;

    mutable std::mutex queue_mutex_;

    std::atomic_int order_ = 0;

private:
    void WakeUpUnlocked(TaskQueueId queue_id, const std::chrono::steady_clock::time_point& time) const;

    std::map<TaskQueueId, std::unique_ptr<TaskQueue>> queue_entries_;
};
} // namespace wtf

#endif //TASKRUNNER_TASK_QUEUES_IMPL_H
