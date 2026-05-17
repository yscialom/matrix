/**
 * @file matrix.hpp
 * @author Yankel Scialom (YSC) <yankel-pro@scialom.org>
 * @date 2019
 *
 * @copyright This project is released under GNU Lesser General Public License;
 * see COPYING and COPYING.LESSER files attached.
 *
 * The matrix library is a header-only template library defining a
 * general-purpose multi-dimension container of static dimensions.
 */
#ifndef YSC_MATRIX_HPP
#define YSC_MATRIX_HPP

#include <algorithm>
#include <array>
#include <compare>
#include <concepts>
#include <functional>
#include <iterator>
#include <numeric>
#include <ostream>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
// Clang < 17 cannot compile libstdc++-14's <format> due to unicode.h
// incompatibility
#if __has_include(<format>) && (!defined(__clang__) || __clang_major__ >= 17)
#include <format>
#include <sstream>
#endif

#include <matrix_detail.hpp>
#include <matrix_view.hpp>

namespace ysc {
namespace detail {

template <class T>
concept ostream_streamable = requires(std::ostream& os, const T& v) { os << v; };

// Print a hyperslice of a matrix starting at `it`, covering dimension `dim_idx`
// onward. Returns an iterator past the last element printed.
// NOLINTNEXTLINE(misc-no-recursion)
template <class It, class Dims>
It print_recursive(std::ostream& os, It it, const Dims& dims, std::size_t dim_idx) {
    os << '[';
    const std::size_t count = dims[dim_idx];
    const bool last_dim = (dim_idx + 1 == dims.size());
    for (std::size_t i = 0; i < count; ++i) {
        if (i > 0) {
            os << ", ";
        }
        if (last_dim) {
            os << *it++;
        } else {
            // NOLINTNEXTLINE(misc-no-recursion)
            it = print_recursive(os, it, dims, dim_idx + 1);
        }
    }
    os << ']';
    return it;
}

} // namespace detail

/**
 * @brief Satisfied when `U` is convertible to `T`.
 * Used to constrain converting constructors and assignment operators.
 */
template <class T, class U>
concept matrix_convertible_from = std::convertible_to<U, T>;

/**
 * @brief Tag a @c matrix object to be zero-initialized.
 * @see matrix::matrix(matrix_zero_t)
 */
constexpr struct matrix_zero_t {
} zero;

/**
 * @brief Multi-dimensional container encapsulating a fixed size matrix.
 * @tparam T          Element type
 * @tparam Dimentions Dimensions of the matrix
 *
 * `matrix<T, 2, 5, 9>` is an order 3  matrix of @c T elements; its dimensions
 * are 2 by 5 by 9 (90 @c T elements in total).
 *
 * This container is a class type with the semantics of an aggregate similar to
 * a struct holding a C-style array `T[Dimensions][...]` as its only non-static
 * data member. Unlike a C-style array, it doesn't decay to `T*` automatically.
 * As an aggregate impersonator, it can be initialized with
 * aggregate-initialization given exactly @c linear_size initializers that are
 * convertible to @c T: `ysc::matrix<int, 3, 2> m = {1,2,3,4,5,6};`.
 *
 * The struct combines the performance and accessibility of a C-style array with
 * the benefits of a standard container, such as knowing its own size,
 * supporting assignment, random access iterators, etc.
 *
 * @todo Requirements (Container, etc.)
 *
 * @todo Special case when one dimension is 0.
 *
 * ### Iterator invalidation
 * As a rule, iterators to aa matrix are never invalidated throughout the
 * lifetime of the matrix. One should take note, however, that during swap, the
 * iterator will continue to point to the same matrix element, and will thus
 * change its value.
 */
template <class T, std::size_t... Dimensions> class matrix {
    template <class, std::size_t...> friend class matrix;

public:
    /** @brief Order of the matrix (2D matrix have order 2, 3D order 3, etc.).
     */
    static constexpr std::size_t order = sizeof...(Dimensions);
    /** @brief Dimensions of the matrix. An order-`N` matrix has `N` dimensions.
     */
    static constexpr std::array dimensions = {Dimensions...};

private:
    static constexpr std::size_t linear_size = (Dimensions * ...);
    std::array<T, linear_size> _data;

public:
    /** @brief Element type. */
    using value_type = T;
    /** @brief Unsigned integer type for sizes and counts. */
    using size_type = std::size_t;
    /** @brief Signed integer type for differences between iterators. */
    using difference_type = std::ptrdiff_t;
    /** @brief Reference to element type. */
    using reference = T&;
    /** @brief Const reference to element type. */
    using const_reference = const T&;
    /** @brief Pointer to element type. */
    using pointer = T*;
    /** @brief Const pointer to element type. */
    using const_pointer = const T*;
    /** @brief Iterator over matrix elements in row-major order. */
    using iterator = typename std::array<T, linear_size>::iterator;
    /** @brief Const iterator over matrix elements in row-major order. */
    using const_iterator = typename std::array<T, linear_size>::const_iterator;
    /** @brief Reverse iterator. */
    using reverse_iterator = std::reverse_iterator<iterator>;
    /** @brief Const reverse iterator. */
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // iterators
    constexpr iterator begin() noexcept { return _data.begin(); }
    [[nodiscard]] constexpr const_iterator begin() const noexcept { return _data.begin(); }
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return _data.cbegin(); }
    constexpr iterator end() noexcept { return _data.end(); }
    [[nodiscard]] constexpr const_iterator end() const noexcept { return _data.end(); }
    [[nodiscard]] constexpr const_iterator cend() const noexcept { return _data.cend(); }
    constexpr reverse_iterator rbegin() noexcept { return _data.rbegin(); }
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
        return _data.rbegin();
    }
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept {
        return _data.crbegin();
    }
    constexpr reverse_iterator rend() noexcept { return _data.rend(); }
    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept { return _data.rend(); }
    [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept { return _data.crend(); }

    // capacity
    /**
     * @brief Returns the number of elements in the matrix (product of all
     * dimensions).
     *
     * @note This function is @c static because the size is a compile-time
     * constant.
     */
    static constexpr size_type size() noexcept { return linear_size; }

    /**
     * @brief Returns the maximum number of elements the matrix can hold.
     *
     * Always equal to @c size() for this fixed-size container.
     *
     * @note This function is @c static because the value is a compile-time
     * constant.
     */
    static constexpr size_type max_size() noexcept { return linear_size; }

    /**
     * @brief Returns whether the matrix has no elements.
     *
     * @note This function is @c static because the value is a compile-time
     * constant.
     */
    static constexpr bool empty() noexcept { return linear_size == 0; }

    /**
     * @brief Returns a pointer to the underlying element storage.
     *
     * Elements are stored in row-major order (rightmost dimension is
     * contiguous).
     */
    constexpr pointer data() noexcept { return _data.data(); }

    /**
     * @brief Returns a pointer to the underlying element storage.
     *
     * Elements are stored in row-major order (rightmost dimension is
     * contiguous).
     */
    [[nodiscard]] constexpr const_pointer data() const noexcept { return _data.data(); }

    /**
     * @brief Exchanges the given values.
     * @param lhs value to be swapped
     * @param rhs value to be swapped
     *
     * Swaps the elements of @a lhs and @a rhs as if:
     * @code
     using std::swap;
     for (auto lhs_it = lhs.begin(), auto rhs_it = rhs.begin() ; lhs_it !=
     lhs.end() ; ++lhs_it,
     ++rhs_it) { swap(*lhs_it, *rhs_it);
     }
     @endcode
     */
    friend void swap(matrix& lhs, matrix& rhs) noexcept(std::is_nothrow_swappable_v<T>) {
        using std::swap;
        swap(lhs._data, rhs._data);
    }

    // default constructor
    /**
     * @brief Initializes the matrix.
     *
     * @note If `T` is a trivial type, initialization may result in
     * indeterminate values.
     */
    // Intentional: _data is deliberately left uninitialized for trivial T to
    // avoid the cost of zero-initialization on the hot path. Use
    // matrix(matrix_zero_t) for zero-initialization.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
    matrix() = default;

    /**
     * @brief Initializes the matrix following the rules of default
     * initialization.
     *
     * @note If `T` is a trivial type, the matrix is zero-initialized; otherwise
     * the default constructors of its elements are called.
     */
    constexpr matrix(matrix_zero_t /*zero*/) : _data({}) {}

    // aggregate constructors
    /**
     * @brief Initializes the matrix following the rules of aggregate
     * initialization.
     * @tparam Args... Source types (must all be convertible to @c T)
     * @param args...  Source values (must be exactly @c linear_size values)
     *
     * `matrix<long, 2, 2> m{true, '\x02', 3, 4L}` initializes an order-2 matrix
     * from the values `true`, `'\x02'`, `3` and `4L` converted to `long`.
     * Partial initialization (fewer than `linear_size` values) is not
     * supported; use `matrix(matrix_zero_t)` to zero-initialize.
     */
    template <class... Args>
        requires(sizeof...(Args) == linear_size) && (std::convertible_to<Args, T> && ...) &&
                (sizeof...(Args) > 0)
    constexpr explicit(sizeof...(Args) == 1) matrix(Args&&... args)
        : _data{static_cast<T>(std::forward<Args>(args))...} {}

    // nested initializer_list constructor (2D only)
    /**
     * @brief Initializes a 2D matrix from a nested initializer list.
     * @tparam D1 Deduced from @c order; constrained to 2 (do not specify
     * explicitly).
     * @param init Row-major nested initializer list; must have exactly @c
     * dimensions[0] rows, each of exactly @c dimensions[1] elements.
     *
     * @throws std::length_error if the number of rows or the size of any row
     * does not match.
     *
     * @code
     * ysc::matrix<int, 2, 3> m{{1, 2, 3}, {4, 5, 6}};
     * assert(m(1, 2) == 6);
     * @endcode
     */
    template <std::size_t D1 = order>
        requires(D1 == 2)
    matrix(std::initializer_list<std::initializer_list<T>> init) : _data{} {
        if (init.size() != dimensions[0]) {
            throw std::length_error{"matrix: wrong number of rows"};
        }
        auto it = _data.begin();
        for (auto const& row_list : init) {
            if (row_list.size() != dimensions[1]) {
                throw std::length_error{"matrix: wrong number of columns"};
            }
            for (auto const& val : row_list) {
                *it++ = val;
            }
        }
    }

    // copy constructors
    /**
     * @brief Initializes the matrix as a copy of another.
     * @param other Source matrix
     */
    matrix(matrix const& other) = default;

    /**
     * @brief Initializes the matrix as a conversion from another.
     * @tparam U     Element type of the source matrix
     * @param  other Source matrix
     *
     * Elements of the matrix are copy-initialized from the elements of the
     * source matrix.
     */
    template <class U>
        requires matrix_convertible_from<T, U>
    matrix(matrix<U, Dimensions...> const& other) {
        std::copy(other._data.cbegin(), other._data.cend(), _data.begin());
    }

    // move constructors
    /**
     * @brief Initializes the matrix with the content of another.
     * @param other Source matrix
     *
     * Elements of the matrix are move-initialized from the elements of the
     * source matrix. `other` is left in a valid but unspecified state.
     */
    matrix(matrix&& other) = default;

    /**
     * @brief Initializes the matrix with the content of another.
     * @tparam U     Element type of the source matrix
     * @param  other Source matrix
     *
     * Elements of the matrix are move-initialized from the elements of the
     * source matrix. `other` is left in a valid but unspecified state.
     */
    template <class U>
        requires matrix_convertible_from<T, U>
    // std::move is used as a range algorithm (element-wise move), not as a
    // cast. Direct std::move(other) is impossible because T != U; per-element
    // conversion is required.
    // NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
    matrix(matrix<U, Dimensions...>&& other) {
        std::move(other._data.cbegin(), other._data.cend(), _data.begin());
    }

    // constructors from matrix_view
    /**
     * @brief Constructs an owning matrix by copying elements from a contiguous view.
     * @param v Contiguous view to copy from
     *
     * Elements are copied with @c std::copy. The resulting matrix is independent of @p v:
     * mutations to either do not affect the other.
     *
     * @code
     * ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
     * auto row0 = m.row(0);                    // contiguous view of row 0
     * auto m2   = ysc::matrix<int, 4>(row0);   // owning copy
     * m2(0) = 99;                              // does not affect m
     * @endcode
     *
     * @ingroup ysc_view
     */
    explicit matrix(const matrix_view<T, contiguous, Dimensions...>& v) {
        std::copy(v.begin(), v.end(), _data.begin());
    }

    /**
     * @brief Constructs an owning matrix by copying elements from a strided view.
     * @param v Strided view to copy from
     *
     * Elements are copied one by one via @c operator(). The resulting matrix is independent
     * of @p v: mutations to either do not affect the other.
     *
     * @code
     * ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
     * auto col1 = m.col(1);                    // strided view of column 1
     * auto m2   = ysc::matrix<int, 3>(col1);   // owning copy
     * m2(0) = 99;                              // does not affect m
     * @endcode
     *
     * @ingroup ysc_view
     */
    explicit matrix(const matrix_view<T, strided, Dimensions...>& v) {
        std::size_t k = 0;
        for (auto& elem : _data) {
            const auto coords = detail::index_to_coordinates(dimensions, k++);
            elem = std::apply([&v](auto... cs) -> T { return v(cs...); }, coords);
        }
    }

    // constructors from std::array and std::span
    /**
     * @brief Constructs from a @c std::array.
     * @param data Array of exactly @c linear_size elements to move into the matrix.
     *
     * @code
     * ysc::matrix<int, 3> m(std::array<int, 3>{1, 2, 3});
     * assert(m(0) == 1);
     * @endcode
     *
     * @ingroup ysc_matrix
     */
    explicit matrix(std::array<T, linear_size> data) noexcept(
        std::is_nothrow_move_constructible_v<T>)
        : _data(std::move(data)) {}

    /**
     * @brief Constructs from a @c std::span.
     * @param data Span of exactly @c linear_size elements; copied into the matrix.
     *
     * @code
     * int buf[3] = {4, 5, 6};
     * ysc::matrix<int, 3> m(std::span<const int, 3>{buf, 3});
     * assert(m(0) == 4);
     * @endcode
     *
     * @ingroup ysc_matrix
     */
    explicit matrix(std::span<const T, linear_size> data) {
        std::copy(data.begin(), data.end(), _data.begin());
    }

    // assignment operators (copy)
    /**
     * @brief Assigns values to a matrix.
     * @param other Source matrix
     */
    matrix& operator=(matrix const& other) = default;

    /**
     * @brief Assigns values to a matrix.
     * @tparam U     Element type of the source matrix
     * @param  other Source matrix
     */
    template <class U>
        requires matrix_convertible_from<T, U>
    matrix& operator=(matrix<U, Dimensions...> const& other) {
        std::copy(other._data.cbegin(), other._data.cend(), _data.begin());
        return *this;
    }

    // assignment operators (move)
    /**
     * @brief Replace the element with those of another matrix.
     * @param other Source matrix
     */
    matrix& operator=(matrix&& other) = default;

    /**
     * @brief Replaces the element with those of another matrix.
     * @tparam U     Element type of the source matrix
     * @param  other Source matrix
     */
    template <class U>
        requires matrix_convertible_from<T, U>
    // Same rationale as the converting move constructor: element-wise move via
    // algorithm, T != U prevents direct std::move(other).
    // NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
    matrix& operator=(matrix<U, Dimensions...>&& other) {
        std::move(other._data.cbegin(), other._data.cend(), _data.begin());
        return *this;
    }

    /** @brief Destructor. */
    ~matrix() = default;

    // comparison operators
    /** @brief Equality comparison — lexicographic on the flat row-major
     * storage.
     */
    friend bool operator==(const matrix& lhs, const matrix& rhs) = default;
    /** @brief Three-way comparison — lexicographic on the flat row-major
     * storage.
     */
    friend auto operator<=>(const matrix& lhs, const matrix& rhs) = default;

    // arithmetic operators
    /**
     * @defgroup ysc_arithmetic Arithmetic operators
     * @brief Element-wise arithmetic operations on @c ysc::matrix.
     */

    /**
     * @brief Adds @a other to this matrix element-wise and assigns the result.
     * @param other Matrix to add
     * @return @c *this
     *
     * @code
     * ysc::matrix<int, 2> a{1, 2}, b{3, 4};
     * a += b;  // a == ysc::matrix<int, 2>{4, 6}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    matrix& operator+=(const matrix& other)
        requires requires(T a, const T& b) { a += b; }
    {
        std::transform(_data.begin(), _data.end(), other._data.cbegin(), _data.begin(),
                       [](T a, const T& b) -> T { return a += b; });
        return *this;
    }

    /**
     * @brief Subtracts @a other from this matrix element-wise and assigns the
     * result.
     * @param other Matrix to subtract
     * @return @c *this
     *
     * @code
     * ysc::matrix<int, 2> a{5, 6}, b{1, 2};
     * a -= b;  // a == ysc::matrix<int, 2>{4, 4}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    matrix& operator-=(const matrix& other)
        requires requires(T a, const T& b) { a -= b; }
    {
        std::transform(_data.begin(), _data.end(), other._data.cbegin(), _data.begin(),
                       [](T a, const T& b) -> T { return a -= b; });
        return *this;
    }

    /**
     * @brief Returns the element-wise sum of two matrices.
     * @param lhs Left-hand matrix
     * @param rhs Right-hand matrix
     * @return New matrix containing element-wise sums
     *
     * @code
     * ysc::matrix<int, 2> a{1, 2}, b{3, 4};
     * auto c = a + b;  // c == ysc::matrix<int, 2>{4, 6}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    [[nodiscard]] friend matrix operator+(matrix lhs, const matrix& rhs)
        requires requires(T a, const T& b) { a += b; }
    {
        return lhs += rhs;
    }

    /**
     * @brief Returns the element-wise difference of two matrices.
     * @param lhs Left-hand matrix
     * @param rhs Right-hand matrix
     * @return New matrix containing element-wise differences
     *
     * @code
     * ysc::matrix<int, 2> a{5, 6}, b{1, 2};
     * auto c = a - b;  // c == ysc::matrix<int, 2>{4, 4}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    [[nodiscard]] friend matrix operator-(matrix lhs, const matrix& rhs)
        requires requires(T a, const T& b) { a -= b; }
    {
        return lhs -= rhs;
    }

    /**
     * @brief Multiplies this matrix by @a other element-wise (Hadamard product)
     * and assigns.
     * @param other Matrix to multiply by
     * @return @c *this
     *
     * @note This is the Hadamard (element-wise) product, not the matrix
     * product. The matrix product will be available as @c ysc::matmul.
     *
     * @code
     * ysc::matrix<int, 2> a{2, 3}, b{4, 5};
     * a *= b;  // a == ysc::matrix<int, 2>{8, 15}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    matrix& operator*=(const matrix& other)
        requires requires(T a, const T& b) { a *= b; }
    {
        std::transform(_data.begin(), _data.end(), other._data.cbegin(), _data.begin(),
                       [](T a, const T& b) -> T { return a *= b; });
        return *this;
    }

    /**
     * @brief Divides this matrix by @a other element-wise and assigns the
     * result.
     * @param other Divisor matrix
     * @return @c *this
     *
     * @code
     * ysc::matrix<int, 2> a{6, 8}, b{2, 4};
     * a /= b;  // a == ysc::matrix<int, 2>{3, 2}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    matrix& operator/=(const matrix& other)
        requires requires(T a, const T& b) { a /= b; }
    {
        std::transform(_data.begin(), _data.end(), other._data.cbegin(), _data.begin(),
                       [](T a, const T& b) -> T { return a /= b; });
        return *this;
    }

    /**
     * @brief Returns the element-wise (Hadamard) product of two matrices.
     * @param lhs Left-hand matrix
     * @param rhs Right-hand matrix
     * @return New matrix containing element-wise products
     *
     * @note This is the Hadamard (element-wise) product, not the matrix
     * product. The matrix product will be available as @c ysc::matmul.
     *
     * @code
     * ysc::matrix<int, 2> a{2, 3}, b{4, 5};
     * auto c = a * b;  // c == ysc::matrix<int, 2>{8, 15}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    [[nodiscard]] friend matrix operator*(matrix lhs, const matrix& rhs)
        requires requires(T a, const T& b) { a *= b; }
    {
        return lhs *= rhs;
    }

    /**
     * @brief Returns the element-wise quotient of two matrices.
     * @param lhs Left-hand matrix (dividend)
     * @param rhs Right-hand matrix (divisor)
     * @return New matrix containing element-wise quotients
     *
     * @code
     * ysc::matrix<int, 2> a{6, 8}, b{2, 4};
     * auto c = a / b;  // c == ysc::matrix<int, 2>{3, 2}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    [[nodiscard]] friend matrix operator/(matrix lhs, const matrix& rhs)
        requires requires(T a, const T& b) { a /= b; }
    {
        return lhs /= rhs;
    }

    // scalar arithmetic operators
    /**
     * @brief Multiplies every element of this matrix by scalar @a s and assigns
     * the result.
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a *=
     * Scalar`
     * @param  s      Scalar multiplier
     * @return @c *this
     *
     * @code
     * ysc::matrix<int, 3> m{1, 2, 3};
     * m *= 2;  // m == ysc::matrix<int, 3>{2, 4, 6}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    template <class Scalar>
    matrix& operator*=(const Scalar& s)
        requires requires(T a, const Scalar& b) { a *= b; }
    {
        std::transform(_data.begin(), _data.end(), _data.begin(),
                       [&s](T a) -> T { return a *= s; });
        return *this;
    }

    /**
     * @brief Divides every element of this matrix by scalar @a s and assigns
     * the result.
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a /=
     * Scalar`
     * @param  s      Scalar divisor
     * @return @c *this
     *
     * @code
     * ysc::matrix<int, 3> m{6, 4, 2};
     * m /= 2;  // m == ysc::matrix<int, 3>{3, 2, 1}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    template <class Scalar>
    matrix& operator/=(const Scalar& s)
        requires requires(T a, const Scalar& b) { a /= b; }
    {
        std::transform(_data.begin(), _data.end(), _data.begin(),
                       [&s](T a) -> T { return a /= s; });
        return *this;
    }

    /**
     * @brief Adds scalar @a s to every element of this matrix and assigns the
     * result.
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a +=
     * Scalar`
     * @param  s      Scalar addend
     * @return @c *this
     *
     * @code
     * ysc::matrix<int, 3> m{1, 2, 3};
     * m += 10;  // m == ysc::matrix<int, 3>{11, 12, 13}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    template <class Scalar>
    matrix& operator+=(const Scalar& s)
        requires requires(T a, const Scalar& b) { a += b; }
    {
        std::transform(_data.begin(), _data.end(), _data.begin(),
                       [&s](T a) -> T { return a += s; });
        return *this;
    }

    /**
     * @brief Subtracts scalar @a s from every element of this matrix and
     * assigns the result.
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a -=
     * Scalar`
     * @param  s      Scalar subtrahend
     * @return @c *this
     *
     * @code
     * ysc::matrix<int, 3> m{11, 12, 13};
     * m -= 10;  // m == ysc::matrix<int, 3>{1, 2, 3}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    template <class Scalar>
    matrix& operator-=(const Scalar& s)
        requires requires(T a, const Scalar& b) { a -= b; }
    {
        std::transform(_data.begin(), _data.end(), _data.begin(),
                       [&s](T a) -> T { return a -= s; });
        return *this;
    }

    /**
     * @brief Returns the element-wise product of a matrix and a scalar.
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a *=
     * Scalar`
     * @param  lhs    Matrix operand
     * @param  s      Scalar multiplier
     * @return New matrix with each element multiplied by @a s
     *
     * @code
     * ysc::matrix<int, 3> m{1, 2, 3};
     * auto r = m * 2;  // r == ysc::matrix<int, 3>{2, 4, 6}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    template <class Scalar>
    [[nodiscard]] friend matrix operator*(matrix lhs, const Scalar& s)
        requires requires(T a, const Scalar& b) { a *= b; }
    {
        return lhs *= s;
    }

    /**
     * @brief Returns the element-wise product of a scalar and a matrix
     * (commutative).
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a *=
     * Scalar`
     * @param  s      Scalar multiplier
     * @param  rhs    Matrix operand
     * @return New matrix with each element multiplied by @a s
     *
     * @code
     * ysc::matrix<int, 3> m{1, 2, 3};
     * auto r = 2 * m;  // r == ysc::matrix<int, 3>{2, 4, 6}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    template <class Scalar>
    [[nodiscard]] friend matrix operator*(const Scalar& s, matrix rhs)
        requires requires(T a, const Scalar& b) { a *= b; }
    {
        return rhs *= s;
    }

    /**
     * @brief Returns a new matrix with every element divided by scalar @a s.
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a /=
     * Scalar`
     * @param  lhs    Matrix operand
     * @param  s      Scalar divisor
     * @return New matrix with each element divided by @a s
     *
     * @code
     * ysc::matrix<int, 3> m{6, 4, 2};
     * auto r = m / 2;  // r == ysc::matrix<int, 3>{3, 2, 1}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    template <class Scalar>
    [[nodiscard]] friend matrix operator/(matrix lhs, const Scalar& s)
        requires requires(T a, const Scalar& b) { a /= b; }
    {
        return lhs /= s;
    }

    /**
     * @brief Returns a new matrix with scalar @a s added to every element.
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a +=
     * Scalar`
     * @param  lhs    Matrix operand
     * @param  s      Scalar addend
     * @return New matrix with @a s added to each element
     *
     * @code
     * ysc::matrix<int, 3> m{1, 2, 3};
     * auto r = m + 10;  // r == ysc::matrix<int, 3>{11, 12, 13}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    template <class Scalar>
    [[nodiscard]] friend matrix operator+(matrix lhs, const Scalar& s)
        requires requires(T a, const Scalar& b) { a += b; }
    {
        return lhs += s;
    }

    /**
     * @brief Returns a new matrix with scalar @a s subtracted from every
     * element.
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a -=
     * Scalar`
     * @param  lhs    Matrix operand
     * @param  s      Scalar subtrahend
     * @return New matrix with @a s subtracted from each element
     *
     * @code
     * ysc::matrix<int, 3> m{11, 12, 13};
     * auto r = m - 10;  // r == ysc::matrix<int, 3>{1, 2, 3}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    template <class Scalar>
    [[nodiscard]] friend matrix operator-(matrix lhs, const Scalar& s)
        requires requires(T a, const Scalar& b) { a -= b; }
    {
        return lhs -= s;
    }

    // unary arithmetic operators
    /**
     * @brief Returns a copy of the matrix (unary plus).
     * @return Copy of @c *this, unchanged
     *
     * @code
     * ysc::matrix<int, 3> m{1, 2, 3};
     * auto r = +m;  // r == m
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    [[nodiscard]] matrix operator+() const { return *this; }

    /**
     * @brief Returns a matrix with all elements negated (unary minus).
     * @return New matrix where each element @c e is replaced by @c -e
     *
     * @code
     * ysc::matrix<int, 3> m{1, -2, 3};
     * auto r = -m;  // r == ysc::matrix<int, 3>{-1, 2, -3}
     * @endcode
     *
     * @ingroup ysc_arithmetic
     */
    [[nodiscard]] constexpr matrix operator-() const noexcept(noexcept(-std::declval<T const&>()))
        requires requires(const T& a) { -a; }
    {
        return map([](const T& v) { return -v; });
    }

    // algorithms
    /**
     * @defgroup ysc_algorithms Algorithms
     * @brief Element-wise functional algorithms on @c ysc::matrix.
     */

    /**
     * @brief Applies a function to every element in place.
     * @tparam F Callable type — must satisfy `std::invocable<F, T&>`
     * @param  f Function to apply to each element
     *
     * Visits every element in row-major order and calls @a f with a reference
     * to the element.
     * @a f may modify the element; the matrix is mutated in place.
     *
     * @code
     * ysc::matrix<int, 3> m{1, 2, 3};
     * m.apply([](int& v) { v *= 2; });
     * // m == ysc::matrix<int, 3>{2, 4, 6}
     * @endcode
     *
     * @ingroup ysc_algorithms
     */
    template <class F>
        requires std::invocable<F, T&>
    constexpr void apply(F f) {
        for (T& v : _data) {
            std::invoke(f, v);
        }
    }

    /**
     * @brief Returns a new matrix obtained by applying a function to every
     * element.
     * @tparam F  Callable type — must satisfy `std::invocable<F, const T&>`
     * @param  f  Function to apply to each element
     * @return A new matrix whose element type is `std::invoke_result_t<F, const
     * T&>` and whose dimensions are identical to @c *this.
     *
     * Does not modify the original matrix.
     *
     * @code
     * ysc::matrix<int, 3> m{1, 2, 3};
     * auto s = m.map([](int v) { return std::to_string(v); });
     * // s is ysc::matrix<std::string, 3>{"1", "2", "3"}
     * @endcode
     *
     * @ingroup ysc_algorithms
     */
    template <class F>
        requires std::invocable<F, const T&>
    [[nodiscard]] constexpr auto
    map(F f) const -> matrix<std::invoke_result_t<F, const T&>, Dimensions...> {
        matrix<std::invoke_result_t<F, const T&>, Dimensions...> result(zero);
        std::transform(_data.cbegin(), _data.cend(), result.begin(),
                       [&f](const T& v) { return std::invoke(f, v); });
        return result;
    }

    /**
     * @brief Returns the sum of all elements.
     * @tparam U  Deduced as @c T; do not specify explicitly.
     * @return Sum of all elements, with the accumulator initialized to @c T{}
     *
     * @code
     * ysc::matrix<int, 3> m{1, 2, 3};
     * assert(m.sum() == 6);
     * @endcode
     *
     * @ingroup ysc_algorithms
     */
    template <class U = T>
        requires std::same_as<U, T>
    [[nodiscard]] constexpr U sum() const
        requires std::default_initializable<T> && requires(T a, const T& b) { a += b; }
    {
        return std::accumulate(cbegin(), cend(), U{},
                               [](U acc, const U& v) -> U { return acc += v; });
    }

    /**
     * @brief Returns the smallest element.
     * @tparam U  Deduced as @c T; do not specify explicitly.
     * @return Minimum element in row-major order
     *
     * @code
     * ysc::matrix<int, 3> m{3, 1, 2};
     * assert(m.min() == 1);
     * @endcode
     *
     * @ingroup ysc_algorithms
     */
    template <class U = T>
        requires std::same_as<U, T>
    [[nodiscard]] constexpr U min() const
        requires(linear_size > 0) && std::totally_ordered<T>
    {
        return std::ranges::min(_data);
    }

    /**
     * @brief Returns the largest element.
     * @tparam U  Deduced as @c T; do not specify explicitly.
     * @return Maximum element in row-major order
     *
     * @code
     * ysc::matrix<int, 3> m{3, 1, 2};
     * assert(m.max() == 3);
     * @endcode
     *
     * @ingroup ysc_algorithms
     */
    template <class U = T>
        requires std::same_as<U, T>
    [[nodiscard]] constexpr U max() const
        requires(linear_size > 0) && std::totally_ordered<T>
    {
        return std::ranges::max(_data);
    }

    /**
     * @brief Returns @c true if every element converts to @c true.
     * @return @c true iff all elements are truthy
     *
     * @code
     * ysc::matrix<int, 3> m{1, 2, 3};
     * assert(m.all() == true);
     * ysc::matrix<int, 3> n{1, 0, 3};
     * assert(n.all() == false);
     * @endcode
     *
     * @ingroup ysc_algorithms
     */
    [[nodiscard]] constexpr bool all() const
        requires std::convertible_to<T, bool>
    {
        return std::ranges::all_of(_data, [](const T& v) { return static_cast<bool>(v); });
    }

    /**
     * @brief Returns @c true if at least one element converts to @c true.
     * @return @c true iff at least one element is truthy
     *
     * @code
     * ysc::matrix<int, 3> m{0, 0, 3};
     * assert(m.any() == true);
     * ysc::matrix<int, 3> n{0, 0, 0};
     * assert(n.any() == false);
     * @endcode
     *
     * @ingroup ysc_algorithms
     */
    [[nodiscard]] constexpr bool any() const
        requires std::convertible_to<T, bool>
    {
        return std::ranges::any_of(_data, [](const T& v) { return static_cast<bool>(v); });
    }

    // modifiers
    /**
     * @brief Assigns the given value to all elements of the matrix.
     * @param value Value to assign
     */
    constexpr void fill(const T& value) noexcept(std::is_nothrow_copy_assignable_v<T>) {
        _data.fill(value);
    }

    /**
     * @brief Exchanges the contents of this matrix with another.
     * @param other Matrix to swap with
     */
    void swap(matrix& other) noexcept(std::is_nothrow_swappable_v<T>) { _data.swap(other._data); }

    // element access
    /**
     * @brief Returns a reference to the first element in the matrix (row-major
     * order).
     *
     * Calling @c front() on an empty matrix is undefined behavior.
     */
    constexpr reference front() noexcept { return _data.front(); }

    /**
     * @brief Returns a const reference to the first element in the matrix
     * (row-major order).
     *
     * Calling @c front() on an empty matrix is undefined behavior.
     */
    [[nodiscard]] constexpr const_reference front() const noexcept { return _data.front(); }

    /**
     * @brief Returns a reference to the last element in the matrix (row-major
     * order).
     *
     * Calling @c back() on an empty matrix is undefined behavior.
     */
    constexpr reference back() noexcept { return _data.back(); }

    /**
     * @brief Returns a const reference to the last element in the matrix
     * (row-major order).
     *
     * Calling @c back() on an empty matrix is undefined behavior.
     */
    [[nodiscard]] constexpr const_reference back() const noexcept { return _data.back(); }

    /**
     * @brief Returns a reference to the element at coordinates.
     * @param coordinates Coordinates of the element to return
     *
     * No bounds checking is performed; if @c coordinates are outside od
     * the matrix dimensions, the behavior is undefined.
     */
    template <class... Coords>
        requires integral_coordinates<Coords...>
    constexpr T const& operator()(Coords... coordinates) const {
        // Intentional: unchecked access on the performance path. Use at() for
        // bounds checking.
        return _data[detail::coordinates_to_index(dimensions, std::array{coordinates...})];
    }

    /**
     * @brief Returns a reference to the element at coordinates.
     * @param coordinates Coordinates of the element to return
     *
     * No bounds checking is performed; if @c coordinates are outside od
     * the matrix dimensions, the behavior is undefined.
     */
    template <class... Coords>
        requires integral_coordinates<Coords...>
    constexpr T& operator()(Coords... coordinates) {
        // Intentional: unchecked access on the performance path. Use at() for
        // bounds checking.
        return _data[detail::coordinates_to_index(dimensions, std::array{coordinates...})];
    }

    /**
     * @brief Returns a reference to the element at coordinates.
     * @param coordinates Coordinates of the element to return
     *
     * If @a coordinates is not within the range of the container, an exception
     * of type @c std::out_of_range is thrown with a message of the form:
     * @code
     * "matrix::at: coordinate <c> is out of bounds for dimension <i> (size=<s>)"
     * @endcode
     * where @c c is the offending coordinate value, @c i is the dimension index,
     * and @c s is the size of that dimension.
     *
     * @throws std::out_of_range if any coordinate is negative or exceeds the
     * dimension size
     */
    template <class... Coords>
        requires integral_coordinates<Coords...>
    [[nodiscard]] const T& at(Coords... coordinates) const {
        const std::array<std::ptrdiff_t, sizeof...(Coords)> coords_arr = {
            static_cast<std::ptrdiff_t>(coordinates)...};
        for (std::size_t i = 0; i < sizeof...(Coords); ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            if (coords_arr[i] < 0 || static_cast<std::size_t>(coords_arr[i]) >= dimensions[i]) {
                throw std::out_of_range(
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    "matrix::at: coordinate " + std::to_string(coords_arr[i]) +
                    " is out of bounds for dimension " + std::to_string(i) +
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    " (size=" + std::to_string(dimensions[i]) + ")");
            }
        }
        return (*this)(coordinates...);
    }

    /**
     * @brief Returns a reference to the element at coordinates.
     * @param coordinates Coordinates of the element to return
     *
     * If @a coordinates is not within the range of the container, an exception
     * of type @c std::out_of_range is thrown with a message of the form:
     * @code
     * "matrix::at: coordinate <c> is out of bounds for dimension <i> (size=<s>)"
     * @endcode
     * where @c c is the offending coordinate value, @c i is the dimension index,
     * and @c s is the size of that dimension.
     *
     * @throws std::out_of_range if any coordinate is negative or exceeds the
     * dimension size
     */
    template <class... Coords>
        requires integral_coordinates<Coords...>
    T& at(Coords... coordinates) {
        const std::array<std::ptrdiff_t, sizeof...(Coords)> coords_arr = {
            static_cast<std::ptrdiff_t>(coordinates)...};
        for (std::size_t i = 0; i < sizeof...(Coords); ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            if (coords_arr[i] < 0 || static_cast<std::size_t>(coords_arr[i]) >= dimensions[i]) {
                throw std::out_of_range(
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    "matrix::at: coordinate " + std::to_string(coords_arr[i]) +
                    " is out of bounds for dimension " + std::to_string(i) +
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    " (size=" + std::to_string(dimensions[i]) + ")");
            }
        }
        return (*this)(coordinates...);
    }

    // ─── views (slice / row / col) ───────────────────────────────────────────

    /**
     * @brief Returns a non-owning view over a hyperslice of the matrix.
     * @tparam Specs Spec types: @c ysc::all_t to keep a dimension, any integral
     * to fix it.
     * @param  specs Per-dimension specs.  Missing trailing specs are implicitly
     * @c ysc::all.
     * @return @c matrix_view<T,contiguous,KeptDims...> if fixed dims form a
     * prefix;
     *         @c matrix_view<T,strided,KeptDims...> otherwise.
     * @throws std::out_of_range if any fixed index is out of bounds for its
     * dimension.
     *
     * @code
     * ysc::matrix<int, 3, 4, 5> m{};
     * auto v0 = m.slice(1);            // contiguous: row 1 across all 4×5
     * columns auto v1 = m.slice(ysc::all, 2);  // strided: column 2 across all
     * 3×5 rows auto v2 = m.slice();             // contiguous: view over the
     * whole matrix
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <typename... Specs>
        requires(sizeof...(Specs) <= order) &&
                ((std::same_as<Specs, all_t> || std::integral<Specs>) && ...) &&
                (sizeof...(Specs) < order || (std::same_as<Specs, all_t> || ...))
    [[nodiscard]] constexpr auto slice(Specs... specs) & {
        using PaddedT = detail::pad_right_with_all_t<order, Specs...>;
        using KeptDims = typename detail::filter_kept_dims<PaddedT, Dimensions...>::type;
        constexpr bool is_prefix = detail::is_prefix_slice_v<PaddedT>;
        using Storage = std::conditional_t<is_prefix, contiguous, strided>;
        using ViewT = detail::make_matrix_view_t<T, Storage, KeptDims>;

        std::array<std::size_t, order> spec_vals{};
        {
            std::size_t i = 0;
            (
                [&](auto s) {
                    using S = std::remove_cvref_t<decltype(s)>;
                    if constexpr (!detail::is_all_v<S>) {
                        auto idx = static_cast<std::size_t>(s);
                        if (idx >= dimensions[i]) {
                            throw std::out_of_range("slice: index out of range");
                        }
                        spec_vals[i] = idx;
                    }
                    ++i;
                }(specs),
                ...);
        }

        auto* base = _data.data() + detail::slice_helper<PaddedT>::offset(dimensions, spec_vals);
        if constexpr (is_prefix) {
            return ViewT{base};
        } else {
            return ViewT{base, detail::slice_helper<PaddedT>::strides(dimensions)};
        }
    }

    /**
     * @brief Returns a non-owning const view over a hyperslice of the matrix.
     * @tparam Specs Spec types: @c ysc::all_t to keep a dimension, any integral
     * to fix it.
     * @param  specs Per-dimension specs.  Missing trailing specs are implicitly
     * @c ysc::all.
     * @return @c matrix_view<const T,contiguous,KeptDims...> or
     *         @c matrix_view<const T,strided,KeptDims...>.
     * @throws std::out_of_range if any fixed index is out of bounds for its
     * dimension.
     *
     * @code
     * const ysc::matrix<int, 3, 4> m{};
     * auto v = m.slice(1);  // const view of row 1
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <typename... Specs>
        requires(sizeof...(Specs) <= order) &&
                ((std::same_as<Specs, all_t> || std::integral<Specs>) && ...) &&
                (sizeof...(Specs) < order || (std::same_as<Specs, all_t> || ...))
    [[nodiscard]] constexpr auto slice(Specs... specs) const& {
        using PaddedT = detail::pad_right_with_all_t<order, Specs...>;
        using KeptDims = typename detail::filter_kept_dims<PaddedT, Dimensions...>::type;
        constexpr bool is_prefix = detail::is_prefix_slice_v<PaddedT>;
        using Storage = std::conditional_t<is_prefix, contiguous, strided>;
        using ViewT = detail::make_matrix_view_t<const T, Storage, KeptDims>;

        std::array<std::size_t, order> spec_vals{};
        {
            std::size_t i = 0;
            (
                [&](auto s) {
                    using S = std::remove_cvref_t<decltype(s)>;
                    if constexpr (!detail::is_all_v<S>) {
                        auto idx = static_cast<std::size_t>(s);
                        if (idx >= dimensions[i]) {
                            throw std::out_of_range("slice: index out of range");
                        }
                        spec_vals[i] = idx;
                    }
                    ++i;
                }(specs),
                ...);
        }

        const auto* base =
            _data.data() + detail::slice_helper<PaddedT>::offset(dimensions, spec_vals);
        if constexpr (is_prefix) {
            return ViewT{base};
        } else {
            return ViewT{base, detail::slice_helper<PaddedT>::strides(dimensions)};
        }
    }

    /**
     * @brief Returns a contiguous view over row @a i (2D matrices only).
     * @tparam D Deduced from @c order; constrained to 2 — do not specify
     * explicitly.
     * @param i  Row index (0-based).
     * @return @c matrix_view<T,contiguous,C> where @c C = @c dimensions[1]
     * @throws std::out_of_range if @a i >= @c dimensions[0].
     *
     * @code
     * ysc::matrix<int, 3, 4> m{};
     * auto r = m.row(1);  // contiguous view of row 1 — 4 elements
     * r(2) = 99;          // writes m(1, 2)
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <std::size_t D = order>
        requires(D == 2)
    [[nodiscard]] constexpr auto row(std::size_t i) & {
        if (i >= dimensions[0]) {
            throw std::out_of_range("row: index out of range");
        }
        return matrix_view<T, contiguous, dimensions[1]>{_data.data() + (i * dimensions[1])};
    }

    /**
     * @brief Returns a const contiguous view over row @a i (2D matrices only).
     * @tparam D Deduced from @c order; constrained to 2 — do not specify
     * explicitly.
     * @param i  Row index (0-based).
     * @return @c matrix_view<const T,contiguous,C> where @c C = @c
     * dimensions[1]
     * @throws std::out_of_range if @a i >= @c dimensions[0].
     *
     * @code
     * const ysc::matrix<int, 3, 4> m{};
     * auto r = m.row(1);
     * assert(r(0) == m(1, 0));
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <std::size_t D = order>
        requires(D == 2)
    [[nodiscard]] constexpr auto row(std::size_t i) const& {
        if (i >= dimensions[0]) {
            throw std::out_of_range("row: index out of range");
        }
        return matrix_view<const T, contiguous, dimensions[1]>{_data.data() + (i * dimensions[1])};
    }

    /**
     * @brief Returns a strided view over column @a j (2D matrices only).
     * @tparam D Deduced from @c order; constrained to 2 — do not specify
     * explicitly.
     * @param j  Column index (0-based).
     * @return @c matrix_view<T,strided,R> where @c R = @c dimensions[0], stride
     * =
     * @c dimensions[1]
     * @throws std::out_of_range if @a j >= @c dimensions[1].
     *
     * @code
     * ysc::matrix<int, 3, 4> m{};
     * auto c = m.col(2);  // strided view of column 2 — 3 elements, stride 4
     * c(1) = 99;          // writes m(1, 2)
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <std::size_t D = order>
        requires(D == 2)
    [[nodiscard]] constexpr auto col(std::size_t j) & {
        if (j >= dimensions[1]) {
            throw std::out_of_range("col: index out of range");
        }
        return matrix_view<T, strided, dimensions[0]>{_data.data() + j,
                                                      std::array<std::size_t, 1>{dimensions[1]}};
    }

    /**
     * @brief Returns a const strided view over column @a j (2D matrices only).
     * @tparam D Deduced from @c order; constrained to 2 — do not specify
     * explicitly.
     * @param j  Column index (0-based).
     * @return @c matrix_view<const T,strided,R> where @c R = @c dimensions[0]
     * @throws std::out_of_range if @a j >= @c dimensions[1].
     *
     * @code
     * const ysc::matrix<int, 3, 4> m{};
     * auto c = m.col(2);
     * assert(c(1) == m(1, 2));
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <std::size_t D = order>
        requires(D == 2)
    [[nodiscard]] constexpr auto col(std::size_t j) const& {
        if (j >= dimensions[1]) {
            throw std::out_of_range("col: index out of range");
        }
        return matrix_view<const T, strided, dimensions[0]>{
            _data.data() + j, std::array<std::size_t, 1>{dimensions[1]}};
    }

    /**
     * @brief Returns a range of contiguous row views (2D matrices only).
     * @tparam D Deduced from @c order; constrained to 2 — do not specify explicitly.
     * @return A range of @c matrix_view<T,contiguous,C> where @c C = @c dimensions[1],
     *         one per row in order (row 0, row 1, …).
     *
     * @code
     * ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * for (auto row_view : m.rows()) {
     *     // row_view is a matrix_view<int, contiguous, 3>
     * }
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <std::size_t D = order>
        requires(D == 2)
    [[nodiscard]] constexpr auto rows() & {
        return [this]<std::size_t... Is>(std::index_sequence<Is...>) {
            return std::array{this->row(Is)...};
        }(std::make_index_sequence<dimensions[0]>{});
    }

    /**
     * @brief Returns a range of const contiguous row views (2D matrices only).
     * @tparam D Deduced from @c order; constrained to 2 — do not specify explicitly.
     * @return A @c std::array of @c matrix_view<const T,contiguous,C> where @c C = @c
     * dimensions[1], one per row in order (row 0, row 1, …).
     *
     * @code
     * const ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * for (auto row_view : m.rows()) {
     *     // row_view is a matrix_view<const int, contiguous, 3>
     * }
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <std::size_t D = order>
        requires(D == 2)
    [[nodiscard]] constexpr auto rows() const& {
        return [this]<std::size_t... Is>(std::index_sequence<Is...>) {
            return std::array{this->row(Is)...};
        }(std::make_index_sequence<dimensions[0]>{});
    }

    /**
     * @brief Returns a range of strided column views (2D matrices only).
     * @tparam D Deduced from @c order; constrained to 2 — do not specify explicitly.
     * @return A @c std::array of @c matrix_view<T,strided,R> where @c R = @c dimensions[0],
     *         one per column in order (col 0, col 1, …).
     *
     * @code
     * ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * for (auto col_view : m.cols()) {
     *     // col_view is a matrix_view<int, strided, 2>
     * }
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <std::size_t D = order>
        requires(D == 2)
    [[nodiscard]] constexpr auto cols() & {
        return [this]<std::size_t... Js>(std::index_sequence<Js...>) {
            return std::array{this->col(Js)...};
        }(std::make_index_sequence<dimensions[1]>{});
    }

    /**
     * @brief Returns a range of const strided column views (2D matrices only).
     * @tparam D Deduced from @c order; constrained to 2 — do not specify explicitly.
     * @return A @c std::array of @c matrix_view<const T,strided,R> where @c R = @c dimensions[0],
     *         one per column in order (col 0, col 1, …).
     *
     * @code
     * const ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * for (auto col_view : m.cols()) {
     *     // col_view is a matrix_view<const int, strided, 2>
     * }
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <std::size_t D = order>
        requires(D == 2)
    [[nodiscard]] constexpr auto cols() const& {
        return [this]<std::size_t... Js>(std::index_sequence<Js...>) {
            return std::array{this->col(Js)...};
        }(std::make_index_sequence<dimensions[1]>{});
    }

    /**
     * @brief Returns a non-owning view over the same data reinterpreted with new dimensions.
     * @tparam NewD New dimensions; their product must equal @c linear_size.
     * @return @c matrix_view<T, contiguous, NewD...>
     *
     * Zero-copy: the view and the matrix share the same memory.
     * Mutation through the view is reflected in the original matrix.
     *
     * @code
     * ysc::matrix<int, 2, 6> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
     * auto v = m.reshape<3, 4>();  // view m as 3x4
     * v(0, 0) = 99;               // also sets m(0, 0)
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <std::size_t... NewD>
    [[nodiscard]] constexpr matrix_view<T, contiguous, NewD...> reshape() & noexcept {
        static_assert((NewD * ...) == linear_size,
                      "reshape: product of new dimensions must equal linear_size");
        return matrix_view<T, contiguous, NewD...>{_data.data()};
    }

    /**
     * @brief Returns a const non-owning view over the same data reinterpreted with new dimensions.
     * @tparam NewD New dimensions; their product must equal @c linear_size.
     * @return @c matrix_view<const T, contiguous, NewD...>
     *
     * @code
     * const ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * auto v = m.reshape<6>();  // matrix_view<const int, contiguous, 6>
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <std::size_t... NewD>
    [[nodiscard]] constexpr matrix_view<const T, contiguous, NewD...> reshape() const& noexcept {
        static_assert((NewD * ...) == linear_size,
                      "reshape: product of new dimensions must equal linear_size");
        return matrix_view<const T, contiguous, NewD...>{_data.data()};
    }

    /**
     * @brief Returns a 1D non-owning view over all elements in row-major order.
     * @return @c matrix_view<T, contiguous, linear_size>
     *
     * Equivalent to @c reshape<linear_size>(). Zero-copy.
     *
     * @code
     * ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * auto v = m.flatten();  // matrix_view<int, contiguous, 6>
     * assert(v(3) == m(1, 0));
     * @endcode
     *
     * @ingroup ysc_view
     */
    [[nodiscard]] constexpr matrix_view<T, contiguous, linear_size> flatten() & noexcept {
        return matrix_view<T, contiguous, linear_size>{_data.data()};
    }

    /**
     * @brief Returns a const 1D non-owning view over all elements in row-major order.
     * @return @c matrix_view<const T, contiguous, linear_size>
     *
     * @code
     * const ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * auto v = m.flatten();  // matrix_view<const int, contiguous, 6>
     * @endcode
     *
     * @ingroup ysc_view
     */
    [[nodiscard]] constexpr matrix_view<const T, contiguous, linear_size>
    flatten() const& noexcept {
        return matrix_view<const T, contiguous, linear_size>{_data.data()};
    }
};

