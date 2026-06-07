#include <array>

#include <benchmark/benchmark.h>

#include "matrix.hpp"

// ── fill ─────────────────────────────────────────────────────────────────────

static void BM_Fill_Small(benchmark::State& state) {
    ysc::matrix<double, 4, 4> m{};
    for (auto _ : state) {
        m.fill(1.0);
        benchmark::DoNotOptimize(m);
    }
}
BENCHMARK(BM_Fill_Small);

static void BM_Fill_StdArray_Small(benchmark::State& state) {
    std::array<double, 4 * 4> a{};
    for (auto _ : state) {
        a.fill(1.0);
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_Fill_StdArray_Small);

static void BM_Fill(benchmark::State& state) {
    ysc::matrix<double, 64, 64> m{};
    for (auto _ : state) {
        m.fill(1.0);
        benchmark::DoNotOptimize(m);
    }
}
BENCHMARK(BM_Fill);

static void BM_Fill_StdArray(benchmark::State& state) {
    std::array<double, 64 * 64> a{};
    for (auto _ : state) {
        a.fill(1.0);
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_Fill_StdArray);

static void BM_Fill_Int(benchmark::State& state) {
    ysc::matrix<int, 64, 64> m{};
    for (auto _ : state) {
        m.fill(42);
        benchmark::DoNotOptimize(m);
    }
}
BENCHMARK(BM_Fill_Int);

static void BM_Fill_StdArray_Int(benchmark::State& state) {
    std::array<int, 64 * 64> a{};
    for (auto _ : state) {
        a.fill(42);
        benchmark::DoNotOptimize(a);
    }
}
BENCHMARK(BM_Fill_StdArray_Int);

// ── swap ─────────────────────────────────────────────────────────────────────

static void BM_Swap_Small(benchmark::State& state) {
    ysc::matrix<double, 4, 4> a{}, b{};
    a.fill(1.0);
    for (auto _ : state) {
        a.swap(b);
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
    }
}
BENCHMARK(BM_Swap_Small);

static void BM_Swap_StdArray_Small(benchmark::State& state) {
    std::array<double, 4 * 4> a{}, b{};
    a.fill(1.0);
    for (auto _ : state) {
        a.swap(b);
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
    }
}
BENCHMARK(BM_Swap_StdArray_Small);

static void BM_Swap(benchmark::State& state) {
    ysc::matrix<double, 64, 64> a{}, b{};
    a.fill(1.0);
    for (auto _ : state) {
        a.swap(b);
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
    }
}
BENCHMARK(BM_Swap);

static void BM_Swap_StdArray(benchmark::State& state) {
    std::array<double, 64 * 64> a{}, b{};
    a.fill(1.0);
    for (auto _ : state) {
        a.swap(b);
        benchmark::DoNotOptimize(a);
        benchmark::DoNotOptimize(b);
    }
}
BENCHMARK(BM_Swap_StdArray);
