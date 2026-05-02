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

#include <array>
#include <compare>
#include <iterator>
#include <numeric>
#include <algorithm>
#include <stdexcept>

namespace ysc
{
namespace _details
{
    template<class InputIt, class OutputIt>
    OutputIt partial_product(InputIt first, InputIt last, OutputIt output)
    { *output++ = 1; return partial_sum(first, last, output, std::multiplies<>{}); }

    // cache-friendly:
    // neighbor objects within the right-most coordinate are neighbors in memory
    template<class TDim, class TCoord>
    auto coordinates_to_index(TDim const& dimensions, TCoord const& coords)
    {
        std::array<std::size_t, std::tuple_size_v<TDim>> dimension_product;
        using std::begin, std::cbegin, std::cend, std::crbegin, std::crend, std::prev;
        partial_product(crbegin(dimensions), prev(crend(dimensions)), begin(dimension_product));
        return std::inner_product(cbegin(dimension_product), cend(dimension_product), crbegin(coords), 0);
    }
}

/**
 * @brief Tag a @c matrix object to be zero-initialized.
 * @see matrix::matrix(matrix_zero_t)
 */
constexpr struct matrix_zero_t {} zero;

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
 * given at most @c N initializers that are convertible to @c T:
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
template<class T, std::size_t... Dimensions>
class matrix
{
template<class, std::size_t...> friend class matrix;

public:
    /** @brief Order of the matrix (2D matrix have order 2, 3D order 3, etc.). */
    static constexpr std::size_t order      = sizeof...(Dimensions);
    /** @brief Dimensions of the matrix. An order-`N` matrix has `N` dimensions. */
    static constexpr std::array  dimensions = { Dimensions... };

private:
    static constexpr std::size_t linear_size = (Dimensions * ...);
    std::array<T, linear_size> _data;

public:
    /** @brief Element type. */
    using value_type             = T;
    /** @brief Unsigned integer type for sizes and counts. */
    using size_type              = std::size_t;
    /** @brief Signed integer type for differences between iterators. */
    using difference_type        = std::ptrdiff_t;
    /** @brief Reference to element type. */
    using reference              = T&;
    /** @brief Const reference to element type. */
    using const_reference        = const T&;
    /** @brief Pointer to element type. */
    using pointer                = T*;
    /** @brief Const pointer to element type. */
    using const_pointer          = const T*;
    /** @brief Iterator over matrix elements in row-major order. */
    using iterator               = typename std::array<T, linear_size>::iterator;
    /** @brief Const iterator over matrix elements in row-major order. */
    using const_iterator         = typename std::array<T, linear_size>::const_iterator;
    /** @brief Reverse iterator. */
    using reverse_iterator       = std::reverse_iterator<iterator>;
    /** @brief Const reverse iterator. */
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

public: // iterators
    constexpr iterator               begin()        noexcept { return _data.begin(); }
    constexpr const_iterator         begin()  const noexcept { return _data.begin(); }
    constexpr const_iterator         cbegin() const noexcept { return _data.cbegin(); }
    constexpr iterator               end()          noexcept { return _data.end(); }
    constexpr const_iterator         end()    const noexcept { return _data.end(); }
    constexpr const_iterator         cend()   const noexcept { return _data.cend(); }
    constexpr reverse_iterator       rbegin()       noexcept { return _data.rbegin(); }
    constexpr const_reverse_iterator rbegin() const noexcept { return _data.rbegin(); }
    constexpr const_reverse_iterator crbegin() const noexcept { return _data.crbegin(); }
    constexpr reverse_iterator       rend()         noexcept { return _data.rend(); }
    constexpr const_reverse_iterator rend()   const noexcept { return _data.rend(); }
    constexpr const_reverse_iterator crend()  const noexcept { return _data.crend(); }

public: // capacity
    /**
     * @brief Returns the number of elements in the matrix (product of all dimensions).
     *
     * @note This function is @c static because the size is a compile-time constant.
     */
    static constexpr size_type size()     noexcept { return linear_size; }

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
    static constexpr bool      empty()    noexcept { return linear_size == 0; }

    /**
     * @brief Returns a pointer to the underlying element storage.
     *
     * Elements are stored in row-major order (rightmost dimension is contiguous).
     */
    constexpr pointer       data()       noexcept { return _data.data(); }

    /**
     * @brief Returns a pointer to the underlying element storage.
     *
     * Elements are stored in row-major order (rightmost dimension is contiguous).
     */
    constexpr const_pointer data() const noexcept { return _data.data(); }

public:
    /**
     * @brief Exchanges the given values.
     * @param lhs value to be swapped
     * @param rhs value to be swapped
     *
     * Swaps the elements of @a lhs and @a rhs as if:
     * @code
     using std::swap;
     for (auto lhs_it = lhs.begin(), auto rhs_it = rhs.begin() ; lhs_it != lhs.end() ; ++lhs_it, ++rhs_it) {
         swap(*lhs_it, *rhs_it);
     }
     @endcode
     */
    friend void swap(matrix& lhs, matrix& rhs)
    {
        using std::swap;
        swap(lhs._data, rhs._data);
    }

public: // default constructor
    /**
     * @brief Initializes the matrix.
     *
     * @note If `T` is a trivial type, initialization may result in indeterminate values.
     */
    matrix() = default;

