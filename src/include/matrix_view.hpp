/**
 * @file matrix_view.hpp
 * @author Yankel Scialom (YSC) <yankel-pro@scialom.org>
 * @date 2026
 *
 * @copyright This project is released under GNU Lesser General Public License; see
 *            COPYING and COPYING.LESSER files attached.
 *
 * Non-owning view over a ysc::matrix storage.
 */
#ifndef YSC_MATRIX_VIEW_HPP
#define YSC_MATRIX_VIEW_HPP

#include <algorithm>
#include <array>
#include <iterator>
#include <stdexcept>

#include <matrix.hpp>

namespace ysc {

/**
 * @defgroup ysc_view Views
 * @brief Non-owning read/write views over @c ysc::matrix storage.
 */

/**
 * @brief Non-owning read/write view over a contiguous sequence of @c T elements.
 *
 * @tparam T          Element type
 * @tparam Dimensions Dimensions of the view (same as the underlying @c matrix)
 *
 * A @c matrix_view holds a raw pointer and the dimension metadata as template
 * parameters.  It does **not** own the memory: the lifetime of the underlying
 * @c matrix (or raw array) must exceed the lifetime of the view.
 *
 * @warning Destroying the underlying @c matrix while a @c matrix_view still
 *          refers to it is undefined behavior.
 *
 * Because dimensions are part of the type, @c sizeof(matrix_view<T,D...>) equals
 * @c sizeof(T*) on every platform (single pointer, no extra bookkeeping).
 *
 * @code
 * ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
 * ysc::matrix_view<int, 2, 3> v = m;
 * v(0, 0) = 99;           // mutation is reflected in m
 * assert(m(0, 0) == 99);
 * @endcode
 *
 * @ingroup ysc_view
 */
template <class T, std::size_t... Dimensions> class matrix_view {
public:
    /** @brief Order of the view (number of dimensions). */
    static constexpr std::size_t order = sizeof...(Dimensions);
    /** @brief Dimensions of the view. */
    static constexpr std::array dimensions = {Dimensions...};

private:
    static constexpr std::size_t linear_size = (Dimensions * ...);
    T* _ptr;

    // Centralises the single pointer-arithmetic operation so every caller stays clean.
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    [[nodiscard]] constexpr T* end_ptr() const noexcept { return _ptr + linear_size; }

public:
    // ─── typedefs ────────────────────────────────────────────────────────────

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
    /** @brief Contiguous iterator over view elements in row-major order. */
    using iterator = T*;
    /** @brief Const contiguous iterator over view elements in row-major order. */
    using const_iterator = const T*;
    /** @brief Reverse iterator. */
    using reverse_iterator = std::reverse_iterator<iterator>;
    /** @brief Const reverse iterator. */
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    // ─── construction ────────────────────────────────────────────────────────

    matrix_view() = delete;

    /**
     * @brief Constructs a view over an existing @c matrix.
     * @param m Matrix to view
     *
     * Implicit conversion is intentional: `matrix_view<T,D...> v = m;` should work
     * as naturally as `std::string_view sv = str;`.
     *
     * @code
     * ysc::matrix<int, 3> m{1, 2, 3};
     * ysc::matrix_view<int, 3> v = m;
     * @endcode
     *
     * @ingroup ysc_view
     */
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    constexpr matrix_view(matrix<T, Dimensions...>& m) noexcept : _ptr{m.data()} {}

    /**
     * @brief Constructs a view from a raw pointer to a contiguous sequence of elements.
     * @param ptr Pointer to the first element
     *
     * The caller is responsible for ensuring that @p ptr points to a valid contiguous
     * sequence of at least @c linear_size elements.
     *
     * @code
     * int buf[6]{};
     * ysc::matrix_view<int, 2, 3> v{buf};
     * @endcode
     *
     * @ingroup ysc_view
     */
    constexpr explicit matrix_view(T* ptr) noexcept : _ptr{ptr} {}

    /** @brief Copies the view (shallow: copies the pointer, not the data). */
    constexpr matrix_view(const matrix_view&) noexcept = default;
    /** @brief Copy-assigns the view (shallow). */
    constexpr matrix_view& operator=(const matrix_view&) noexcept = default;
    /** @brief Move-constructs the view. */
    constexpr matrix_view(matrix_view&&) noexcept = default;
    /** @brief Move-assigns the view. */
    constexpr matrix_view& operator=(matrix_view&&) noexcept = default;
    /** @brief Destructor (does not free the underlying storage). */
    ~matrix_view() = default;

    // ─── iterators ───────────────────────────────────────────────────────────

    /**
     * @brief Returns an iterator to the first element (row-major order).
     * @return Iterator to the first element
     * @ingroup ysc_view
     */
    constexpr iterator begin() noexcept { return _ptr; }

    /**
     * @brief Returns a const iterator to the first element (row-major order).
     * @return Const iterator to the first element
     * @ingroup ysc_view
     */
    [[nodiscard]] constexpr const_iterator begin() const noexcept { return _ptr; }

    /**
     * @brief Returns a const iterator to the first element (row-major order).
     * @return Const iterator to the first element
     * @ingroup ysc_view
     */
    [[nodiscard]] constexpr const_iterator cbegin() const noexcept { return _ptr; }

    /**
     * @brief Returns an iterator past the last element.
     * @return Iterator past the last element
     * @ingroup ysc_view
     */
    constexpr iterator end() noexcept { return end_ptr(); }

    /**
     * @brief Returns a const iterator past the last element.
     * @return Const iterator past the last element
     * @ingroup ysc_view
     */
    [[nodiscard]] constexpr const_iterator end() const noexcept { return end_ptr(); }

    /**
     * @brief Returns a const iterator past the last element.
     * @return Const iterator past the last element
     * @ingroup ysc_view
     */
    [[nodiscard]] constexpr const_iterator cend() const noexcept { return end_ptr(); }

    /**
     * @brief Returns a reverse iterator to the last element.
     * @return Reverse iterator to the last element
     * @ingroup ysc_view
     */
    constexpr reverse_iterator rbegin() noexcept { return reverse_iterator{end_ptr()}; }

    /**
     * @brief Returns a const reverse iterator to the last element.
     * @return Const reverse iterator to the last element
     * @ingroup ysc_view
     */
    [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator{end_ptr()};
    }

    /**
     * @brief Returns a const reverse iterator to the last element.
     * @return Const reverse iterator to the last element
     * @ingroup ysc_view
     */
    [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator{end_ptr()};
    }

    /**
     * @brief Returns a reverse iterator past the first element.
     * @return Reverse iterator past the first element
     * @ingroup ysc_view
     */
    constexpr reverse_iterator rend() noexcept { return reverse_iterator{_ptr}; }

    /**
     * @brief Returns a const reverse iterator past the first element.
     * @return Const reverse iterator past the first element
     * @ingroup ysc_view
     */
    [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator{_ptr};
    }

    /**
     * @brief Returns a const reverse iterator past the first element.
     * @return Const reverse iterator past the first element
     * @ingroup ysc_view
     */
    [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator{_ptr};
    }

    // ─── capacity ────────────────────────────────────────────────────────────

    /**
     * @brief Returns the number of elements in the view (product of all dimensions).
     * @return Number of elements
     *
     * @note This function is @c static because the size is a compile-time constant.
     *
     * @code
     * static_assert(ysc::matrix_view<int, 2, 3>::size() == 6);
     * @endcode
     *
     * @ingroup ysc_view
     */
    [[nodiscard]] static constexpr size_type size() noexcept { return linear_size; }

    /**
     * @brief Returns the maximum number of elements the view can address.
     * @return Maximum number of elements (always equal to @c size())
     *
     * @note This function is @c static because the value is a compile-time constant.
     *
     * @ingroup ysc_view
     */
    [[nodiscard]] static constexpr size_type max_size() noexcept { return linear_size; }

    /**
     * @brief Returns whether the view has no elements.
     * @return @c true if @c linear_size == 0
     *
     * @note This function is @c static because the value is a compile-time constant.
     *
     * @ingroup ysc_view
     */
    [[nodiscard]] static constexpr bool empty() noexcept { return linear_size == 0; }

    /**
     * @brief Returns a pointer to the underlying element storage.
     * @return Pointer to the first element
     *
     * Elements are stored in row-major order (rightmost dimension is contiguous).
     *
     * @ingroup ysc_view
     */
    [[nodiscard]] constexpr pointer data() noexcept { return _ptr; }

    /**
     * @brief Returns a const pointer to the underlying element storage.
     * @return Const pointer to the first element
     *
     * Elements are stored in row-major order (rightmost dimension is contiguous).
     *
     * @ingroup ysc_view
     */
    [[nodiscard]] constexpr const_pointer data() const noexcept { return _ptr; }

    // ─── element access ──────────────────────────────────────────────────────

    /**
     * @brief Returns a reference to the first element in the view (row-major order).
     *
     * Calling @c front() on an empty view is undefined behavior.
     *
     * @ingroup ysc_view
     */
    constexpr reference front() noexcept { return *_ptr; }

    /**
     * @brief Returns a const reference to the first element in the view (row-major order).
     *
     * Calling @c front() on an empty view is undefined behavior.
     *
     * @ingroup ysc_view
     */
    [[nodiscard]] constexpr const_reference front() const noexcept { return *_ptr; }

    /**
     * @brief Returns a reference to the last element in the view (row-major order).
     *
     * Calling @c back() on an empty view is undefined behavior.
     *
     * @ingroup ysc_view
     */
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    constexpr reference back() noexcept { return *(end_ptr() - 1); }

    /**
     * @brief Returns a const reference to the last element in the view (row-major order).
     *
     * Calling @c back() on an empty view is undefined behavior.
     *
     * @ingroup ysc_view
     */
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    [[nodiscard]] constexpr const_reference back() const noexcept { return *(end_ptr() - 1); }

    /**
     * @brief Returns a reference to the element at the given coordinates.
     * @tparam Coords Integral coordinate types
     * @param coords  Coordinates of the element
     * @return Reference to the element
     *
     * No bounds checking is performed; if @p coords are outside the view dimensions,
     * the behavior is undefined.  Use @c at() for bounds-checked access.
     *
     * @code
     * ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * ysc::matrix_view<int, 2, 3> v = m;
     * assert(v(1, 2) == 6);
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <class... Coords>
        requires integral_coordinates<Coords...>
    constexpr T const& operator()(Coords... coords) const noexcept {
        // Intentional: unchecked access on the performance path. Use at() for bounds checking.
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return _ptr[detail::coordinates_to_index(dimensions, std::array{coords...})];
    }

    /**
     * @brief Returns a reference to the element at the given coordinates.
     * @tparam Coords Integral coordinate types
     * @param coords  Coordinates of the element
     * @return Reference to the element
     *
     * No bounds checking is performed; if @p coords are outside the view dimensions,
     * the behavior is undefined.  Use @c at() for bounds-checked access.
     *
     * @code
     * ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * ysc::matrix_view<int, 2, 3> v = m;
     * v(0, 0) = 99;
     * assert(m(0, 0) == 99);
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <class... Coords>
        requires integral_coordinates<Coords...>
    constexpr T& operator()(Coords... coords) noexcept {
        // Intentional: unchecked access on the performance path. Use at() for bounds checking.
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return _ptr[detail::coordinates_to_index(dimensions, std::array{coords...})];
    }

    /**
     * @brief Returns a reference to the element at the given coordinates, with bounds checking.
     * @tparam Coords Integral coordinate types
     * @param coords  Coordinates of the element
     * @return Reference to the element
     * @throws std::out_of_range if any coordinate is negative or out of bounds
     *
     * @code
     * ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * ysc::matrix_view<int, 2, 3> v = m;
     * assert(v.at(1, 2) == 6);
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <class... Coords>
        requires integral_coordinates<Coords...>
    [[nodiscard]] const T& at(Coords... coords) const {
        const bool any_of_coords_is_negative = ((coords < 0) || ...);
        const bool any_of_coords_is_out_of_bound = ((coords >= Dimensions) || ...);
        if (any_of_coords_is_negative || any_of_coords_is_out_of_bound) {
            throw std::out_of_range{"matrix_view::at"};
        }
        return (*this)(coords...);
    }

    /**
     * @brief Returns a reference to the element at the given coordinates, with bounds checking.
     * @tparam Coords Integral coordinate types
     * @param coords  Coordinates of the element
     * @return Reference to the element
     * @throws std::out_of_range if any coordinate is negative or out of bounds
     *
     * @code
     * ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * ysc::matrix_view<int, 2, 3> v = m;
     * v.at(0, 0) = 99;
     * assert(m(0, 0) == 99);
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <class... Coords>
        requires integral_coordinates<Coords...>
    T& at(Coords... coords) {
        const bool any_of_coords_is_negative = ((coords < 0) || ...);
        const bool any_of_coords_is_out_of_bound = ((coords >= Dimensions) || ...);
        if (any_of_coords_is_negative || any_of_coords_is_out_of_bound) {
            throw std::out_of_range{"matrix_view::at"};
        }
        return (*this)(coords...);
    }

    // ─── modifiers ───────────────────────────────────────────────────────────

    /**
     * @brief Assigns the given value to all elements of the view.
     * @param value Value to assign
     *
     * Modifications are reflected in the underlying @c matrix.
     *
     * @code
     * ysc::matrix<int, 3> m{1, 2, 3};
     * ysc::matrix_view<int, 3> v = m;
     * v.fill(0);
     * assert(m(0) == 0 && m(2) == 0);
     * @endcode
     *
     * @ingroup ysc_view
     */
    constexpr void fill(const T& value) noexcept(std::is_nothrow_copy_assignable_v<T>) {
        std::fill(begin(), end(), value);
    }
};

} // namespace ysc

#endif // YSC_MATRIX_VIEW_HPP
