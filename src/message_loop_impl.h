//
// Created by guozhenxiong on 2021-07-16.
//

#ifndef TASKRUNNER_MESSAGE_LOOP_IMPL_H
#define TASKRUNNER_MESSAGE_LOOP_IMPL_H

#include <functional>
#include <atomic>
#include <mutex>

#include "task_queues.h"
#include "message_loop_task_queues.h"

#if OS_WIN
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#endif // OS_WIN

namespace wtf {

class MessageLoopImpl {
public:

    virtual void AddTaskObserver(intptr_t key, const std::function<void ()>& callback);

    virtual void RemoveTaskObserver(intptr_t key);

    virtual void Run();

    virtual void PostTask(const std::function<void ()>& task,
                  const std::chrono::steady_clock::time_point& target_time = std::chrono::steady_clock::time_point::max());

    virtual void Terminate();

    virtual void DoTerminate();

    virtual void DoRun();

    MessageLoopImpl();

    ~MessageLoopImpl();

private:
    TaskQueues* task_queues_;

    TaskQueueId queue_id_;

    std::atomic_bool terminated_;

    std::mutex task_mutex_;

    std::condition_variable task_condition_;
};
}

#endif //TASKRUNNER_MESSAGE_LOOP_IMPL_H
