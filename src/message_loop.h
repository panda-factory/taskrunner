//
// Created by admin on 2021-07-02.
//

#ifndef TEST_MESSAGE_LOOP_H
#define TEST_MESSAGE_LOOP_H

#include "task_runner.h"

namespace wtf {
class MessageLoop {
public:
    static MessageLoop& GetCurrent();

    void Run();

    void Terminate();

    void AddTaskObserver(intptr_t key, const wtf::Task& callback);

    void RemoveTaskObserver(intptr_t key);

    wtf::TaskRunner* GetTaskRunner() const;

    // Exposed for the embedder shell which allows clients to poll for events
    // instead of dedicating a thread to the message loop.
    void RunExpiredTasksNow();

    static void EnsureInitializedForCurrentThread();

    /// Returns true if \p EnsureInitializedForCurrentThread has been called on
    /// this thread already.
    static bool IsInitializedForCurrentThread();

    ~MessageLoop() {};

    /// Gets the unique identifier for the TaskQueue associated with the current
    /// thread.
    /// \see wtf::MessageLoopTaskQueues
    static TaskQueueId GetCurrentTaskQueueId();

private:
    friend class TaskRunner;
    friend class MessageLoopImpl;

    std::unique_ptr<MessageLoopImpl> loop_;
    std::unique_ptr<wtf::TaskRunner> task_runner_;

    MessageLoop();

    MessageLoopImpl* GetLoopImpl() const;

    WTF_DISALLOW_COPY_AND_ASSIGN(MessageLoop);

};
} // namespace wtf

#endif //TEST_MESSAGE_LOOP_H
