//
// Created by guozhenxiong on 2021-07-02.
//

#ifndef TASKRUNNER_TASK_RUNNER_H
#define TASKRUNNER_TASK_RUNNER_H

#include <functional>
#include <chrono>
#include <thread>

#include "config.h"

namespace wtf {
class DelayedMessageLoopImpl;

class TaskRunner {
public:
    virtual void PostTask(const std::function<void ()>& task) = 0;

    virtual void AddTaskObserver(intptr_t key, const std::function<void ()>& callback) = 0;

    virtual void RemoveTaskObserver(intptr_t key) = 0;

    virtual void Join() = 0;

    virtual void Terminate() = 0;

    TaskRunner() = default;

    virtual ~TaskRunner() = default;

protected:
    static void TaskRunner::SetCurrentThreadName(const std::string& name);
};

class WTF_DLL DelayedTaskRunner : public TaskRunner {
public:
    static std::unique_ptr<DelayedTaskRunner> CreateTaskRunner(const std::string& task_name = "");

    void AddTaskObserver(intptr_t key, const std::function<void ()>& callback) override;

    void RemoveTaskObserver(intptr_t key) override;

    void Join() override;

    void PostTask(const std::function<void ()>& task) override;

    void PostTaskForTime(const std::function<void ()>& task, const std::chrono::steady_clock::time_point& target_time);

    void PostDelayedTask(const std::function<void ()>& task, const std::chrono::milliseconds& delay);

    void Terminate() override;

    DelayedTaskRunner();

    ~DelayedTaskRunner();

protected:

private:

    wtf::DelayedMessageLoopImpl* loop_;

    std::unique_ptr<std::thread> thread_;

    std::atomic_bool joined_;

    DelayedTaskRunner(wtf::DelayedMessageLoopImpl* loop);

    DelayedTaskRunner(const DelayedTaskRunner&) = delete;

    DelayedTaskRunner& operator=(const DelayedTaskRunner&) = delete;
};

} // namespace wtf

#endif //TASKRUNNER_TASK_RUNNER_H
