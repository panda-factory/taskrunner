//
// Created by guozhenxiong on 2021-07-02.
//

#ifndef TASKRUNNER_MESSAGE_LOOP_IMPL_H
#define TASKRUNNER_MESSAGE_LOOP_IMPL_H

#include "wakeable.h"
#include "task_queue_id.h"
#include "message_loop_task_queues.h"

namespace wtf {

class MessageLoopImpl : public Wakeable {
public:
    static std::unique_ptr<MessageLoopImpl> Create();

    virtual ~MessageLoopImpl();

    virtual void Run() = 0;

    virtual void Terminate() = 0;

    void PostTask(const std::function<void ()>& task, const std::chrono::steady_clock::time_point& target_time);

    void AddTaskObserver(intptr_t key, const std::function<void ()>& callback);

    void RemoveTaskObserver(intptr_t key);

    void DoRun();

    void DoTerminate();

    virtual TaskQueueId GetTaskQueueId() const;

protected:
    friend class MessageLoop;

    void RunExpiredTasksNow();

    void RunSingleExpiredTaskNow();

protected:
    MessageLoopImpl();

private:
    MessageLoopTaskQueues* task_queue_;
    TaskQueueId queue_id_;

    std::atomic_bool terminated_;

    void FlushTasks(FlushType type);

    MessageLoopImpl(const MessageLoopImpl&) = delete;
    MessageLoopImpl& operator=(const MessageLoopImpl&) = delete;

};
} // namespace wtf

#endif //TASKRUNNER_MESSAGE_LOOP_IMPL_H
