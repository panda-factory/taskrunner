//
// Created by guozhenxiong on 2021-07-02.
//

#ifndef TASKRUNNER_TASK_QUEUE_ID_H
#define TASKRUNNER_TASK_QUEUE_ID_H

namespace wtf {

class TaskQueueId {
public:
    static const size_t unmerged;

    TaskQueueId() = default;

    explicit TaskQueueId(size_t value) : value_(value) {}

    operator size_t() const { return value_; }

private:
    size_t value_ = unmerged;
};

}  // namespace wtf
#endif //TASKRUNNER_TASK_QUEUE_ID_H
