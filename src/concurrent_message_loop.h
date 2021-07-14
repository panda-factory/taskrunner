//
// Created by guozhenxiong on 2021-07-14.
//

#ifndef TASKRUNNER_CONCURRENT_MESSAGE_LOOP_H
#define TASKRUNNER_CONCURRENT_MESSAGE_LOOP_H

#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <map>

namespace wtf {

class ConcurrentMessageLoop {
public:
    bool HasThreadTasksLocked() const;

    std::vector<std::function<void ()>> GetThreadTasksLocked();

    void PostTask(const std::function<void ()>& task);

    void PostTaskToWorker(std::thread::id thread_id, const std::function<void ()>& task);

    void NotifyAll();

    void Terminate();

    void WorkerMain();

    ConcurrentMessageLoop() = default;
private:
    std::mutex tasks_mutex_;
    bool shutdown_ = false;
    std::queue<std::function<void ()>> tasks_;
    std::condition_variable tasks_condition_;
    std::map<std::thread::id, std::vector<std::function<void ()>>> thread_tasks_;
};

} // namespace wtf



#endif //TASKRUNNER_CONCURRENT_MESSAGE_LOOP_H
