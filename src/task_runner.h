//
// Created by guozhenxiong on 2021-07-02.
//

#ifndef TASKRUNNER_TASK_RUNNER_H
#define TASKRUNNER_TASK_RUNNER_H

#include <functional>

#include "time_point//time_point.h"
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

    virtual void PostTask(const wtf::Task& task) override;

    virtual void PostTaskForTime(const wtf::Task& task,
                                 wtf::TimePoint target_time);

    virtual void PostDelayedTask(const wtf::Task& task, wtf::TimeDelta delay);

    virtual bool RunsTasksOnCurrentThread();

    virtual TaskQueueId GetTaskQueueId();

    TaskRunner(MessageLoopImpl* loop);

    virtual ~TaskRunner();

protected:

private:
    MessageLoopImpl* loop_;

    WTF_DISALLOW_COPY_AND_ASSIGN(TaskRunner);
};
} // namespace wtf

#endif //TASKRUNNER_TASK_RUNNER_H
