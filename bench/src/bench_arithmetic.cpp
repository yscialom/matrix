#include <algorithm>
#include <array>

#include <benchmark/benchmark.h>

#include "matrix.hpp"

// ── double ───────────────────────────────────────────────────────────────────

static void BM_AddMatrices_Small(benchmark::State& state) {
    ysc::matrix<double, 4, 4> a{}, b{};
    for (auto _ : state)
        benchmark::DoNotOptimize(a + b);
}
BENCHMARK(BM_AddMatrices_Small);

static void BM_AddMatrices_StdArray_Small(benchmark::State& state) {
    std::array<double, 4 * 4> a{}, b{}, c{};
    for (auto _ : state) {
        std::transform(a.begin(), a.end(), b.begin(), c.begin(),
                       [](double x, double y) { return x + y; });
        benchmark::DoNotOptimize(c);
    }
}
BENCHMARK(BM_AddMatrices_StdArray_Small);

static void BM_AddMatrices(benchmark::State& state) {
    ysc::matrix<double, 64, 64> a{}, b{};
    for (auto _ : state)
        benchmark::DoNotOptimize(a + b);
}
BENCHMARK(BM_AddMatrices);

static void BM_AddMatrices_StdArray(benchmark::State& state) {
    std::array<double, 64 * 64> a{}, b{}, c{};
    for (auto _ : state) {
        std::transform(a.begin(), a.end(), b.begin(), c.begin(),
                       [](double x, double y) { return x + y; });
        benchmark::DoNotOptimize(c);
    }
}
BENCHMARK(BM_AddMatrices_StdArray);

static void BM_MulHadamard(benchmark::State& state) {
    ysc::matrix<double, 64, 64> a{}, b{};
    for (auto _ : state)
        benchmark::DoNotOptimize(a * b);
}
BENCHMARK(BM_MulHadamard);

static void BM_AddAssignMatrices(benchmark::State& state) {
    ysc::matrix<double, 64, 64> a{}, b{};
    for (auto _ : state) {
        a += b;
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_AddAssignMatrices);

static void BM_AddAssignMatrices_StdArray(benchmark::State& state) {
    std::array<double, 64 * 64> a{}, b{};
    for (auto _ : state) {
        std::transform(a.begin(), a.end(), b.begin(), a.begin(),
                       [](double x, double y) { return x + y; });
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_AddAssignMatrices_StdArray);

// ── int ──────────────────────────────────────────────────────────────────────

static void BM_AddMatrices_Int_Small(benchmark::State& state) {
    ysc::matrix<int, 4, 4> a{}, b{};
    for (auto _ : state)
        benchmark::DoNotOptimize(a + b);
}
BENCHMARK(BM_AddMatrices_Int_Small);

static void BM_AddMatrices_Int(benchmark::State& state) {
    ysc::matrix<int, 64, 64> a{}, b{};
    for (auto _ : state)
        benchmark::DoNotOptimize(a + b);
}
BENCHMARK(BM_AddMatrices_Int);

static void BM_AddMatrices_StdArray_Int(benchmark::State& state) {
    std::array<int, 64 * 64> a{}, b{}, c{};
    for (auto _ : state) {
        std::transform(a.begin(), a.end(), b.begin(), c.begin(),
                       [](int x, int y) { return x + y; });
        benchmark::DoNotOptimize(c);
    }
}
BENCHMARK(BM_AddMatrices_StdArray_Int);