/**
 * @brief Returns a matrix with all elements zero-initialized.
 * @tparam T  Element type
 * @tparam D  Dimensions
 */
template <class T, std::size_t... D> constexpr matrix<T, D...> zeros() noexcept {
    return matrix<T, D...>(zero);
}

/**
 * @brief Returns a matrix with all elements set to @a v.
 * @tparam T  Element type
 * @tparam D  Dimensions
 * @param  v  Value to fill
 */
template <class T, std::size_t... D> constexpr matrix<T, D...> full(const T& v) {
    auto m = zeros<T, D...>();
    m.fill(v);
    return m;
}

/**
 * @brief Returns a matrix with all elements set to @c T{1}.
 * @tparam T  Element type — must support construction from integer literal @c 1
 * @tparam D  Dimensions
 */
template <class T, std::size_t... D>
    requires requires { T{1}; }
constexpr matrix<T, D...> ones() {
    return full<T, D...>(T{1});
}

/**
 * @brief Returns the N×N identity matrix (diagonal = 1, rest = 0).
 * @tparam T  Element type — must support construction from integer literal @c 1
 * @tparam N  Dimension
 */
template <class T, std::size_t N>
    requires requires { T{1}; }
constexpr matrix<T, N, N> identity() {
    auto m = zeros<T, N, N>();
    for (std::size_t i = 0; i < N; ++i) {
        m(i, i) = T{1};
    }
    return m;
}

