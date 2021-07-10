//
// Created by guozhenxiong on 2021-07-02.
//

#ifndef TASKRUNNER_WAKEABLE_H
#define TASKRUNNER_WAKEABLE_H

#include <chrono>

namespace wtf {

class Wakeable {
public:
    virtual ~Wakeable() {}

    virtual void WakeUp(const std::chrono::steady_clock::time_point& time_point) = 0;
};

}  // namespace wtf

#endif //TASKRUNNER_WAKEABLE_H
