//
// Created by admin on 2021-07-02.
//

#ifndef TEST_MESSAGE_LOOP_IMPL_H
#define TEST_MESSAGE_LOOP_IMPL_H

#include "task.h"
#include "wakeable.h"
#include "task_queue_id.h"
#include "message_loop_task_queues.h"
#include "wtf/macros.h"

namespace wtf {

class MessageLoopImpl : public Wakeable {
public:
    static std::unique_ptr<MessageLoopImpl> Create();

    virtual ~MessageLoopImpl();

    virtual void Run() = 0;

    virtual void Terminate() = 0;

    void PostTask(const wtf::Task& task, wtf::TimePoint target_time);

    void AddTaskObserver(intptr_t key, const wtf::Task& callback);

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

    WTF_DISALLOW_COPY_AND_ASSIGN(MessageLoopImpl);

};
} // namespace wtf

#endif //TEST_MESSAGE_LOOP_IMPL_H
