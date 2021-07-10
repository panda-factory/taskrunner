//
// Created by guozhenxiong on 2021-07-02.
//

#ifndef TASKRUNNER_TASK_RUNNER_H
#define TASKRUNNER_TASK_RUNNER_H

#include <functional>

#include "wtf/macros.h"
#include "task_queue_id.h"
#include "message_loop_impl.h"
namespace wtf {

class BasicTaskRunner {
public:
    virtual void PostTask(const wtf::Task& task) = 0;
};

class TaskRunner : public BasicTaskRunner {
public:
    static void RunNowOrPostTask(wtf::TaskRunner* runner,
                                 const wtf::Task& task);

    void PostTask(const wtf::Task& task) override;

    void PostTaskForTime(const wtf::Task& task, const std::chrono::steady_clock::time_point& target_time);

    void PostDelayedTask(const wtf::Task& task, const std::chrono::milliseconds& delay);

    virtual bool RunsTasksOnCurrentThread();

    virtual TaskQueueId GetTaskQueueId();

    TaskRunner(MessageLoopImpl* loop);

    virtual ~TaskRunner();

protected:

private:
    MessageLoopImpl* loop_;

    TaskRunner(const TaskRunner&) = delete;

    TaskRunner& operator=(const TaskRunner&) = delete;
};
} // namespace wtf

#endif //TASKRUNNER_TASK_RUNNER_H
