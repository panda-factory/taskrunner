//
// Created by guozhenxiong on 2021-07-02.
//

#ifndef TASKRUNNER_DELAYED_TASK_QUEUES_IMPL_H
#define TASKRUNNER_DELAYED_TASK_QUEUES_IMPL_H

#include <map>
#include <mutex>

#include "task_type.h"
#include "task_queue_id.h"
#include "task_queues.h"
#include "macros.h"

namespace wtf {

class TaskQueueEntry {
public:
    using TaskObservers = std::map<intptr_t, std::function<void ()>>;
    Wakeable* wakeable;
    TaskObservers task_observers;

    void Erase();

    bool IsEmpty();

    size_t Size();

    void Pop();

    void Push(const DelayedTask& task);

    DelayedTask Top();

    TaskQueueId owner_of;

    explicit TaskQueueEntry(TaskQueueId owner_of);

private:
    DelayedTaskQueue task_queue_;

    WTF_DISALLOW_COPY_ASSIGN_AND_MOVE(TaskQueueEntry);
};

class DelayedTaskQueuesImpl : public TaskQueuesImpl {
public:

    static DelayedTaskQueuesImpl* GetInstance();

    // | TaskQueuesImpl |
    TaskQueueId CreateTaskQueue() override;

    // | TaskQueuesImpl |
    void Dispose(TaskQueueId queue_id) override;

    // | TaskQueuesImpl |
    void DisposeTasks(TaskQueueId queue_id) override;

    // | TaskQueuesImpl |
    void RegisterTask(TaskQueueId queue_id,
                      const std::function<void ()>& task,
                      const std::chrono::steady_clock::time_point& target_time) override;

    // | TaskQueuesImpl |
    std::function<void ()> GetNextTask(TaskQueueId queue_id,
                                       const std::chrono::steady_clock::time_point& from_time) override;

    // | TaskQueuesImpl |
    size_t GetNumPendingTasks(TaskQueueId queue_id) const override;

    // | TaskQueuesImpl |
    bool HasPendingTasks(TaskQueueId queue_id) const override;

    // | TaskQueuesImpl |
    void AddTaskObserver(TaskQueueId queue_id,
                         intptr_t key,
                         const std::function<void ()>& callback) override;

    // | TaskQueuesImpl |
    void RemoveTaskObserver(TaskQueueId queue_id, intptr_t key) override;

    // | TaskQueuesImpl |
    std::vector<std::function<void ()>> GetObserversToNotify(TaskQueueId queue_id) const override;

    // | TaskQueuesImpl |
    void SetWakeable(TaskQueueId queue_id, wtf::Wakeable* wakeable) override;

    DelayedTaskQueuesImpl() = default;

    ~DelayedTaskQueuesImpl() = default;

private:
    void WakeUpUnlocked(TaskQueueId queue_id, const std::chrono::steady_clock::time_point& time) const;

    wtf::DelayedTask PeekNextTaskUnlocked(TaskQueueId owner) const;

    std::chrono::steady_clock::time_point GetNextWakeTimeUnlocked(TaskQueueId queue_id) const;

    std::map<TaskQueueId, std::unique_ptr<TaskQueueEntry>> queue_entries_;

    WTF_DISALLOW_COPY_ASSIGN_AND_MOVE(DelayedTaskQueuesImpl);
};
} // namespace wtf

#endif //TASKRUNNER_DELAYED_TASK_QUEUES_IMPL_H
