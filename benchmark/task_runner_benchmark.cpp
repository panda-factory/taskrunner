//
// Created by guozhenxiong on 2021-07-15.
//
#include "benchmark/benchmark.h"
#include <memory>
#include <string>
static void BM_SHARED_PTR(benchmark::State& state) {  // NOLINT
    std::shared_ptr<std::string> shard_ptr = std::make_shared<std::string>();
    for (auto _ : state) {
        // This code gets timed
        shard_ptr->empty();
    }
}
static void BM_UNIQUE_PTR(benchmark::State& state) {  // NOLINT
    std::unique_ptr<std::string> unique_ptr = std::make_unique<std::string>();
    for (auto _ : state) {
        // This code gets timed
        unique_ptr->empty();
    }
}
static void BM_RAW_PTR(benchmark::State& state) {  // NOLINT
    std::string* raw_ptr = new std::string();
    for (auto _ : state) {
        // This code gets timed
        raw_ptr->empty();
    }
}

// Run the benchmark
BENCHMARK(BM_SHARED_PTR);
BENCHMARK(BM_UNIQUE_PTR);
BENCHMARK(BM_RAW_PTR);
BENCHMARK_MAIN();