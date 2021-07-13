//
// Created by guozhenxiong on 2021-07-02.
//

#ifndef TASKRUNNER_MESSAGE_LOOP_H
#define TASKRUNNER_MESSAGE_LOOP_H

#include "task_runner/task_runner.h"
#include "task_queue_id.h"
#include "message_loop_impl.h"
#include "macros.h"

namespace wtf {
class MessageLoop {
public:
    static void EnsureInitializedForCurrentThread();

    static bool IsInitializedForCurrentThread();

    static TaskQueueId GetCurrentTaskQueueId();

    static MessageLoop& GetCurrent();

    void Run();

    void Terminate();

    void AddTaskObserver(intptr_t key, const std::function<void ()>& callback);

    void RemoveTaskObserver(intptr_t key);

    void RunExpiredTasksNow();

    ~MessageLoop() {};


private:
    friend class TaskRunner;
    friend class MessageLoopImpl;

    std::unique_ptr<MessageLoopImpl> loop_;

    MessageLoop();

    MessageLoopImpl* GetLoopImpl() const;

    WTF_DISALLOW_COPY_AND_ASSIGN(MessageLoop);

};
} // namespace wtf

#endif //TASKRUNNER_MESSAGE_LOOP_H
