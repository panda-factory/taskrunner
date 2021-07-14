//
// Created by guozhenxiong on 2021-07-02.
//

#include "task_runner/task_runner.h"

#include <future>

#include "message_loop.h"
#include "delayed_message_loop_impl.h"
#include <iostream>
#if defined(OS_WIN)
#include <windows.h>
#endif

namespace wtf {
namespace {
#if defined(OS_WIN)
// The information on how to set the thread name comes from
// a MSDN article: http://msdn2.microsoft.com/en-us/library/xcb2z8hs.aspx
const DWORD kVCThreadNameException = 0x406D1388;
typedef struct tagTHREADNAME_INFO {
    DWORD dwType;      // Must be 0x1000.
    LPCSTR szName;     // Pointer to name (in user addr space).
    DWORD dwThreadID;  // Thread ID (-1=caller thread).
    DWORD dwFlags;     // Reserved for future use, must be zero.
} THREADNAME_INFO;
#endif
}
// | static |
void TaskRunner::SetCurrentThreadName(const std::string& name) {
    if (name == "") {
        return;
    }
#if defined(OS_WIN)
    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = name.c_str();
    info.dwThreadID = GetCurrentThreadId();
    info.dwFlags = 0;
    __try {
            RaiseException(kVCThreadNameException, 0, sizeof(info) / sizeof(DWORD),
                           reinterpret_cast<DWORD_PTR*>(&info));
    } __except (EXCEPTION_CONTINUE_EXECUTION) {
    }
#endif
}

std::unique_ptr<DelayedTaskRunner> DelayedTaskRunner::CreateTaskRunner(const std::string& task_name)
{
    auto task_runner = std::make_unique<DelayedTaskRunner>();
    std::promise<wtf::DelayedMessageLoopImpl*> message_loop_promise;
    auto message_loop_future = message_loop_promise.get_future();

    task_runner->thread_ = std::make_unique<std::thread>([&message_loop_promise, task_name]() -> void {
        SetCurrentThreadName(task_name);
        wtf::MessageLoop::EnsureInitializedForCurrentThread();
        auto& loop = MessageLoop::GetCurrent();
        message_loop_promise.set_value(loop.GetLoopImpl());
        loop.Run();
    });
    task_runner->loop_ = message_loop_future.get();

    return std::move(task_runner);
}

DelayedTaskRunner::DelayedTaskRunner() : joined_(false), loop_(nullptr)
{}

DelayedTaskRunner::DelayedTaskRunner(wtf::DelayedMessageLoopImpl* loop)
        : loop_(loop) {}

DelayedTaskRunner::~DelayedTaskRunner()
{
    Terminate();
    Join();
}

void DelayedTaskRunner::AddTaskObserver(intptr_t key, const std::function<void ()>& callback) {
    loop_->AddTaskObserver(key, callback);
}

void DelayedTaskRunner::RemoveTaskObserver(intptr_t key) {
    loop_->RemoveTaskObserver(key);
}

void DelayedTaskRunner::Join() {
    if (joined_) {
        return;
    }
    joined_ = true;
    thread_->join();
}

void DelayedTaskRunner::PostTask(const std::function<void ()>& task) {
    loop_->PostTask(task, std::chrono::steady_clock::now());
}

void DelayedTaskRunner::PostTaskForTime(const std::function<void ()>& task,
                                 const std::chrono::steady_clock::time_point& target_time) {
    loop_->PostTask(task, target_time);
}

void DelayedTaskRunner::PostDelayedTask(const std::function<void ()>& task,
                                 const std::chrono::milliseconds& delay) {
    loop_->PostTask(task, std::chrono::steady_clock::now() + delay);
}

void DelayedTaskRunner::Terminate() {
    PostTask([]() {
        MessageLoop::GetCurrent().Terminate();
    });
}

} // namespace wtf