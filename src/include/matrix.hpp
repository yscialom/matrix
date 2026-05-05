/**
 * @file matrix.hpp
 * @author Yankel Scialom (YSC) <yankel-pro@scialom.org>
 * @date 2019
 *
 * @copyright This project is released under GNU Lesser General Public License; see
 *            COPYING and COPYING.LESSER files attached.
 *
 * The matrix library is a header-only template library defining a general-purpose
 * multi-dimension container of static dimensions.
 */
#ifndef YSC_MATRIX_HPP
#define YSC_MATRIX_HPP

#include <algorithm>
#include <array>
#include <compare>
#include <concepts>
#include <functional>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <type_traits>
// Clang < 17 cannot compile libstdc++-14's <format> due to unicode.h incompatibility
#if __has_include(<format>) && (!defined(__clang__) || __clang_major__ >= 17)
#include <format>
#include <sstream>
#endif

namespace ysc {
namespace detail {

template <class T>
concept ostream_streamable = requires(std::ostream& os, const T& v) { os << v; };

// Print a hyperslice of a matrix starting at `it`, covering dimension `dim_idx` onward.
// Returns an iterator past the last element printed.
// NOLINTNEXTLINE(misc-no-recursion)
template <class It, class Dims>
It print_recursive(std::ostream& os, It it, const Dims& dims, std::size_t dim_idx) {
    os << '[';
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
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

// cache-friendly: neighbor objects within the right-most coordinate are neighbors in memory
template <class TDim, class TCoord>
constexpr auto coordinates_to_index(TDim const& dimensions, TCoord const& coords) {
    // Row-major index: index = c[0]*D[1]*…*D[N-1] + c[1]*D[2]*…*D[N-1] + … + c[N-1]
    // Evaluated right-to-left; `stride` is the product of all dimensions already visited.
    std::size_t index = 0;
    std::size_t stride = 1;
    auto dim = dimensions.crbegin();
    auto coord = coords.crbegin();
    for (; dim != dimensions.crend(); ++dim, ++coord) {
        index += static_cast<std::size_t>(*coord) * stride;
        stride *= *dim;
    }
    return index;
}
} // namespace detail

/**
 * @brief Satisfied when `U` is convertible to `T`.
 * Used to constrain converting constructors and assignment operators.
 */
template <class T, class U>
concept matrix_convertible_from = std::convertible_to<U, T>;

/**
 * @brief Satisfied when all types in `Coords` are integral (ignoring cv-ref qualifiers).
 * Used to constrain `operator()` and `at()` so that floating-point or other
 * non-integral coordinate types yield a readable diagnostic.
 */
template <class... Coords>
concept integral_coordinates = (std::integral<std::remove_cvref_t<Coords>> && ...);

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
 * `matrix<T, 2, 5, 9>` is an order 3  matrix of @c T elements; its dimensions are
 * 2 by 5 by 9 (90 @c T elements in total).
 *
 * This container is a class type with the semantics of an aggregate similar to
 * a struct holding a C-style array `T[Dimensions][...]` as its only non-static
 * data member. Unlike a C-style array, it doesn't decay to `T*` automatically.
 * As an aggregate impersonator, it can be initialized with aggregate-initialization
 * given exactly @c linear_size initializers that are convertible to @c T:
 * `ysc::matrix<int, 3, 2> m = {1,2,3,4,5,6};`.
 *
 * The struct combines the performance and accessibility of a C-style array with
 * the benefits of a standard container, such as knowing its own size, supporting
 * assignment, random access iterators, etc.
 *
 * @todo Requirements (Container, etc.)
 *
 * @todo Special case when one dimension is 0.
 *
 * ### Iterator invalidation
 * As a rule, iterators to aa matrix are never invalidated throughout the lifetime of
 * the matrix. One should take note, however, that during swap, the iterator will continue
 * to point to the same matrix element, and will thus change its value.
 */
template <class T, std::size_t... Dimensions> class matrix {
    template <class, std::size_t...> friend class matrix;

public:
    /** @brief Order of the matrix (2D matrix have order 2, 3D order 3, etc.). */
    static constexpr std::size_t order = sizeof...(Dimensions);
    /** @brief Dimensions of the matrix. An order-`N` matrix has `N` dimensions. */
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
     * @brief Returns the number of elements in the matrix (product of all dimensions).
     *
     * @note This function is @c static because the size is a compile-time constant.
     */
    static constexpr size_type size() noexcept { return linear_size; }

    /**
     * @brief Returns the maximum number of elements the matrix can hold.
     *
     * Always equal to @c size() for this fixed-size container.
     *
     * @note This function is @c static because the value is a compile-time constant.
     */
    static constexpr size_type max_size() noexcept { return linear_size; }

    /**
     * @brief Returns whether the matrix has no elements.
     *
     * @note This function is @c static because the value is a compile-time constant.
     */
    static constexpr bool empty() noexcept { return linear_size == 0; }

    /**
     * @brief Returns a pointer to the underlying element storage.
     *
     * Elements are stored in row-major order (rightmost dimension is contiguous).
     */
    constexpr pointer data() noexcept { return _data.data(); }

    /**
     * @brief Returns a pointer to the underlying element storage.
     *
     * Elements are stored in row-major order (rightmost dimension is contiguous).
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
     for (auto lhs_it = lhs.begin(), auto rhs_it = rhs.begin() ; lhs_it != lhs.end() ; ++lhs_it,
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
     * @note If `T` is a trivial type, initialization may result in indeterminate values.
     */
    // Intentional: _data is deliberately left uninitialized for trivial T to avoid the cost of
    // zero-initialization on the hot path. Use matrix(matrix_zero_t) for zero-initialization.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
    matrix() = default;

    /**
     * @brief Initializes the matrix following the rules of default initialization.
     *
     * @note If `T` is a trivial type, the matrix is zero-initialized; otherwise the default
     * constructors of its elements are called.
     */
    constexpr matrix(matrix_zero_t /*zero*/) : _data({}) {}

    // aggregate constructors
    /**
     * @brief Initializes the matrix following the rules of aggregate initialization.
     * @tparam Args... Source types (must all be convertible to @c T)
     * @param args...  Source values (must be exactly @c linear_size values)
     *
     * `matrix<long, 2, 2> m{true, '\x02', 3, 4L}` initializes an order-2 matrix from the values
     * `true`, `'\x02'`, `3` and `4L` converted to `long`.
     * Partial initialization (fewer than `linear_size` values) is not supported; use
     * `matrix(matrix_zero_t)` to zero-initialize.
     */
    template <class... Args>
        requires(sizeof...(Args) == linear_size) && (std::convertible_to<Args, T> && ...) &&
                (sizeof...(Args) > 0)
    constexpr explicit(sizeof...(Args) == 1) matrix(Args&&... args)
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay)
        : _data{static_cast<T>(std::forward<Args>(args))...} {}

