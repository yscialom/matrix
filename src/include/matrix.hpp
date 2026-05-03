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
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace ysc {
namespace _details {
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
} // namespace _details

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
        return _data[_details::coordinates_to_index(dimensions, std::array{coordinates...})];
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
        return _data[_details::coordinates_to_index(dimensions, std::array{coordinates...})];
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

} // namespace ysc

#endif // YSC_MATRIX_HPP