/**
 * @brief Returns a matrix filled by calling @a f(i) for each linear index @a i.
 * @tparam T    Element type
 * @tparam Dims Dimensions of the result matrix
 * @tparam F    Callable type — must satisfy @c std::invocable<F, std::size_t>
 * @param  f    Generator callable: @c f(i) must be convertible to @c T
 * @return New @c matrix<T, Dims...> where element at linear index @a i equals @c f(i)
 *
 * @code
 * auto m = ysc::generate<int, 3>([](std::size_t i) { return static_cast<int>(i * 2); });
 * // m == ysc::matrix<int, 3>{0, 2, 4}
 * @endcode
 *
 * @ingroup ysc_matrix
 */
template <class T, std::size_t... Dims, std::invocable<std::size_t> F>
[[nodiscard]] constexpr matrix<T, Dims...> generate(F f) {
    matrix<T, Dims...> result;
    for (std::size_t i = 0; i < matrix<T, Dims...>::size(); ++i) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        result.begin()[i] = f(i);
    }
    return result;
}

/**
 * @defgroup ysc_linalg Linear algebra
 * @brief Linear algebra operations on @c ysc::matrix.
 */

/**
 * @brief Returns the transpose of a 2D matrix.
 * @tparam T  Element type
 * @tparam R  Number of rows of the input matrix
 * @tparam C  Number of columns of the input matrix
 * @param  m  Matrix to transpose
 * @return New @c matrix<T,C,R> where `result(j, i) == m(i, j)` for all valid @a
 * i, @a j
 *
 * @code
 * ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
 * auto t = ysc::transpose(m);  // t is ysc::matrix<int, 3, 2>
 * // t(0,0)==1, t(0,1)==4, t(1,0)==2, t(1,1)==5, t(2,0)==3, t(2,1)==6
 * @endcode
 *
 * @ingroup ysc_linalg
 */
