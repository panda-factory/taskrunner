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
class MessageLoopImpl;

class WTF_DLL TaskRunner {
public:
    static std::unique_ptr<TaskRunner> CreateTaskRunner(const std::string& task_name = "");

    void Join();

    void PostTask(const std::function<void ()>& task);

    void PostTaskForTime(const std::function<void ()>& task, const std::chrono::steady_clock::time_point& target_time);

    void PostDelayedTask(const std::function<void ()>& task, const std::chrono::milliseconds& delay);

    void Terminate();

    TaskRunner();

    ~TaskRunner();

protected:

private:
    static void TaskRunner::SetCurrentThreadName(const std::string& name);

    wtf::MessageLoopImpl* loop_;

    std::unique_ptr<std::thread> thread_;

    std::atomic_bool joined_;

    TaskRunner(wtf::MessageLoopImpl* loop);

    TaskRunner(const TaskRunner&) = delete;

    TaskRunner& operator=(const TaskRunner&) = delete;
};

} // namespace wtf

#endif //TASKRUNNER_TASK_RUNNER_H
