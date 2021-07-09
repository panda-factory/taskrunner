//
// Created by guozhenxiong on 2021-07-02.
//

#include "thread.h"

#include <future>

#include "message_loop.h"

#if defined(OS_WIN)
#include <windows.h>
#elif defined(OS_FUCHSIA)
#include <lib/zx/thread.h>
#else
#include <pthread.h>
#endif

namespace wtf {

Thread::Thread(const std::string& name) : joined_(false) {
    std::promise<wtf::TaskRunner*> task_runner_promise;
    auto task_runner_future = task_runner_promise.get_future();
    thread_ = std::make_unique<std::thread>([&task_runner_promise, name]() -> void {
        SetCurrentThreadName(name);
        wtf::MessageLoop::EnsureInitializedForCurrentThread();
        auto& loop = MessageLoop::GetCurrent();
        auto runner = loop.GetTaskRunner();
        task_runner_promise.set_value(runner);
        loop.Run();
    });
    task_runner_ = task_runner_future.get();
}

Thread::~Thread() {
    Join();
}

wtf::TaskRunner& Thread::GetTaskRunner() const {
    return *task_runner_;
}

void Thread::Join() {
    if (joined_) {
        return;
    }
    joined_ = true;
    task_runner_->PostTask([]() { MessageLoop::GetCurrent().Terminate(); });
    thread_->join();
}

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

void Thread::SetCurrentThreadName(const std::string& name) {
    if (name == "") {
        return;
    }
#if defined(OS_MACOSX)
    pthread_setname_np(name.c_str());
#elif defined(OS_LINUX) || defined(OS_ANDROID)
    pthread_setname_np(pthread_self(), name.c_str());
#elif defined(OS_WIN)
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
#elif defined(OS_FUCHSIA)
    zx::thread::self()->set_property(ZX_PROP_NAME, name.c_str(), name.size());
#else
  WTF_DLOG(INFO) << "Could not set the thread name to '" << name
                 << "' on this platform.";
#endif
}

} // namespace wtf