//
// Created by guozhenxiong on 2021-07-02.
//

#include "delayed_message_loop.h"
#include "thread_local.h"
#include "logging/logging.h"
#include "delayed_message_loop_impl.h"

namespace wtf {

WTF_THREAD_LOCAL std::unique_ptr<DelayedMessageLoop> tls_message_loop;

void DelayedMessageLoop::EnsureInitializedForCurrentThread() {
    if (tls_message_loop.get() != nullptr) {
        return;
    }
    tls_message_loop.reset(new DelayedMessageLoop());
}

DelayedMessageLoop::DelayedMessageLoop()
        : loop_(DelayedMessageLoopImpl::Create()) {
    WTF_CHECK(loop_ != nullptr);
}

DelayedMessageLoop& DelayedMessageLoop::GetCurrent() {
    auto* loop = tls_message_loop.get();
    WTF_CHECK(loop != nullptr)
            << "DelayedMessageLoop::EnsureInitializedForCurrentThread was not called on "
               "this thread prior to message loop use.";
    return *loop;
}

DelayedMessageLoopImpl* DelayedMessageLoop::GetLoopImpl() const {
    return loop_.get();
}

void DelayedMessageLoop::Run() {
    loop_->DoRun();
}

void DelayedMessageLoop::Terminate() {
    loop_->DoTerminate();
}

} // namespace wtf