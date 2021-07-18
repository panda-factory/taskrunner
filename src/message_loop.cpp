//
// Created by guozhenxiong on 2021-07-16.
//
#include "message_loop.h"
#include "logging/logging.h"
#include "thread_local.h"

namespace wtf {

WTF_THREAD_LOCAL std::unique_ptr<MessageLoop> tls_message_loop;

void MessageLoop::EnsureInitializedForCurrentThread(std::unique_ptr<MessageLoop> loop) {
    if (tls_message_loop.get() != nullptr) {
        return;
    }

    if (loop) {
        // Initiate tls message_loop for sub-class message_loop.
        tls_message_loop = std::move(loop);
    } else {
        tls_message_loop.reset(new MessageLoop());
    }
}

MessageLoop::MessageLoop()
        : MessageLoop(std::make_unique<MessageLoopImpl>())
{
    WTF_CHECK(loop_ != nullptr);
}

MessageLoop::MessageLoop(std::unique_ptr<MessageLoopImpl> loop)
    : loop_(std::move(loop))
{
    WTF_CHECK(loop_ != nullptr);
}

MessageLoop& MessageLoop::GetCurrent() {
    auto* loop = tls_message_loop.get();
    WTF_CHECK(loop != nullptr)
    << "MessageLoop::EnsureInitializedForCurrentThread was not called on "
       "this thread prior to message loop use.";
    return *loop;
}

MessageLoopImpl* MessageLoop::GetLoopImpl() const
{
    return loop_.get();
}

void MessageLoop::Run()
{
    loop_->DoRun();
}

void MessageLoop::Terminate() {
    loop_->DoTerminate();
}

}