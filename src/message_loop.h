//
// Created by guozhenxiong on 2021-07-14.
//

#ifndef TASKRUNNER_MESSAGELOOP_H
#define TASKRUNNER_MESSAGELOOP_H

#include <memory>

#include "message_loop_impl.h"

namespace wtf {
class MessageLoop {
public:
    static void EnsureInitializedForCurrentThread();

    static MessageLoop& GetCurrent();

    virtual void Run();

    virtual void Terminate();

    MessageLoop();
private:
    friend class TaskRunner;
    friend class MessageLoopImpl;

    MessageLoopImpl* GetLoopImpl() const;

    MessageLoop(std::unique_ptr<MessageLoopImpl> loop);

    std::unique_ptr<MessageLoopImpl> loop_;
};
}
#endif //TASKRUNNER_MESSAGELOOP_H
