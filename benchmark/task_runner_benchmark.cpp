//
// Created by guozhenxiong on 2021-07-15.
//
#include "benchmark/benchmark.h"


static void BM_RunTaskForAdd10000(benchmark::State& state) {  // NOLINT
    for (auto _ : state) {
        // This code gets timed
        ;
    }
}

// Run the benchmark
BENCHMARK(BM_RunTaskForAdd10000);
BENCHMARK_MAIN();