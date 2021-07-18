//
// Created by guozhenxiong on 2021-07-16.
//

#ifndef TASKRUNNER_MESSAGE_LOOP_IMPL_H
#define TASKRUNNER_MESSAGE_LOOP_IMPL_H

#include <functional>
#include <atomic>
#include <mutex>

#include "task_queues.h"
#include "delayed/delayed_task_queues_impl.h"

#if OS_WIN
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#endif // OS_WIN

namespace wtf {

class MessageLoopImpl : public Wakeable {
public:

    virtual void AddTaskObserver(intptr_t key, const std::function<void ()>& callback);

    virtual void RemoveTaskObserver(intptr_t key);

    void FlushTasks(FlushType type);

    virtual void Run();

    virtual void PostTask(const std::function<void ()>& task,
                  const std::chrono::steady_clock::time_point& target_time = std::chrono::steady_clock::time_point::max());

    virtual void Terminate();

    virtual void DoTerminate();

    virtual void DoRun();

    // | Wakeable |
    virtual void WakeUp(const std::chrono::steady_clock::time_point& time_point);

    MessageLoopImpl();

    MessageLoopImpl(wtf::Wakeable *wakeable);

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
