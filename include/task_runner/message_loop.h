//
// Created by guozhenxiong on 2021-07-02.
//

#ifndef TASKRUNNER_MESSAGE_LOOP_H
#define TASKRUNNER_MESSAGE_LOOP_H

#include "task_runner/task_runner.h"

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

    wtf::TaskRunner* GetTaskRunner() const;

    void RunExpiredTasksNow();

    ~MessageLoop() {};


private:
    friend class TaskRunner;
    friend class MessageLoopImpl;

    std::unique_ptr<MessageLoopImpl> loop_;
    std::unique_ptr<wtf::TaskRunner> task_runner_;

    MessageLoop();

    MessageLoopImpl* GetLoopImpl() const;

    MessageLoop(const MessageLoop&) = delete;
    MessageLoop& operator=(const MessageLoop&) = delete;

};
} // namespace wtf

#endif //TASKRUNNER_MESSAGE_LOOP_H
