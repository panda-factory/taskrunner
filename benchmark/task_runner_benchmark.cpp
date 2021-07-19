//
// Created by guozhenxiong on 2021-07-15.
//
#include "benchmark/benchmark.h"
#include <memory>
#include <string>
#include <atomic>

#include "task_runner/task_runner.h"
#include "task_runner/delayed_task_runner.h"
#include "task_runner/concurrent_task_runner.h"

constexpr size_t kTotalCount = 10000;

void PostTask(wtf::TaskRunner* task_runner, size_t& current)
{
    for (size_t i = 0; i < kTotalCount; i++) {
        task_runner->PostTask([&current]() {
            current++;
        });
    }
}

static void BM_TaskRunner(benchmark::State& state) {  // NOLINT
    size_t current = 0;
    auto task_runner = wtf::TaskRunner::Create("task-runner");
    for (auto _ : state) {
        // This code gets timed
        PostTask(task_runner.get(), current);
    }
}

static void BM_DelayedTaskRunner(benchmark::State& state) {  // NOLINT
    size_t current = 0;
    auto task_runner = wtf::DelayedTaskRunner::CreateTaskRunner("delayed-task-runner");
    for (auto _ : state) {
        // This code gets timed
        PostTask(task_runner.get(), current);
    }
}

static void BM_ConcurrentTaskRunner(benchmark::State& state) {  // NOLINT
    size_t current = 0;
    auto task_runner = wtf::ConcurrentTaskRunner::Create();
    for (auto _ : state) {
        // This code gets timed
        PostTask(task_runner.get(), current);
    }
}

// Run the benchmark
BENCHMARK(BM_TaskRunner);
BENCHMARK(BM_DelayedTaskRunner);
BENCHMARK(BM_ConcurrentTaskRunner);
BENCHMARK_MAIN();