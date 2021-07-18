//
// Created by guozhenxiong on 2021-07-02.
//

#ifndef TASKRUNNER_DELAYED_MESSAGE_LOOP_IMPL_H
#define TASKRUNNER_DELAYED_MESSAGE_LOOP_IMPL_H

#include "wakeable.h"
#include "task_queue_id.h"
#include "delayed_task_queues.h"
#include "macros.h"
#include "message_loop_impl.h"

namespace wtf {

class DelayedMessageLoopImpl : public MessageLoopImpl {
public:
    static std::unique_ptr<DelayedMessageLoopImpl> Create();

    virtual ~DelayedMessageLoopImpl();

    virtual void Run() = 0;

    virtual void Terminate() = 0;

    // | MessageLoopImpl |
    void PostTask(const std::function<void ()>& task, const std::chrono::steady_clock::time_point& target_time) override;

    // | MessageLoopImpl |
    void AddTaskObserver(intptr_t key, const std::function<void ()>& callback) override;

    // | MessageLoopImpl |
    void RemoveTaskObserver(intptr_t key) override;

    void DoRun();

    void DoTerminate();

    virtual TaskQueueId GetTaskQueueId() const;

protected:
    friend class DelayedMessageLoop;

    void RunExpiredTasksNow();

protected:
    DelayedMessageLoopImpl();

private:
    TaskQueues* task_queue_;

    TaskQueueId queue_id_;

    std::atomic_bool terminated_;

    void FlushTasks(FlushType type);

    WTF_DISALLOW_COPY_AND_ASSIGN(DelayedMessageLoopImpl);

};
} // namespace wtf

#endif //TASKRUNNER_DELAYED_MESSAGE_LOOP_IMPL_H
