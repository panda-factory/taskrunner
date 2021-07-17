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
    static void EnsureInitializedForCurrentThread(std::unique_ptr<MessageLoop> loop = nullptr);

    static MessageLoop& GetCurrent();

    virtual void Run();

    virtual void Terminate();

    MessageLoopImpl* GetLoopImpl() const;

    MessageLoop();

protected:
    MessageLoop(std::unique_ptr<MessageLoopImpl> loop);

    std::unique_ptr<MessageLoopImpl> loop_;
private:
    friend class TaskRunner;
    friend class MessageLoopImpl;

};
}
#endif //TASKRUNNER_MESSAGELOOP_H