    /**
     * @brief Initializes the matrix following the rules of default initialization.
     *
     * @note If `T` is a trivial type, the matrix is zero-initialized; otherwise the default
     * constructors of its elements are called.
     */
    matrix(matrix_zero_t)
        : _data({})
    {}

    /*
     * @todo Code & test "As an aggregate impersonator, it can be initialized with aggregate-initialization
     * given at most @c N initializers that are convertible to @c T"
     */

public: // aggregate constructors
    /**
     * @brief Initializes the matrix following the rules of aggregate initialization.
     * @tparam Args... Source types
     * @param args...  Source values
     *
     * `matrix<long, 2, 2> m{true, '\x02', 3, 4L},` initializes an order-2 matrix from the values
     * ` true`, `'\x02'`, `3` and `4L` converted to `int`.
     */
    template<class ... Args>
    matrix(Args&& ... args)
        : _data{std::forward<Args>(args)...}
    {}

public: // copy constructors
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
    template<class U>
    matrix(matrix<U, Dimensions...> const& other)
    { std::copy(other._data.cbegin(), other._data.cend(), _data.begin()); }

public: // move constructors
    /**
     * @brief Initializes the matrix with the content of another.
     * @param other Source matrix
     *
     * Elements of the matrix are move-initialized from the elements of the source matrix.
     * `other` is left in a valid but unspecified state.
     */
    matrix(matrix && other) = default;

    /**
     * @brief Initializes the matrix with the content of another.
     * @tparam U     Element type of the source matrix
     * @param  other Source matrix
     *
     * Elements of the matrix are move-initialized from the elements of the source matrix.
     * `other` is left in a valid but unspecified state.
     */
    template<class U>
    matrix(matrix<U, Dimensions...> && other)
    { std::move(other._data.cbegin(), other._data.cend(), _data.begin()); }

public: // assignment operators (copy)
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
    template<class U>
    matrix& operator=(matrix<U, Dimensions...> const& other)
    { std::copy(other._data.cbegin(), other._data.cend(), _data.begin()); return *this; }

public: // assignment operators (move)
    /**
     * @brief Replace the element with those of another matrix.
     * @param other Source matrix
     */
    matrix& operator=(matrix && other) = default;

    /**
     * @brief Replaces the element with those of another matrix.
     * @tparam U     Element type of the source matrix
     * @param  other Source matrix
     */
    template<class U>
    matrix& operator=(matrix<U, Dimensions...> && other)
    { std::move(other._data.cbegin(), other._data.cend(), _data.begin()); return *this; }

public: // comparison operators
    /** @brief Equality comparison — lexicographic on the flat row-major storage. */
    friend bool operator==(const matrix& lhs, const matrix& rhs) = default;
    /** @brief Three-way comparison — lexicographic on the flat row-major storage. */
    friend auto operator<=>(const matrix& lhs, const matrix& rhs) = default;

public: // element access
    /**
     * @brief Returns a reference to the element at coordinates.
     * @param coordinates Coordinates of the element to return
     *
     * No bounds checking is performed; if @c coordinates are outside od
     * the matrix dimensions, the behavior is undefined.
     */
    template<class... Coords>
    T const& operator()(Coords... coordinates) const
    { return _data[_details::coordinates_to_index(dimensions, std::array{coordinates...})]; }

    /**
     * @brief Returns a reference to the element at coordinates.
     * @param coordinates Coordinates of the element to return
     *
     * No bounds checking is performed; if @c coordinates are outside od
     * the matrix dimensions, the behavior is undefined.
     */
    template<class... Coords>
    T& operator()(Coords... coordinates)
    { return _data[_details::coordinates_to_index(dimensions, std::array{coordinates...})]; }

    /**
     * @brief Returns a reference to the element at coordinates.
     * @param coordinates Coordinates of the element to return
     *
     * If @a coordinates is not within the range of the container, an exception of type
     * @c std::out_of_range is thrown.
     */
    template<class... Coords>
    const T& at(Coords... coordinates) const
    {
        const bool any_of_coords_is_negative = ( (coordinates < 0) || ... );
        const bool any_of_coords_is_out_of_bound = ( (coordinates >= Dimensions) || ... );
        if (any_of_coords_is_negative == true || any_of_coords_is_out_of_bound == true) {
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
    template<class... Coords>
    T& at(Coords... coordinates)
    {
        const bool any_of_coords_is_negative = ( (coordinates < 0) || ... );
        const bool any_of_coords_is_out_of_bound = ( (coordinates >= Dimensions) || ... );
        if (any_of_coords_is_negative == true || any_of_coords_is_out_of_bound == true) {
            throw std::out_of_range{"matrix::at"};
        }
        return (*this)(coordinates...);
    }
};
} // namespace ysc

#endif // YSC_MATRIX_HPP
