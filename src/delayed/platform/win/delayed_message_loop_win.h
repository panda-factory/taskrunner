//
// Created by guozhenxiong on 2021-07-03.
//

#ifndef TASKRUNNER_DELAYED_MESSAGE_LOOP_WIN_H
#define TASKRUNNER_DELAYED_MESSAGE_LOOP_WIN_H

#include <Windows.h>

#include "delayed/delayed_message_loop_impl.h"

namespace wtf {

class DelayedMessageLoopWin : public DelayedMessageLoopImpl {
public:
    DelayedMessageLoopWin();
    ~DelayedMessageLoopWin() = default;
private:
    struct UniqueHandleTraits {
        static HANDLE InvalidValue() { return NULL; }
        static bool IsValid(HANDLE value) { return value != NULL; }
        static void Free(HANDLE value) { CloseHandle(value); }
    };

    bool running_;
    HANDLE timer_;


    void Run() override;

    void Terminate() override;

    void WakeUp(const std::chrono::steady_clock::time_point& time_point) override;

    DelayedMessageLoopWin(const DelayedMessageLoopWin&) = delete;
    DelayedMessageLoopWin& operator=(const DelayedMessageLoopWin&) = delete;
};

} // namespace wtf


#endif //TASKRUNNER_DELAYED_MESSAGE_LOOP_WIN_H