template <class T, std::size_t R, std::size_t C>
[[nodiscard]] constexpr matrix<T, C, R> transpose(const matrix<T, R, C>& m) {
    matrix<T, C, R> result(zero);
    for (std::size_t i = 0; i < R; ++i) {
        for (std::size_t j = 0; j < C; ++j) {
            result(j, i) = m(i, j);
        }
    }
    return result;
}

/**
 * @brief Computes the matrix product of two 2D matrices.
 * @tparam Ta Element type of @a a — must be multipliable with @a Tb
 * @tparam Tb Element type of @a b — must be multipliable with @a Ta
 * @tparam M  Number of rows of @a a and the result
 * @tparam N  Shared inner dimension (columns of @a a, rows of @a b)
 * @tparam P  Number of columns of @a b and the result
 * @param  a  Left-hand matrix of size M×N
 * @param  b  Right-hand matrix of size N×P
 * @return New @c matrix<Tc,M,P> where `Tc = decltype(Ta{} * Tb{})`, equal to
 * the matrix product `a × b`
 *
 * @code
 * ysc::matrix<int, 2, 3>    a{1, 2, 3, 4, 5, 6};
 * ysc::matrix<double, 3, 2> b{1.5, 0.0, 0.0, 2.5, 0.0, 0.0};
 * auto c = ysc::matmul(a, b);  // c is ysc::matrix<double, 2, 2>
 * @endcode
 *
 * @ingroup ysc_linalg
 */
