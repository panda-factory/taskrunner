//
// Created by guozhenxiong on 2021-07-02.
//

#ifndef TASKRUNNER_TASK_H
#define TASKRUNNER_TASK_H

#include <functional>

namespace wtf {

using Task = std::function<void ()>;

} // namespace wtf

#endif //TASKRUNNER_TASK_H
