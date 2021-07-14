//
// Created by guozhenxiong on 2021-07-14.
//

#include "concurrent_message_loop.h"

#include "logging/logging.h"

namespace wtf {

bool ConcurrentMessageLoop::HasThreadTasksLocked() const {
    return thread_tasks_.count(std::this_thread::get_id()) > 0;
}

std::vector<std::function<void ()>> ConcurrentMessageLoop::GetThreadTasksLocked() {
    auto found = thread_tasks_.find(std::this_thread::get_id());
    WTF_DCHECK(found != thread_tasks_.end());
    std::vector<std::function<void ()>> pending_tasks;
    std::swap(pending_tasks, found->second);
    thread_tasks_.erase(found);
    return pending_tasks;
}

void ConcurrentMessageLoop::PostTask(const std::function<void ()>& task) {
    if (!task) {
        return;
    }

    std::unique_lock lock(tasks_mutex_);

    // Don't just drop tasks on the floor in case of shutdown.
    if (shutdown_) {
        WTF_DLOG(WARNING)
                << "Tried to post a task to shutdown concurrent message "
                   "loop. The task will be executed on the callers thread.";
        lock.unlock();
        task();
        return;
    }

    tasks_.push(task);

    lock.unlock();

    tasks_condition_.notify_one();
}

void ConcurrentMessageLoop::PostTaskToWorker(std::thread::id worker_thread_id, const std::function<void ()>& task)
{
    if (!task) {
        return;
    }

    std::scoped_lock lock(tasks_mutex_);
    thread_tasks_[worker_thread_id].emplace_back(task);
}

void ConcurrentMessageLoop::NotifyAll()
{
    std::scoped_lock lock(tasks_mutex_);
    tasks_condition_.notify_all();
}

void ConcurrentMessageLoop::Terminate() {
    std::scoped_lock lock(tasks_mutex_);
    if (!shutdown_) {
        shutdown_ = true;
        tasks_condition_.notify_all();
    }
}

void ConcurrentMessageLoop::Run() {
    while (true) {
        std::unique_lock lock(tasks_mutex_);
        tasks_condition_.wait(lock, [&]() {
            return tasks_.size() > 0 || shutdown_ || HasThreadTasksLocked();
        });

        // Shutdown cannot be read with the task mutex unlocked.
        bool shutdown_now = shutdown_;
        std::function<void ()> task;
        std::vector<std::function<void ()>> thread_tasks;

        if (tasks_.size() != 0) {
            task = tasks_.front();
            tasks_.pop();
        }

        if (HasThreadTasksLocked()) {
            thread_tasks = GetThreadTasksLocked();
            WTF_DCHECK(!HasThreadTasksLocked());
        }

        lock.unlock();

        if (task) {
            task();
        }

        // Execute any thread tasks.
        for (const auto& thread_task : thread_tasks) {
            thread_task();
        }

        if (shutdown_now) {
            break;
        }
    }
}
} // namespace wtf