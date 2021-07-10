//
// Created by guozhenxiong on 2021-07-02.
//

#ifndef TASKRUNNER_THREAD_H
#define TASKRUNNER_THREAD_H

#include <atomic>
#include <memory>
#include <thread>

#include "task_runner.h"

namespace wtf {

class Thread {
public:
    explicit Thread(const std::string& name = "");

    ~Thread();

    TaskRunner& GetTaskRunner() const;

    void Join();

    static void SetCurrentThreadName(const std::string& name);

private:
    std::unique_ptr<std::thread> thread_;
    wtf::TaskRunner* task_runner_;
    std::atomic_bool joined_;

    Thread(const Thread&) = delete;
    Thread& operator=(const Thread&) = delete;

};
} // namespace wtf

#endif //TASKRUNNER_THREAD_H
