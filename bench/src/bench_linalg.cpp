#include <benchmark/benchmark.h>

#include "matrix.hpp"

static void BM_Matmul4x4(benchmark::State& state)
{
    ysc::matrix<double, 4, 4> a{}, b{};
    for (auto _ : state)
        benchmark::DoNotOptimize(ysc::matmul(a, b));
}
BENCHMARK(BM_Matmul4x4);

static void BM_Matmul16x16(benchmark::State& state)
{
    ysc::matrix<double, 16, 16> a{}, b{};
    for (auto _ : state)
        benchmark::DoNotOptimize(ysc::matmul(a, b));
}
BENCHMARK(BM_Matmul16x16);

static void BM_Transpose4x4(benchmark::State& state)
{
    ysc::matrix<double, 4, 4> m{};
    for (auto _ : state)
        benchmark::DoNotOptimize(ysc::transpose(m));
}
BENCHMARK(BM_Transpose4x4);

static void BM_DotProduct(benchmark::State& state)
{
    ysc::matrix<double, 256> a{}, b{};
    for (auto _ : state)
        benchmark::DoNotOptimize(ysc::dot(a, b));
}
BENCHMARK(BM_DotProduct);
