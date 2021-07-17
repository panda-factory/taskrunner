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

class TaskRunner {
public:
    static std::unique_ptr<TaskRunner> Create(const std::string& task_name = "");

    virtual void PostTask(const std::function<void ()>& task);

    virtual void AddTaskObserver(intptr_t key, const std::function<void ()>& callback) {};

    virtual void RemoveTaskObserver(intptr_t key) {};

    virtual void Join();

    virtual void Terminate();

    TaskRunner();

    virtual ~TaskRunner();

protected:
    static void TaskRunner::SetCurrentThreadName(const std::string& name);

    std::unique_ptr<std::thread> thread_;

    std::atomic_bool joined_;

    std::atomic_bool terminated_;

private:
    wtf::MessageLoopImpl* loop_;
};


} // namespace wtf

#endif //TASKRUNNER_TASK_RUNNER_H
