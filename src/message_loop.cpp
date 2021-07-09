//
// Created by admin on 2021-07-02.
//

#include "message_loop.h"
#include "thread_local.h"
#include "logging/logging.h"
namespace wtf {

WTF_THREAD_LOCAL std::unique_ptr<MessageLoop> tls_message_loop;

void MessageLoop::EnsureInitializedForCurrentThread() {
    if (tls_message_loop.get() != nullptr) {
        // Already initialized.
        return;
    }
    tls_message_loop.reset(new MessageLoop());
}

bool MessageLoop::IsInitializedForCurrentThread() {
    return tls_message_loop.get() != nullptr;
}

MessageLoop::MessageLoop()
        : loop_(MessageLoopImpl::Create()),
          task_runner_(std::make_unique<wtf::TaskRunner>(loop_.get())) {
    WTF_CHECK(loop_ != nullptr);
    WTF_CHECK(task_runner_ != nullptr);
}

void MessageLoop::AddTaskObserver(intptr_t key, const wtf::Task& callback) {
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

wtf::TaskRunner* MessageLoop::GetTaskRunner() const {
    return task_runner_.get();
}

void MessageLoop::Run() {
    loop_->DoRun();
}

void MessageLoop::Terminate() {
    loop_->DoTerminate();
}

} // namespace wtf