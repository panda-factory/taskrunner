//
// Created by guozhenxiong on 2021-07-03.
//

#include "delayed_message_loop_win.h"
#include "logging/logging.h"

namespace wtf {

DelayedMessageLoopWin::DelayedMessageLoopWin()
    : timer_(CreateWaitableTimer(NULL, FALSE, NULL)) {
    WTF_CHECK(timer_ != NULL);
}

void DelayedMessageLoopWin::Run() {
    running_ = true;

    while (running_) {
        WTF_CHECK(WaitForSingleObject(timer_, INFINITE) == 0);
        RunExpiredTasksNow();
    }
}

void DelayedMessageLoopWin::Terminate() {
    running_ = false;
    WakeUp(std::chrono::steady_clock::now());
}

void DelayedMessageLoopWin::WakeUp(const std::chrono::steady_clock::time_point& time_point) {
    LARGE_INTEGER due_time = {0};
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    if (time_point > now) {
        due_time.QuadPart = std::chrono::nanoseconds(time_point - now).count() / -100;
    }
    WTF_CHECK(SetWaitableTimer(timer_, &due_time, 0, NULL, NULL, FALSE));
}
} // namespace wtf