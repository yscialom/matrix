#include <benchmark/benchmark.h>

#include "matrix.hpp"

static void BM_AddMatrices(benchmark::State& state)
{
    ysc::matrix<double, 64, 64> a{}, b{};
    for (auto _ : state)
        benchmark::DoNotOptimize(a + b);
}
BENCHMARK(BM_AddMatrices);

static void BM_MulHadamard(benchmark::State& state)
{
    ysc::matrix<double, 64, 64> a{}, b{};
    for (auto _ : state)
        benchmark::DoNotOptimize(a * b);
}
BENCHMARK(BM_MulHadamard);

static void BM_AddAssignMatrices(benchmark::State& state)
{
    ysc::matrix<double, 64, 64> a{}, b{};
    for (auto _ : state) {
        a += b;
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_AddAssignMatrices);
