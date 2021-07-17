//
// Created by guozhenxiong on 2021-07-02.
//

#ifndef TASKRUNNER_DELAYED_MESSAGE_LOOP_H
#define TASKRUNNER_DELAYED_MESSAGE_LOOP_H

#include "message_loop.h"
#include "task_runner/task_runner.h"
#include "task_queue_id.h"
#include "delayed_message_loop_impl.h"
#include "macros.h"

namespace wtf {
class DelayedMessageLoop : public MessageLoop {
public:
    static void EnsureInitializedForCurrentThread();

    static DelayedMessageLoop& GetCurrent();

    // | MessageLoop |
    void Run() override;

    void Terminate();

    void AddTaskObserver(intptr_t key, const std::function<void ()>& callback);

    void RemoveTaskObserver(intptr_t key);

    void RunExpiredTasksNow();

    ~DelayedMessageLoop() {};


private:
    friend class DelayedTaskRunner;
    friend class DelayedMessageLoopImpl;

    std::unique_ptr<DelayedMessageLoopImpl> loop_;

    DelayedMessageLoop();

    DelayedMessageLoopImpl* GetLoopImpl() const;

    WTF_DISALLOW_COPY_AND_ASSIGN(DelayedMessageLoop);

};
} // namespace wtf

#endif //TASKRUNNER_DELAYED_MESSAGE_LOOP_H
