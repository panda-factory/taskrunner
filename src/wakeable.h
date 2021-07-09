//
// Created by guozhenxiong on 2021-07-02.
//

#ifndef TASKRUNNER_WAKEABLE_H
#define TASKRUNNER_WAKEABLE_H

#include "time_point/time_point.h"

namespace wtf {

class Wakeable {
public:
    virtual ~Wakeable() {}

    virtual void WakeUp(wtf::TimePoint time_point) = 0;
};

}  // namespace wtf

#endif //TASKRUNNER_WAKEABLE_H
