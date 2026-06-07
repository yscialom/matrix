#include <array>
#include <cstddef>

#include <benchmark/benchmark.h>

#include "matrix.hpp"

// ── operator() ── double, 2D ────────────────────────────────────────────────

static void BM_AccessOperator_Small(benchmark::State& state) {
    ysc::matrix<double, 4, 4> m{};
    for (auto _ : state)
        benchmark::DoNotOptimize(m(2, 2));
}
BENCHMARK(BM_AccessOperator_Small);

static void BM_AccessOperator_StdArray_Small(benchmark::State& state) {
    std::array<double, 4 * 4> a{};
    for (auto _ : state)
        benchmark::DoNotOptimize(
            a[2 * 4 + 2]); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
}
BENCHMARK(BM_AccessOperator_StdArray_Small);

static void BM_AccessOperator(benchmark::State& state) {
    ysc::matrix<double, 64, 64> m{};
    for (auto _ : state)
        benchmark::DoNotOptimize(m(32, 32));
}
BENCHMARK(BM_AccessOperator);

static void BM_AccessAt(benchmark::State& state) {
    ysc::matrix<double, 64, 64> m{};
    for (auto _ : state)
        benchmark::DoNotOptimize(m.at(32, 32));
}
BENCHMARK(BM_AccessAt);

static void BM_AccessOperator_StdArray(benchmark::State& state) {
    std::array<double, 64 * 64> a{};
    for (auto _ : state)
        benchmark::DoNotOptimize(
            a[32 * 64 + 32]); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
}
BENCHMARK(BM_AccessOperator_StdArray);

// ── operator() ── int ───────────────────────────────────────────────────────

static void BM_AccessOperator_Int_Small(benchmark::State& state) {
    ysc::matrix<int, 4, 4> m{};
    for (auto _ : state)
        benchmark::DoNotOptimize(m(2, 2));
}
BENCHMARK(BM_AccessOperator_Int_Small);

static void BM_AccessOperator_StdArray_Int_Small(benchmark::State& state) {
    std::array<int, 4 * 4> a{};
    for (auto _ : state)
        benchmark::DoNotOptimize(
            a[2 * 4 + 2]); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
}
BENCHMARK(BM_AccessOperator_StdArray_Int_Small);

static void BM_AccessOperator_Int(benchmark::State& state) {
    ysc::matrix<int, 64, 64> m{};
    for (auto _ : state)
        benchmark::DoNotOptimize(m(32, 32));
}
BENCHMARK(BM_AccessOperator_Int);

static void BM_AccessOperator_StdArray_Int(benchmark::State& state) {
    std::array<int, 64 * 64> a{};
    for (auto _ : state)
        benchmark::DoNotOptimize(
            a[32 * 64 + 32]); // NOLINT(cppcoreguidelines-pro-bounds-constant-array-index)
}
BENCHMARK(BM_AccessOperator_StdArray_Int);

// ── iterate ─────────────────────────────────────────────────────────────────

static void BM_IterateRangeFor(benchmark::State& state) {
    ysc::matrix<double, 64, 64> m{};
    for (auto _ : state) {
        double sum = 0.0;
        for (const auto& v : m)
            sum += v;
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_IterateRangeFor);

static void BM_IterateRangeFor_StdArray(benchmark::State& state) {
    std::array<double, 64 * 64> a{};
    for (auto _ : state) {
        double sum = 0.0;
        for (const auto& v : a)
            sum += v;
        benchmark::DoNotOptimize(sum);
    }
}
BENCHMARK(BM_IterateRangeFor_StdArray);

static void BM_IterateIndex(benchmark::State& state) {
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
