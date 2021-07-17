//
// Created by guozhenxiong on 2021-07-17.
//

#ifndef TASKRUNNER_DELAYED_TASK_RUNNER_H
#define TASKRUNNER_DELAYED_TASK_RUNNER_H

#include "task_runner.h"

namespace wtf {
class DelayedMessageLoopImpl;

class WTF_DLL DelayedTaskRunner : public TaskRunner {
public:
    static std::unique_ptr<DelayedTaskRunner> CreateTaskRunner(const std::string &task_name = "");

    void AddTaskObserver(intptr_t key, const std::function<void()> &callback) override;

    void RemoveTaskObserver(intptr_t key) override;

    void Join() override;

    void PostTask(const std::function<void()> &task) override;

    void PostTaskForTime(const std::function<void()> &task, const std::chrono::steady_clock::time_point &target_time);

    void PostDelayedTask(const std::function<void()> &task, const std::chrono::milliseconds &delay);

    void Terminate() override;

    DelayedTaskRunner();

    ~DelayedTaskRunner();

protected:

private:

    wtf::DelayedMessageLoopImpl *loop_;

    DelayedTaskRunner(wtf::DelayedMessageLoopImpl *loop);

    DelayedTaskRunner(const DelayedTaskRunner &) = delete;

    DelayedTaskRunner &operator=(const DelayedTaskRunner &) = delete;
};
} // namespace wtf

#endif //TASKRUNNER_DELAYED_TASK_RUNNER_H