template <class Ta, class Tb, std::size_t M, std::size_t N, std::size_t P>
    requires std::invocable<std::multiplies<>, const Ta&, const Tb&> &&
                 requires(std::invoke_result_t<std::multiplies<>, const Ta&, const Tb&> tc,
                          const Ta& a, const Tb& b) { tc += a * b; }
[[nodiscard]] constexpr auto matmul(const matrix<Ta, M, N>& a, const matrix<Tb, N, P>& b)
    -> matrix<std::invoke_result_t<std::multiplies<>, const Ta&, const Tb&>, M, P> {
    using Tc = std::invoke_result_t<std::multiplies<>, const Ta&, const Tb&>;
    matrix<Tc, M, P> result(zero);
    for (std::size_t i = 0; i < M; ++i) {
        for (std::size_t k = 0; k < N; ++k) {
            for (std::size_t j = 0; j < P; ++j) {
                result(i, j) += a(i, k) * b(k, j);
            }
        }
    }
    return result;
}

/**
 * @brief Returns the dot product of two 1D matrices (vectors) of the same
 * length.
 * @tparam Ta Element type of @a a — must be multipliable with @a Tb
 * @tparam Tb Element type of @a b — must be multipliable with @a Ta
 * @tparam N  Number of elements
 * @param  a  First vector
 * @param  b  Second vector
 * @return Scalar value `Tc` where `Tc = decltype(Ta{} * Tb{})`, equal to
 * `sum(a[i] * b[i])` for `i` in `[0, N)`
 *
 * @code
 * ysc::matrix<int, 3>    a{1, 2, 3};
 * ysc::matrix<double, 3> b{0.5, 1.0, 1.5};
 * double r = ysc::dot(a, b);  // r == 7.0
 * @endcode
 *
 * @ingroup ysc_linalg
 */