    // nested initializer_list constructor (2D only)
    /**
     * @brief Initializes a 2D matrix from a nested initializer list.
     * @tparam D1 Deduced from @c order; constrained to 2 (do not specify explicitly).
     * @param init Row-major nested initializer list; must have exactly @c dimensions[0] rows,
     *             each of exactly @c dimensions[1] elements.
     *
     * @throws std::length_error if the number of rows or the size of any row does not match.
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
     * Elements of the matrix are copy-initialized from the elements of the source matrix.
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
     * Elements of the matrix are move-initialized from the elements of the source matrix.
     * `other` is left in a valid but unspecified state.
     */
    matrix(matrix&& other) = default;

    /**
     * @brief Initializes the matrix with the content of another.
     * @tparam U     Element type of the source matrix
     * @param  other Source matrix
     *
     * Elements of the matrix are move-initialized from the elements of the source matrix.
     * `other` is left in a valid but unspecified state.
     */
    template <class U>
        requires matrix_convertible_from<T, U>
    // std::move is used as a range algorithm (element-wise move), not as a cast.
    // Direct std::move(other) is impossible because T != U; per-element conversion is required.
    // NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
    matrix(matrix<U, Dimensions...>&& other) {
        std::move(other._data.cbegin(), other._data.cend(), _data.begin());
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
    // Same rationale as the converting move constructor: element-wise move via algorithm,
    // T != U prevents direct std::move(other).
    // NOLINTNEXTLINE(cppcoreguidelines-rvalue-reference-param-not-moved)
    matrix& operator=(matrix<U, Dimensions...>&& other) {
        std::move(other._data.cbegin(), other._data.cend(), _data.begin());
        return *this;
    }

    /** @brief Destructor. */
    ~matrix() = default;

    // comparison operators
    /** @brief Equality comparison — lexicographic on the flat row-major storage. */
    friend bool operator==(const matrix& lhs, const matrix& rhs) = default;
    /** @brief Three-way comparison — lexicographic on the flat row-major storage. */
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
     * @brief Subtracts @a other from this matrix element-wise and assigns the result.
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
     * @brief Multiplies this matrix by @a other element-wise (Hadamard product) and assigns.
     * @param other Matrix to multiply by
     * @return @c *this
     *
     * @note This is the Hadamard (element-wise) product, not the matrix product.
     *       The matrix product will be available as @c ysc::matmul.
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
     * @brief Divides this matrix by @a other element-wise and assigns the result.
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
     * @note This is the Hadamard (element-wise) product, not the matrix product.
     *       The matrix product will be available as @c ysc::matmul.
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
     * @brief Multiplies every element of this matrix by scalar @a s and assigns the result.
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a *= Scalar`
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
     * @brief Divides every element of this matrix by scalar @a s and assigns the result.
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a /= Scalar`
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
     * @brief Adds scalar @a s to every element of this matrix and assigns the result.
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a += Scalar`
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
     * @brief Subtracts scalar @a s from every element of this matrix and assigns the result.
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a -= Scalar`
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
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a *= Scalar`
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
     * @brief Returns the element-wise product of a scalar and a matrix (commutative).
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a *= Scalar`
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
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a /= Scalar`
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
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a += Scalar`
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
     * @brief Returns a new matrix with scalar @a s subtracted from every element.
     * @tparam Scalar Scalar type — must support compound assignment: `T a; a -= Scalar`
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
    [[nodiscard]] matrix operator-() const
        requires requires(const T& a) { -a; }
    {
        matrix result(zero);
        std::transform(cbegin(), cend(), result.begin(), [](const T& a) -> T { return -a; });
        return result;
    }

    // algorithms
    /**
     * @defgroup ysc_algorithms Algorithms
     * @brief Element-wise functional algorithms on @c ysc::matrix.
     */

    /**
     * @brief Applies a function to every element in place.
     * @tparam F Callable type — must accept `T&`
     * @param  f Function to apply to each element
     *
     * Visits every element in row-major order and calls @a f with a reference to the element.
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
    template <std::invocable<T&> F>
    void apply(F f) {
        for (T& v : _data) {
            std::invoke(f, v);
        }
    }

    /**
     * @brief Returns a new matrix obtained by applying a function to every element.
     * @tparam F  Callable type — must accept `const T&`
     * @param  f  Function to apply to each element
     * @return A new matrix whose element type is `std::invoke_result_t<F, const T&>`
     *         and whose dimensions are identical to @c *this.
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
    template <std::invocable<const T&> F>
    [[nodiscard]] auto map(F f) const -> matrix<std::invoke_result_t<F, const T&>, Dimensions...> {
        matrix<std::invoke_result_t<F, const T&>, Dimensions...> result(zero);
        std::transform(_data.cbegin(), _data.cend(), result.begin(),
                       [&f](const T& v) { return std::invoke(f, v); });
        return result;
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
     * @brief Returns a reference to the first element in the matrix (row-major order).
     *
     * Calling @c front() on an empty matrix is undefined behavior.
     */
    constexpr reference front() noexcept { return _data.front(); }

    /**
     * @brief Returns a const reference to the first element in the matrix (row-major order).
     *
     * Calling @c front() on an empty matrix is undefined behavior.
     */
    [[nodiscard]] constexpr const_reference front() const noexcept { return _data.front(); }

    /**
     * @brief Returns a reference to the last element in the matrix (row-major order).
     *
     * Calling @c back() on an empty matrix is undefined behavior.
     */
    constexpr reference back() noexcept { return _data.back(); }

    /**
     * @brief Returns a const reference to the last element in the matrix (row-major order).
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
        // Intentional: unchecked access on the performance path. Use at() for bounds checking.
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
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
        // Intentional: unchecked access on the performance path. Use at() for bounds checking.
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        return _data[detail::coordinates_to_index(dimensions, std::array{coordinates...})];
    }

    /**
     * @brief Returns a reference to the element at coordinates.
     * @param coordinates Coordinates of the element to return
     *
     * If @a coordinates is not within the range of the container, an exception of type
     * @c std::out_of_range is thrown.
     */
    template <class... Coords>
        requires integral_coordinates<Coords...>
    [[nodiscard]] const T& at(Coords... coordinates) const {
        const bool any_of_coords_is_negative = ((coordinates < 0) || ...);
        const bool any_of_coords_is_out_of_bound = ((coordinates >= Dimensions) || ...);
        if (any_of_coords_is_negative || any_of_coords_is_out_of_bound) {
            throw std::out_of_range{"matrix::at"};
        }
        return (*this)(coordinates...);
    }

    /**
     * @brief Returns a reference to the element at coordinates.
     * @param coordinates Coordinates of the element to return
     *
     * If @a coordinates is not within the range of the container, an exception of type
     * @c std::out_of_range is thrown.
     */
    template <class... Coords>
        requires integral_coordinates<Coords...>
    T& at(Coords... coordinates) {
        const bool any_of_coords_is_negative = ((coordinates < 0) || ...);
        const bool any_of_coords_is_out_of_bound = ((coordinates >= Dimensions) || ...);
        if (any_of_coords_is_negative || any_of_coords_is_out_of_bound) {
            throw std::out_of_range{"matrix::at"};
        }
        return (*this)(coordinates...);
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
 * @defgroup ysc_io I/O
 * @brief Stream output for `ysc::matrix`.
 */

/**
 * @brief Writes a matrix to an output stream.
 * @tparam T    Element type — must be writable to `std::ostream` via `operator<<`
 * @tparam Dims Dimensions of the matrix
 * @param  os   Destination stream
 * @param  m    Matrix to print
 * @return @a os
 *
 * Produces nested bracket notation: `[e0, e1, ...]` for 1D,
 * `[[e00, e01], [e10, e11]]` for 2D, and so on recursively.
 *
 * This overload only participates in overload resolution when `T` is streamable,
 * so it never causes a hard error for non-streamable element types.
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
            h ^= hasher(v) + 0x9e3779b9 + (h << 6) + (h >> 2);
        }
        return h;
    }
};

#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L

/**
 * @brief Specialization of @c std::formatter for @c ysc::matrix.
 * @tparam T     Element type — must be streamable via @c operator<<(std::ostream&, const T&)
 * @tparam D     Dimensions of the matrix
 * @tparam CharT Character type for the format context
 *
 * Enables `std::format("{}", m)` and produces the same nested-bracket output as
 * `operator<<`: `[e0, e1, …]` for 1D, `[[e00, e01], [e10, e11]]` for 2D, and so on.
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
