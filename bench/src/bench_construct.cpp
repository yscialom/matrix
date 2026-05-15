#include <array>

#include <benchmark/benchmark.h>

#include "matrix.hpp"

static void BM_ConstructDefault(benchmark::State& state)
{
    for (auto _ : state)
        benchmark::DoNotOptimize(ysc::matrix<double, 4, 4>{});
}
BENCHMARK(BM_ConstructDefault);

static void BM_ConstructZeros(benchmark::State& state)
{
    for (auto _ : state)
        benchmark::DoNotOptimize(ysc::matrix<double, 4, 4>(ysc::zero));
}
BENCHMARK(BM_ConstructZeros);

static void BM_ConstructDefault_StdArray(benchmark::State& state)
{
    for (auto _ : state)
        benchmark::DoNotOptimize(std::array<double, 16>{});
}
BENCHMARK(BM_ConstructDefault_StdArray);