template <class Ta, class Tb, std::size_t N>
    requires std::invocable<std::multiplies<>, const Ta&, const Tb&> &&
                 requires(std::invoke_result_t<std::multiplies<>, const Ta&, const Tb&> tc,
                          const Ta& a, const Tb& b) { tc += a * b; }
[[nodiscard]] constexpr auto dot(const matrix<Ta, N>& a, const matrix<Tb, N>& b)
    -> std::invoke_result_t<std::multiplies<>, const Ta&, const Tb&> {
    using Tc = std::invoke_result_t<std::multiplies<>, const Ta&, const Tb&>;
    Tc result{};
    for (std::size_t i = 0; i < N; ++i) {
        result += a(i) * b(i);
    }
    return result;
}

/**
 * @brief Multiply a matrix by a column vector.
 * @tparam Ta Element type of @a mat
 * @tparam Tb Element type of @a vec — must be multipliable with @a Ta
 * @tparam M  Number of rows of the matrix
 * @tparam N  Number of columns of the matrix (= size of the vector)
 * @param mat The M×N matrix
 * @param vec The column vector of size N
 * @return Result vector of size M with element type
 *         @c Tc = @c decltype(Ta{} * Tb{})
 *
 * @code
 * ysc::matrix<int, 2, 3>    A{1, 0, 0, 0, 1, 0};
 * ysc::matrix<double, 3>    v{1.5, 2.5, 3.5};
 * auto r = ysc::matmul(A, v); // matrix<double, 2>{1.5, 2.5}
 * @endcode
 *
 * @ingroup ysc_linalg
 */
