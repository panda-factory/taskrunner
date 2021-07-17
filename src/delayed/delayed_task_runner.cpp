//
// Created by guozhenxiong on 2021-07-17.
//

#include "task_runner/delayed_task_runner.h"

#include <future>

#include "delayed_message_loop.h"

namespace wtf {

std::unique_ptr<DelayedTaskRunner> DelayedTaskRunner::CreateTaskRunner(const std::string& task_name)
{
    auto task_runner = std::make_unique<DelayedTaskRunner>();
    std::promise<wtf::MessageLoopImpl*> message_loop_promise;
    auto message_loop_future = message_loop_promise.get_future();

    task_runner->thread_ = std::make_unique<std::thread>([&message_loop_promise, task_name]() -> void {
        SetCurrentThreadName(task_name);
        wtf::DelayedMessageLoop::EnsureInitializedForCurrentThread();
        auto& loop = DelayedMessageLoop::GetCurrent();
        message_loop_promise.set_value(loop.GetLoopImpl());
        loop.Run();
    });
    task_runner->loop_ = message_loop_future.get();

    return std::move(task_runner);
}

DelayedTaskRunner::DelayedTaskRunner() : loop_(nullptr)
{}

DelayedTaskRunner::DelayedTaskRunner(wtf::DelayedMessageLoopImpl* loop)
        : loop_(loop) {}

DelayedTaskRunner::~DelayedTaskRunner()
{
    Terminate();
    Join();
}

void DelayedTaskRunner::AddTaskObserver(intptr_t key, const std::function<void ()>& callback)
{
    loop_->AddTaskObserver(key, callback);
}

void DelayedTaskRunner::RemoveTaskObserver(intptr_t key)
{
    loop_->RemoveTaskObserver(key);
}

void DelayedTaskRunner::Join()
{
    if (joined_) {
        return;
    }
    joined_ = true;
    thread_->join();
}

void DelayedTaskRunner::PostTask(const std::function<void ()>& task)
{
    loop_->PostTask(task, std::chrono::steady_clock::now());
}

void DelayedTaskRunner::PostTaskFor(const std::function<void ()>& task,
                                        const std::chrono::steady_clock::time_point& target_time)
{
    loop_->PostTask(task, target_time);
}

void DelayedTaskRunner::PostTaskUntil(const std::function<void ()>& task,
                                        const std::chrono::milliseconds& delay)
{
    loop_->PostTask(task, std::chrono::steady_clock::now() + delay);
}

void DelayedTaskRunner::Terminate() {
    if (terminated_) {
        return;
    }

    PostTask([]() {
        DelayedMessageLoop::GetCurrent().Terminate();
    });
    terminated_ = true;
}

} // namespace wtf