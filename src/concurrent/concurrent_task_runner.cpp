//
// Created by guozhenxiong on 2021-07-14.
//
#include "task_runner/concurrent_task_runner.h"

#include <memory>

#include "concurrent_message_loop.h"
#include "logging/logging.h"

namespace wtf {

// | static |
std::shared_ptr<ConcurrentTaskRunner> ConcurrentTaskRunner::Create(
        size_t worker_count) {
    return std::make_shared<ConcurrentTaskRunner>(worker_count);
}

ConcurrentTaskRunner::ConcurrentTaskRunner(size_t worker_count)
        : worker_count_(std::max<size_t>(worker_count, 1ul)), joined_(false) {
    loop_ = std::make_unique<ConcurrentMessageLoop>();
    for (size_t i = 0; i < worker_count_; ++i) {
        workers_.emplace_back([i, loop = loop_.get()]() {
            wtf::TaskRunner::SetCurrentThreadName(
                    std::string{"worker." + std::to_string(i + 1)});

            loop->Run();
        });
    }

    for (const auto& worker : workers_) {
        worker_thread_ids_.emplace_back(worker.get_id());
    }
}

ConcurrentTaskRunner::~ConcurrentTaskRunner() {
    Terminate();
    Join();
}

void ConcurrentTaskRunner::AddTaskObserver(intptr_t key, const std::function<void ()>& callback)
{}

void ConcurrentTaskRunner::RemoveTaskObserver(intptr_t key)
{}

void ConcurrentTaskRunner::PostTask(const std::function<void ()>& task) {
    if (!task) {
        return;
    }

    loop_->PostTask(task);
    return;
}

void ConcurrentTaskRunner::Join()
{
    if (joined_) {
        return;
    }
    joined_ = true;
    for (auto& worker : workers_) {
        worker.join();
    }
}

void ConcurrentTaskRunner::Terminate()
{
    loop_->Terminate();
    terminated_ = true;
}

void ConcurrentTaskRunner::PostTaskToAllWorkers(const std::function<void ()>& task) {
    if (!task) {
        return;
    }

    for (const auto& worker_thread_id : worker_thread_ids_) {
        loop_->PostTaskToWorker(worker_thread_id, task);
    }
    loop_->NotifyAll();
}

} // namespace wtf