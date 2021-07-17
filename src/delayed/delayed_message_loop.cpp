//
// Created by guozhenxiong on 2021-07-02.
//

#include "delayed_message_loop.h"
#include "thread_local.h"
#include "logging/logging.h"
#include "delayed_message_loop_impl.h"

namespace wtf {

void DelayedMessageLoop::EnsureInitializedForCurrentThread()
{
    auto loop = std::make_unique<DelayedMessageLoop>();
    MessageLoop::EnsureInitializedForCurrentThread(std::move(loop));
}

DelayedMessageLoop::DelayedMessageLoop()
        : MessageLoop(DelayedMessageLoopImpl::Create())
{
    WTF_CHECK(loop_ != nullptr);
}

MessageLoopImpl* DelayedMessageLoop::GetLoopImpl() const {
    return loop_.get();
}

void DelayedMessageLoop::Run() {
    loop_->DoRun();
}

void DelayedMessageLoop::Terminate() {
    loop_->DoTerminate();
}

} // namespace wtf