template <class Ta, class Tb, std::size_t M, std::size_t N>
    requires std::invocable<std::multiplies<>, const Ta&, const Tb&> &&
                 requires(std::invoke_result_t<std::multiplies<>, const Ta&, const Tb&> tc,
                          const Ta& a, const Tb& b) { tc += a * b; }
[[nodiscard]] constexpr auto matmul(const matrix<Ta, M, N>& mat, const matrix<Tb, N>& vec)
    -> matrix<std::invoke_result_t<std::multiplies<>, const Ta&, const Tb&>, M> {
    using Tc = std::invoke_result_t<std::multiplies<>, const Ta&, const Tb&>;
    matrix<Tc, M> result(zero);
    for (std::size_t i = 0; i < M; ++i) {
        for (std::size_t k = 0; k < N; ++k) {
            result(i) += mat(i, k) * vec(k);
        }
    }
    return result;
}

/**
 * @defgroup ysc_io I/O
 * @brief Stream output for `ysc::matrix`.
 */

/**
 * @brief Writes a matrix to an output stream.
 * @tparam T    Element type — must be writable to `std::ostream` via
 * `operator<<`
 * @tparam Dims Dimensions of the matrix
 * @param  os   Destination stream
 * @param  m    Matrix to print
 * @return @a os
 *
 * Produces nested bracket notation: `[e0, e1, ...]` for 1D,
 * `[[e00, e01], [e10, e11]]` for 2D, and so on recursively.
 *
 * This overload only participates in overload resolution when `T` is
 * streamable, so it never causes a hard error for non-streamable element types.
 *
 * @code
 * ysc::matrix<int, 2, 2> m{1, 2, 3, 4};
 * std::cout << m;  // [[1, 2], [3, 4]]
 * @endcode
 *
 * @ingroup ysc_io
 */
