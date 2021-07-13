//
// Created by guozhenxiong on 2021-07-02.
//

#include "message_loop.h"
#include "thread_local.h"
#include "logging/logging.h"
#include "message_loop_impl.h"

namespace wtf {

WTF_THREAD_LOCAL std::unique_ptr<MessageLoop> tls_message_loop;

void MessageLoop::EnsureInitializedForCurrentThread() {
    if (tls_message_loop.get() != nullptr) {
        return;
    }
    tls_message_loop.reset(new MessageLoop());
}

bool MessageLoop::IsInitializedForCurrentThread() {
    return tls_message_loop.get() != nullptr;
}

MessageLoop::MessageLoop()
        : loop_(MessageLoopImpl::Create()) {
    WTF_CHECK(loop_ != nullptr);
}

void MessageLoop::AddTaskObserver(intptr_t key, const std::function<void ()>& callback) {
    loop_->AddTaskObserver(key, callback);
}

MessageLoop& MessageLoop::GetCurrent() {
    auto* loop = tls_message_loop.get();
    WTF_CHECK(loop != nullptr)
            << "MessageLoop::EnsureInitializedForCurrentThread was not called on "
               "this thread prior to message loop use.";
    return *loop;
}

TaskQueueId MessageLoop::GetCurrentTaskQueueId() {
    auto* loop = tls_message_loop.get();
    WTF_CHECK(loop != nullptr)
            << "MessageLoop::EnsureInitializedForCurrentThread was not called on "
               "this thread prior to message loop use.";
    return loop->GetLoopImpl()->GetTaskQueueId();
}

MessageLoopImpl* MessageLoop::GetLoopImpl() const {
    return loop_.get();
}

void MessageLoop::Run() {
    loop_->DoRun();
}

void MessageLoop::Terminate() {
    loop_->DoTerminate();
}

} // namespace wtf