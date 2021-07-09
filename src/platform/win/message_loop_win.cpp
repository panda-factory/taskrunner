//
// Created by admin on 2021-07-03.
//

#include "message_loop_win.h"
#include "logging/logging.h"

namespace wtf {

MessageLoopWin::MessageLoopWin()
    : timer_(CreateWaitableTimer(NULL, FALSE, NULL)) {
    WTF_CHECK(timer_ != NULL);
}

void MessageLoopWin::Run() {
    running_ = true;

    while (running_) {
        WTF_CHECK(WaitForSingleObject(timer_, INFINITE) == 0);
        RunExpiredTasksNow();
    }
}

void MessageLoopWin::Terminate() {
    running_ = false;
    WakeUp(wtf::TimePoint::Now());
}

void MessageLoopWin::WakeUp(wtf::TimePoint time_point) {
    LARGE_INTEGER due_time = {0};
    wtf::TimePoint now = wtf::TimePoint::Now();
    if (time_point > now) {
        due_time.QuadPart = (time_point - now).ToNanoseconds() / -100;
    }
    WTF_CHECK(SetWaitableTimer(timer_, &due_time, 0, NULL, NULL, FALSE));
}
} // namespace wtf