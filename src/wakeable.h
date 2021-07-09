//
// Created by admin on 2021-07-02.
//

#ifndef TEST_WAKEABLE_H
#define TEST_WAKEABLE_H

#include "time_point/time_point.h"

namespace wtf {

class Wakeable {
public:
    virtual ~Wakeable() {}

    virtual void WakeUp(wtf::TimePoint time_point) = 0;
};

}  // namespace wtf

#endif //TEST_WAKEABLE_H
