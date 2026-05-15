#include <array>
#include <cstddef>

#include <benchmark/benchmark.h>

#include "matrix.hpp"

static void BM_AccessOperator(benchmark::State& state)
{
    ysc::matrix<double, 64, 64> m{};
    for (auto _ : state)
        benchmark::DoNotOptimize(m(32, 32));
}
BENCHMARK(BM_AccessOperator);

static void BM_AccessAt(benchmark::State& state)
{
    ysc::matrix<double, 64, 64> m{};
    for (auto _ : state)
        benchmark::DoNotOptimize(m.at(32, 32));
}
BENCHMARK(BM_AccessAt);

static void BM_AccessOperator_StdArray(benchmark::State& state)
{
    std::array<double, 64 * 64> a{};
    for (auto _ : state)
        benchmark::DoNotOptimize(a[32 * 64 + 32]); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
}
BENCHMARK(BM_AccessOperator_StdArray);

static void BM_IterateRangeFor(benchmark::State& state)
{
    ysc::matrix<double, 64, 64> m{};
    for (auto _ : state) {
        double sum = 0.0;
        for (const auto& v : m)
            sum += v;
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_IterateRangeFor);

static void BM_IterateIndex(benchmark::State& state)
{
    ysc::matrix<double, 64, 64> m{};
    for (auto _ : state) {
        double sum = 0.0;
        for (std::size_t i = 0; i < 64; ++i)
            for (std::size_t j = 0; j < 64; ++j)
                sum += m(i, j);
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_IterateIndex);
