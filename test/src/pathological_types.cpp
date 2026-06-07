#include "utils.hpp"
#include <matrix.hpp>

#include <gtest/gtest.h>

#include <functional>
#include <memory>
#include <stdexcept>

// Type whose copy constructor/assignment always throw
struct throwing_copy {
    int val{};
    throwing_copy() = default;
    explicit throwing_copy(int v) : val(v) {}
    throwing_copy(const throwing_copy& /*unused*/) { throw std::runtime_error("copy"); }
    throwing_copy& operator=(const throwing_copy& /*unused*/) {
        throw std::runtime_error("copy=");
        return *this;
    }
    throwing_copy(throwing_copy&&) noexcept = default;
    throwing_copy& operator=(throwing_copy&&) noexcept = default;
    ~throwing_copy() = default;
    bool operator==(const throwing_copy&) const = default;
};

// Type without operator==: matrix<no_eq> is constructible but not equality-comparable.
// Note: a static_assert(!is_invocable_v<equal_to<>, matrix<no_eq,2>, matrix<no_eq,2>>) would
// trigger a hard error inside std::array::operator== (which uses std::equal, not = default) —
// not SFINAE-friendly on GCC/libstdc++. The compilation refusal is verified manually: attempting
// to compare two matrix<no_eq,2> values fails to compile, as expected by design.
struct no_eq {
    // NOLINTNEXTLINE(cppcoreguidelines-special-member-functions)
    bool operator==(const no_eq&) = delete;
};

//
// --- MOVE-ONLY TYPE (std::unique_ptr<int>) ---
//

TEST(pathological_move_only, default_construction) {
    ysc::matrix<std::unique_ptr<int>, 3> m;
    EXPECT_EQ(m(0), nullptr);
    EXPECT_EQ(m(2), nullptr);
}

TEST(pathological_move_only, element_access_and_assignment) {
    ysc::matrix<std::unique_ptr<int>, 3> m;
    m(0) = std::make_unique<int>(42);
    m(1) = std::make_unique<int>(7);
    EXPECT_EQ(*m(0), 42);
    EXPECT_EQ(*m(1), 7);
    EXPECT_EQ(m(2), nullptr);
}

TEST(pathological_move_only, move_construction) {
    ysc::matrix<std::unique_ptr<int>, 2> src;
    src(0) = std::make_unique<int>(10);
    src(1) = std::make_unique<int>(20);
    ysc::matrix<std::unique_ptr<int>, 2> dst = std::move(src);
    EXPECT_EQ(*dst(0), 10);
    EXPECT_EQ(*dst(1), 20);
}

TEST(pathological_move_only, move_assignment) {
    ysc::matrix<std::unique_ptr<int>, 2> src;
    src(0) = std::make_unique<int>(99);
    ysc::matrix<std::unique_ptr<int>, 2> dst;
    dst = std::move(src);
    EXPECT_EQ(*dst(0), 99);
}

TEST(pathological_move_only, range_iteration) {
    ysc::matrix<std::unique_ptr<int>, 3> m;
    m(0) = std::make_unique<int>(1);
    m(1) = std::make_unique<int>(2);
    m(2) = std::make_unique<int>(3);
    int sum = 0;
    for (const auto& ptr : m) {
        sum += *ptr;
    }
    EXPECT_EQ(sum, 6);
}

TEST(pathological_move_only, size_and_data) {
    ysc::matrix<std::unique_ptr<int>, 4> m;
    static_assert(decltype(m)::size() == 4);
    EXPECT_FALSE(m.empty());
    EXPECT_EQ(m.data(), &m(0));
}

TEST(pathological_move_only, equality) {
    // Two default-constructed matrices of null unique_ptrs must compare equal
    ysc::matrix<std::unique_ptr<int>, 2> m1;
    ysc::matrix<std::unique_ptr<int>, 2> m2;
    EXPECT_EQ(m1, m2);
}

//
// --- TYPE WITH THROWING COPY ---
//

TEST(pathological_throwing_copy, default_construction) {
    ysc::matrix<throwing_copy, 3> m;
    EXPECT_EQ(m(0).val, 0);
    EXPECT_EQ(m(2).val, 0);
}

TEST(pathological_throwing_copy, move_construction) {
    ysc::matrix<throwing_copy, 2> src;
    src(0).val = 5;
    src(1).val = 6;
    ysc::matrix<throwing_copy, 2> dst = std::move(src);
    EXPECT_EQ(dst(0).val, 5);
    EXPECT_EQ(dst(1).val, 6);
}

TEST(pathological_throwing_copy, copy_propagates_exception) {
    using tc3 = ysc::matrix<throwing_copy, 3>;
    tc3 src;
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    EXPECT_THROW(tc3 copy(src), std::runtime_error);
}

//
// --- TYPE WITHOUT EQUALITY OPERATOR ---
//

TEST(pathological_no_eq, construction_and_access) {
    ysc::matrix<no_eq, 2> m;
    EXPECT_EQ(m.size(), 2U);
}