template <class T, std::size_t... Dims>
    requires detail::ostream_streamable<T>
std::ostream& operator<<(std::ostream& os, const matrix<T, Dims...>& m) {
    detail::print_recursive(os, m.cbegin(), matrix<T, Dims...>::dimensions, std::size_t{0});
    return os;
}

} // namespace ysc

/**
 * @defgroup ysc_hash Hash support
 * @brief `std::hash` specialization for `ysc::matrix`.
 */

/**
 * @brief Specialization of `std::hash` for `ysc::matrix`.
 * @tparam T  Element type — must be hashable via `std::hash<T>`
 * @tparam D  Dimensions of the matrix
 *
 * Combines element hashes using the boost::hash_combine mixing strategy,
 * so that two matrices with the same elements in the same order produce
 * equal hashes, and matrices differing in at least one element are very
 * likely to produce different hashes.
 *
 * @code
 * std::unordered_set<ysc::matrix<int, 3>> s;
 * s.insert({1, 2, 3});
 * @endcode
 *
 * @ingroup ysc_hash
 */
template <class T, std::size_t... D> struct std::hash<ysc::matrix<T, D...>> {
    std::size_t operator()(const ysc::matrix<T, D...>& m) const noexcept {
        std::size_t h = 0;
        std::hash<T> hasher;
        for (const auto& v : m) {
            h ^= hasher(v) + 0x9E3779B97F4A7C15ULL + (h << 12) + (h >> 4);
        }
        return h;
    }
};

#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L

/**
 * @brief Specialization of @c std::formatter for @c ysc::matrix.
 * @tparam T     Element type — must be streamable via @c
 * operator<<(std::ostream&, const T&)
 * @tparam D     Dimensions of the matrix
 * @tparam CharT Character type for the format context
 *
 * Enables `std::format("{}", m)` and produces the same nested-bracket output as
 * `operator<<`: `[e0, e1, …]` for 1D, `[[e00, e01], [e10, e11]]` for 2D, and so
 * on.
 *
 * Only available when the @c <format> library feature is present
 * (@c __cpp_lib_format ≥ 201907L).
 *
 * @code
 * ysc::matrix<int, 2, 2> m{1, 2, 3, 4};
 * std::string s = std::format("{}", m);  // "[[1, 2], [3, 4]]"
 * @endcode
 *
 * @ingroup ysc_io
 */
template <class T, std::size_t... D, class CharT>
    requires ysc::detail::ostream_streamable<T>
struct std::formatter<ysc::matrix<T, D...>, CharT> : std::formatter<std::string, CharT> {
    template <class FormatContext>
    auto format(const ysc::matrix<T, D...>& m, FormatContext& ctx) const {
        std::ostringstream oss;
        oss << m;
        return std::formatter<std::string, CharT>::format(oss.str(), ctx);
    }
};

#endif // defined(__cpp_lib_format) && __cpp_lib_format >= 201907L

#endif // YSC_MATRIX_HPP
