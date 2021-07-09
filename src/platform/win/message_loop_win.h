//
// Created by guozhenxiong on 2021-07-03.
//

#ifndef TASKRUNNER_MESSAGE_LOOP_WIN_H
#define TASKRUNNER_MESSAGE_LOOP_WIN_H

#include <windows.h>

#include "message_loop_impl.h"

namespace wtf {

class MessageLoopWin : public MessageLoopImpl {
public:
    MessageLoopWin();
    ~MessageLoopWin() = default;
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

    void WakeUp(wtf::TimePoint time_point) override;

    WTF_DISALLOW_COPY_AND_ASSIGN(MessageLoopWin);
};

} // namespace wtf


#endif //TASKRUNNER_MESSAGE_LOOP_WIN_H
