//
// Created by guozhenxiong on 2021-07-15.
//
#include "benchmark/benchmark.h"
#include <memory>
#include <string>
#include <atomic>

#include "task_runner/task_runner.h"
#include "task_runner/delayed_task_runner.h"
void foo()
{
    ;
}

static void BM_TASK_RUNNER(benchmark::State& state) {  // NOLINT
    auto task_runner = wtf::TaskRunner::Create("task-runner");
    std::atomic_int count = 0;
    for (auto _ : state) {
        // This code gets timed
        task_runner->PostTask([&count]() {
            count++;
        });
    }
}
static void BM_DELAYED_TASK_RUNNER(benchmark::State& state) {  // NOLINT
    auto task_runner = wtf::DelayedTaskRunner::CreateTaskRunner("task-runner");
    std::atomic_int count = 0;
    for (auto _ : state) {
        // This code gets timed
        task_runner->PostTask([&count]() {
            count++;
        });
    }
}

// Run the benchmark
BENCHMARK(BM_TASK_RUNNER);
BENCHMARK(BM_DELAYED_TASK_RUNNER);
BENCHMARK_MAIN();