//
// Created by guozhenxiong on 2021-07-14.
//

#ifndef TASKRUNNER_CONCURRENT_TASK_RUNNER_H
#define TASKRUNNER_CONCURRENT_TASK_RUNNER_H

#include "task_runner.h"

namespace wtf {
class ConcurrentMessageLoop;

class WTF_DLL ConcurrentTaskRunner : public TaskRunner {
public:
    static std::shared_ptr<ConcurrentTaskRunner> Create(
            size_t worker_count = std::thread::hardware_concurrency());

    // | TaskRunner |
    void PostTask(const std::function<void ()>& task) override;

    // | TaskRunner |
    void AddTaskObserver(intptr_t key, const std::function<void ()>& callback) override;

    // | TaskRunner |
    void RemoveTaskObserver(intptr_t key) override;

    // | TaskRunner |
    void Join() override;

    // | TaskRunner |
    void Terminate() override;

    void PostTaskToAllWorkers(const std::function<void ()>& task);

    ConcurrentTaskRunner(size_t worker_count);

    ~ConcurrentTaskRunner();

private:
    friend ConcurrentMessageLoop;

    std::unique_ptr<ConcurrentMessageLoop> loop_;

    size_t worker_count_ = 0;

    std::vector<std::thread> workers_;

    std::vector<std::thread::id> worker_thread_ids_;

    std::atomic_bool joined_;

    ConcurrentTaskRunner(const ConcurrentTaskRunner&) = delete;

    ConcurrentTaskRunner& operator=(const ConcurrentTaskRunner&) = delete;
};
} // namespace wtf
#endif //TASKRUNNER_CONCURRENT_TASK_RUNNER_H
