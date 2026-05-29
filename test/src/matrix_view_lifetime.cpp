/**
 * @file matrix_view_lifetime.cpp
 * @brief ASan test: dangling matrix_view use-after-free detection.
 *
 * This test verifies that AddressSanitizer correctly detects a heap-use-after-free
 * when a @c matrix_view outlives the @c matrix it refers to.
 *
 * The matrix is allocated on the heap (via @c new) so that ASan reliably poisons
 * the freed memory and detects the subsequent read through the dangling view.
 *
 * @note The death-test style is set to @c "threadsafe" so that the child process
 *       is spawned via @c exec() rather than @c fork().  This is required because
 *       ASan's shadow memory is not correctly inherited across a plain @c fork(),
 *       which would prevent the heap-use-after-free from being detected.
 *
 * Compiled in all configurations; the test body only runs under ASan
 * (@c YSC_SANITIZERS_ENABLED defined).  Without sanitizers the test is
 * skipped to avoid invoking undefined behavior in the normal build.
 */
#include <matrix.hpp>
#include <matrix_view.hpp>

#include <gtest/gtest.h>

// Under ASan, accessing a dangling matrix_view must terminate the process.
// EXPECT_DEATH forks a child process; ASan detects the heap-use-after-free in
// the child and aborts it, which satisfies the "death" predicate.
// The matrix is allocated on the heap so that ASan poisons the freed region.
// The "threadsafe" style spawns via exec() so ASan initialises cleanly.
TEST(MatrixViewLifetime, DanglingViewUseAfterFree) {
#ifndef YSC_SANITIZERS_ENABLED
    GTEST_SKIP() << "Test only meaningful under ASan (build with ENABLE_SANITIZERS=ON)";
#else
    ::testing::FLAGS_gtest_death_test_style = "threadsafe";
    auto trigger_uaf = [] {
        // Allocate on the heap so that ASan reliably poisons the freed memory.
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        auto* m = new ysc::matrix<int, 3>{1, 2, 3};
        ysc::matrix_view<int, ysc::contiguous, 3> view{*m};
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        delete m;
        // The matrix has been freed; accessing the view is heap-use-after-free.
        // NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
        volatile int val = view(0);
        (void)val;
    };
    EXPECT_DEATH(trigger_uaf(), "");
#endif
}
