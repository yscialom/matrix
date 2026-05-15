/**
 * @file matrix_view.hpp
 * @author Yankel Scialom (YSC) <yankel-pro@scialom.org>
 * @date 2026
 *
 * @copyright This project is released under GNU Lesser General Public License; see
 *            COPYING and COPYING.LESSER files attached.
 *
 * Non-owning views over a ysc::matrix storage:
 *   - matrix_view<T, contiguous, Dims...> — elements are contiguous in memory
 *   - matrix_view<T, strided,    Dims...> — elements are non-contiguous (strided)
 */
#ifndef YSC_MATRIX_VIEW_HPP
#define YSC_MATRIX_VIEW_HPP

#include <algorithm>
#include <array>
#include <compare>
#include <iterator>
#include <stdexcept>
#include <string>
#include <tuple>

#include <matrix_detail.hpp>

namespace ysc {

/**
 * @defgroup ysc_view Views
 * @brief Non-owning read/write views over @c ysc::matrix storage.
 */

// ─── primary template (not defined — use a specialization) ───────────────────

/**
 * @brief Non-owning view over a @c ysc::matrix storage.
 *
 * Two specializations exist:
 *   - @c matrix_view<T, contiguous, Dims...> — elements contiguous in memory
 *   - @c matrix_view<T, strided,    Dims...> — elements non-contiguous (strided)
 *
 * @tparam T       Element type
 * @tparam Storage Storage tag: @c ysc::contiguous or @c ysc::strided
 * @tparam Dims    Dimensions of the view
 *
 * @ingroup ysc_view
 */
template <class T, class Storage, std::size_t... Dims> class matrix_view;

// ─── contiguous specialization ────────────────────────────────────────────────

/**
 * @brief Non-owning read/write view over a **contiguous** sequence of @c T elements.
 *
 * @tparam T          Element type
 * @tparam Dimensions Dimensions of the view (same as the underlying @c matrix)
 *
 * A @c matrix_view<T,contiguous,Dims...> holds a single raw pointer and the
 * dimension metadata as template parameters.  It does **not** own the memory:
 * the lifetime of the underlying @c matrix (or raw array) must exceed that of
 * the view.
 *
 * @warning Destroying the underlying @c matrix while a view still refers to it
 *          is undefined behavior.
 *
 * Because dimensions are part of the type,
 * @c sizeof(matrix_view<T,contiguous,D...>) equals @c sizeof(T*) on every platform.
 *
 * @code
 * ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
 * ysc::matrix_view<int, ysc::contiguous, 2, 3> v = m;
 * v(0, 0) = 99;           // mutation is reflected in m
 * assert(m(0, 0) == 99);
 * @endcode
 *
 * @ingroup ysc_view
 */
template <class T, std::size_t... Dimensions> class matrix_view<T, contiguous, Dimensions...> {
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
     * Implicit conversion is intentional: `matrix_view<T,contiguous,D...> v = m;`
     * should work as naturally as `std::string_view sv = str;`.
     *
     * @code
     * ysc::matrix<int, 3> m{1, 2, 3};
     * ysc::matrix_view<int, ysc::contiguous, 3> v = m;
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
     * ysc::matrix_view<int, ysc::contiguous, 2, 3> v{buf};
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

    /**
     * @brief Implicit conversion to the corresponding strided view.
     *
     * Every contiguous view is also a valid strided view (with natural strides).
     * This conversion allows passing a contiguous view to any API that accepts a
     * strided view.
     *
     * @code
     * ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * ysc::matrix_view<int, ysc::contiguous, 2, 3> cv = m;
     * ysc::matrix_view<int, ysc::strided,    2, 3> sv = cv;  // implicit conversion
     * @endcode
     *
     * @ingroup ysc_view
     */
    // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
    operator matrix_view<T, strided, Dimensions...>() const noexcept;

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
     * static_assert(ysc::matrix_view<int, ysc::contiguous, 2, 3>::size() == 6);
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
     * No bounds checking is performed; use @c at() for bounds-checked access.
     *
     * @code
     * ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * ysc::matrix_view<int, ysc::contiguous, 2, 3> v = m;
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
     * @brief Returns a mutable reference to the element at the given coordinates.
     * @tparam Coords Integral coordinate types
     * @param coords  Coordinates of the element
     * @return Mutable reference to the element
     *
     * No bounds checking is performed; use @c at() for bounds-checked access.
     *
     * @code
     * ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * ysc::matrix_view<int, ysc::contiguous, 2, 3> v = m;
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
     * @brief Returns a const reference to the element at the given coordinates, with bounds
     *        checking.
     * @tparam Coords Integral coordinate types
     * @param coords  Coordinates of the element
     * @return Const reference to the element
     * @throws std::out_of_range if any coordinate is negative or out of bounds, with a message
     *         of the form:
     *         @code
     *         "matrix_view::at: coordinate <c> is out of bounds for dimension <i> (size=<s>)"
     *         @endcode
     *
     * @code
     * ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * ysc::matrix_view<int, ysc::contiguous, 2, 3> v = m;
     * assert(v.at(1, 2) == 6);
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <class... Coords>
        requires integral_coordinates<Coords...>
    [[nodiscard]] const T& at(Coords... coords) const {
        const std::array<std::ptrdiff_t, sizeof...(Coords)> coords_arr = {
            static_cast<std::ptrdiff_t>(coords)...};
        for (std::size_t i = 0; i < sizeof...(Coords); ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            if (coords_arr[i] < 0 || static_cast<std::size_t>(coords_arr[i]) >= dimensions[i]) {
                throw std::out_of_range(
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    "matrix_view::at: coordinate " + std::to_string(coords_arr[i]) +
                    " is out of bounds for dimension " + std::to_string(i) +
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    " (size=" + std::to_string(dimensions[i]) + ")");
            }
        }
        return (*this)(coords...);
    }

    /**
     * @brief Returns a mutable reference to the element at the given coordinates, with bounds
     *        checking.
     * @tparam Coords Integral coordinate types
     * @param coords  Coordinates of the element
     * @return Mutable reference to the element
     * @throws std::out_of_range if any coordinate is negative or out of bounds, with a message
     *         of the form:
     *         @code
     *         "matrix_view::at: coordinate <c> is out of bounds for dimension <i> (size=<s>)"
     *         @endcode
     *
     * @code
     * ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
     * ysc::matrix_view<int, ysc::contiguous, 2, 3> v = m;
     * v.at(0, 0) = 99;
     * assert(m(0, 0) == 99);
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <class... Coords>
        requires integral_coordinates<Coords...>
    T& at(Coords... coords) {
        const std::array<std::ptrdiff_t, sizeof...(Coords)> coords_arr = {
            static_cast<std::ptrdiff_t>(coords)...};
        for (std::size_t i = 0; i < sizeof...(Coords); ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            if (coords_arr[i] < 0 || static_cast<std::size_t>(coords_arr[i]) >= dimensions[i]) {
                throw std::out_of_range(
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    "matrix_view::at: coordinate " + std::to_string(coords_arr[i]) +
                    " is out of bounds for dimension " + std::to_string(i) +
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    " (size=" + std::to_string(dimensions[i]) + ")");
            }
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
     * ysc::matrix_view<int, ysc::contiguous, 3> v = m;
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

// ─── strided specialization ───────────────────────────────────────────────────

/**
 * @brief Non-owning read/write view over a **non-contiguous** (strided) sequence of elements.
 *
 * @tparam T          Element type
 * @tparam Dimensions Dimensions of the view (kept dimensions only)
 *
 * A @c matrix_view<T,strided,Dims...> holds a raw pointer and a strides array.
 * Elements are accessed as:
 * @code
 * element(c0, c1, ...) == *(_ptr + c0*strides[0] + c1*strides[1] + ...)
 * @endcode
 *
 * @note Iterators and @c data() are not available (non-contiguous layout).
 *       Use @c operator() or @c at() for element access.
 *
 * @warning Destroying the underlying @c matrix while a view still refers to it
 *          is undefined behavior.
 *
 * @code
 * ysc::matrix<int, 3, 4> m{};
 * // Column 2: elements m(0,2), m(1,2), m(2,2) — stride 4
 * auto col2 = m.col(2);   // matrix_view<int, strided, 3>
 * col2(1) = 99;           // writes m(1, 2)
 * assert(m(1, 2) == 99);
 * @endcode
 *
 * @ingroup ysc_view
 */
template <class T, std::size_t... Dimensions> class matrix_view<T, strided, Dimensions...> {
public:
    /** @brief Order of the view (number of dimensions). */
    static constexpr std::size_t order = sizeof...(Dimensions);
    /** @brief Dimensions of the view (kept dimensions only). */
    static constexpr std::array dimensions = {Dimensions...};

private:
    static constexpr std::size_t linear_size = (Dimensions * ...);
    T* _ptr;
    std::array<std::size_t, sizeof...(Dimensions)> _strides;

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

    // ─── strided iterator (random-access, 1-D views) ─────────────────────────

    /**
     * @brief Random-access iterator over a 1-D strided view.
     *
     * Advances by @c _stride elements per step.  Satisfies
     * `std::random_access_iterator` (but not `std::contiguous_iterator`).
     *
     * @ingroup ysc_view
     */
    struct iterator {
        /** @brief Iterator category: random-access (not contiguous). */
        using iterator_category = std::random_access_iterator_tag;
        /** @brief Element type. */
        using value_type = T;
        /** @brief Signed difference type. */
        using difference_type = std::ptrdiff_t;
        /** @brief Pointer type. */
        using pointer = T*;
        /** @brief Reference type. */
        using reference = T&;

        T* _ptr;             ///< Pointer to current element
        std::size_t _stride; ///< Stride in number of T elements

        /** @brief Dereference. */
        reference operator*() const noexcept { return *_ptr; }
        /** @brief Arrow. */
        pointer operator->() const noexcept { return _ptr; }

        /** @brief Pre-increment. */
        iterator& operator++() noexcept {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            _ptr += static_cast<difference_type>(_stride);
            return *this;
        }
        /** @brief Post-increment. */
        iterator operator++(int) noexcept {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }
        /** @brief Pre-decrement. */
        iterator& operator--() noexcept {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            _ptr -= static_cast<difference_type>(_stride);
            return *this;
        }
        /** @brief Post-decrement. */
        iterator operator--(int) noexcept {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        /** @brief Advance by @p n steps. */
        iterator& operator+=(difference_type n) noexcept {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            _ptr += n * static_cast<difference_type>(_stride);
            return *this;
        }
        /** @brief Retreat by @p n steps. */
        iterator& operator-=(difference_type n) noexcept {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            _ptr -= n * static_cast<difference_type>(_stride);
            return *this;
        }
        /** @brief Iterator + n. */
        [[nodiscard]] iterator operator+(difference_type n) const noexcept {
            auto tmp = *this;
            return tmp += n;
        }
        /** @brief n + iterator. */
        [[nodiscard]] friend iterator operator+(difference_type n, iterator it) noexcept {
            return it + n;
        }
        /** @brief Iterator - n. */
        [[nodiscard]] iterator operator-(difference_type n) const noexcept {
            auto tmp = *this;
            return tmp -= n;
        }
        /** @brief Distance between iterators. */
        [[nodiscard]] difference_type operator-(const iterator& other) const noexcept {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            return (_ptr - other._ptr) / static_cast<difference_type>(_stride);
        }
        /** @brief Subscript. */
        reference operator[](difference_type n) const noexcept {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            return *(_ptr + (n * static_cast<difference_type>(_stride)));
        }

        /** @brief Equality comparison. */
        bool operator==(const iterator& other) const noexcept = default;
        /** @brief Three-way comparison (by pointer address). */
        [[nodiscard]] auto operator<=>(const iterator& other) const noexcept {
            return _ptr <=> other._ptr;
        }
    };

    /**
     * @brief Const random-access iterator over a 1-D strided view.
     * @ingroup ysc_view
     */
    struct const_iterator {
        /** @brief Iterator category: random-access (not contiguous). */
        using iterator_category = std::random_access_iterator_tag;
        /** @brief Element type. */
        using value_type = T;
        /** @brief Signed difference type. */
        using difference_type = std::ptrdiff_t;
        /** @brief Pointer type. */
        using pointer = const T*;
        /** @brief Reference type. */
        using reference = const T&;

        const T* _ptr = nullptr; ///< Pointer to current element
        std::size_t _stride = 0; ///< Stride in number of T elements

        const_iterator() noexcept = default;
        const_iterator(const T* ptr, std::size_t stride) noexcept : _ptr{ptr}, _stride{stride} {}
        // NOLINTNEXTLINE(google-explicit-constructor,hicpp-explicit-conversions)
        const_iterator(iterator it) noexcept : _ptr{it._ptr}, _stride{it._stride} {}

        /** @brief Dereference. */
        reference operator*() const noexcept { return *_ptr; }
        /** @brief Arrow. */
        pointer operator->() const noexcept { return _ptr; }

        /** @brief Pre-increment. */
        const_iterator& operator++() noexcept {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            _ptr += static_cast<difference_type>(_stride);
            return *this;
        }
        /** @brief Post-increment. */
        const_iterator operator++(int) noexcept {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }
        /** @brief Pre-decrement. */
        const_iterator& operator--() noexcept {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            _ptr -= static_cast<difference_type>(_stride);
            return *this;
        }
        /** @brief Post-decrement. */
        const_iterator operator--(int) noexcept {
            auto tmp = *this;
            --(*this);
            return tmp;
        }

        /** @brief Advance by @p n steps. */
        const_iterator& operator+=(difference_type n) noexcept {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            _ptr += n * static_cast<difference_type>(_stride);
            return *this;
        }
        /** @brief Retreat by @p n steps. */
        const_iterator& operator-=(difference_type n) noexcept {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            _ptr -= n * static_cast<difference_type>(_stride);
            return *this;
        }
        /** @brief Iterator + n. */
        [[nodiscard]] const_iterator operator+(difference_type n) const noexcept {
            auto tmp = *this;
            return tmp += n;
        }
        /** @brief n + const_iterator. */
        [[nodiscard]] friend const_iterator operator+(difference_type n,
                                                      const_iterator it) noexcept {
            return it + n;
        }
        /** @brief Iterator - n. */
        [[nodiscard]] const_iterator operator-(difference_type n) const noexcept {
            auto tmp = *this;
            return tmp -= n;
        }
        /** @brief Distance between iterators. */
        [[nodiscard]] difference_type operator-(const const_iterator& other) const noexcept {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            return (_ptr - other._ptr) / static_cast<difference_type>(_stride);
        }
        /** @brief Subscript. */
        reference operator[](difference_type n) const noexcept {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            return *(_ptr + (n * static_cast<difference_type>(_stride)));
        }

        /** @brief Equality comparison. */
        bool operator==(const const_iterator& other) const noexcept = default;
        /** @brief Three-way comparison (by pointer address). */
        [[nodiscard]] auto operator<=>(const const_iterator& other) const noexcept {
            return _ptr <=> other._ptr;
        }
    };

    // ─── construction ────────────────────────────────────────────────────────

    matrix_view() = delete;

    /**
     * @brief Constructs a strided view from a raw pointer and a strides array.
     * @param ptr     Pointer to the first element of the view
     * @param strides Stride for each dimension (in elements, not bytes)
     *
     * @code
     * // View every other element of a 1D array as a 1D strided view of size 3
     * int buf[6]{1, 2, 3, 4, 5, 6};
     * ysc::matrix_view<int, ysc::strided, 3> v{buf, {2}};
     * assert(v(0) == 1 && v(1) == 3 && v(2) == 5);
     * @endcode
     *
     * @ingroup ysc_view
     */
    constexpr explicit matrix_view(T* ptr,
                                   std::array<std::size_t, sizeof...(Dimensions)> strides) noexcept
        : _ptr{ptr}, _strides{strides} {}

    /** @brief Copies the view (shallow: copies the pointer and strides, not the data). */
    constexpr matrix_view(const matrix_view&) noexcept = default;
    /** @brief Copy-assigns the view (shallow). */
    constexpr matrix_view& operator=(const matrix_view&) noexcept = default;
    /** @brief Move-constructs the view. */
    constexpr matrix_view(matrix_view&&) noexcept = default;
    /** @brief Move-assigns the view. */
    constexpr matrix_view& operator=(matrix_view&&) noexcept = default;
    /** @brief Destructor (does not free the underlying storage). */
    ~matrix_view() = default;

    // ─── capacity ────────────────────────────────────────────────────────────

    /**
     * @brief Returns the number of elements in the view (product of all kept dimensions).
     * @return Number of elements
     *
     * @note This function is @c static because the size is a compile-time constant.
     *
     * @code
     * static_assert(ysc::matrix_view<int, ysc::strided, 3, 4>::size() == 12);
     * @endcode
     *
     * @ingroup ysc_view
     */
    [[nodiscard]] static constexpr size_type size() noexcept { return linear_size; }

    /**
     * @brief Returns the maximum number of elements the view can address.
     * @return Maximum number of elements (always equal to @c size())
     * @ingroup ysc_view
     */
    [[nodiscard]] static constexpr size_type max_size() noexcept { return linear_size; }

    /**
     * @brief Returns whether the view has no elements.
     * @return @c true if @c linear_size == 0
     * @ingroup ysc_view
     */
    [[nodiscard]] static constexpr bool empty() noexcept { return linear_size == 0; }

    // ─── iterators (1-D strided views only) ──────────────────────────────────

    /**
     * @brief Returns an iterator to the first element of a 1-D strided view.
     * @return Iterator to the first element
     *
     * Only available for 1-D views (@c order == 1).
     *
     * @code
     * ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
     * auto col = m.col(1);  // strided view of column 1
     * int sum = std::accumulate(col.begin(), col.end(), 0);
     * @endcode
     *
     * @ingroup ysc_view
     */
    iterator begin() noexcept
        requires(order == 1)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        return {_ptr, _strides[0]};
    }

    /**
     * @brief Returns a const iterator to the first element of a 1-D strided view.
     * @return Const iterator to the first element
     * @ingroup ysc_view
     */
    [[nodiscard]] const_iterator begin() const noexcept
        requires(order == 1)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        return {_ptr, _strides[0]};
    }

    /**
     * @brief Returns a const iterator to the first element of a 1-D strided view.
     * @return Const iterator to the first element
     * @ingroup ysc_view
     */
    [[nodiscard]] const_iterator cbegin() const noexcept
        requires(order == 1)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        return {_ptr, _strides[0]};
    }

    /**
     * @brief Returns an iterator past the last element of a 1-D strided view.
     * @return Iterator past the last element
     * @ingroup ysc_view
     */
    iterator end() noexcept
        requires(order == 1)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index,cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return {_ptr + static_cast<difference_type>(linear_size * _strides[0]), _strides[0]};
    }

    /**
     * @brief Returns a const iterator past the last element of a 1-D strided view.
     * @return Const iterator past the last element
     * @ingroup ysc_view
     */
    [[nodiscard]] const_iterator end() const noexcept
        requires(order == 1)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index,cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return {_ptr + static_cast<difference_type>(linear_size * _strides[0]), _strides[0]};
    }

    /**
     * @brief Returns a const iterator past the last element of a 1-D strided view.
     * @return Const iterator past the last element
     * @ingroup ysc_view
     */
    [[nodiscard]] const_iterator cend() const noexcept
        requires(order == 1)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index,cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return {_ptr + static_cast<difference_type>(linear_size * _strides[0]), _strides[0]};
    }

    // ─── element access ──────────────────────────────────────────────────────

    /**
     * @brief Returns a const reference to the element at the given coordinates.
     * @tparam Coords Integral coordinate types
     * @param coords  Coordinates of the element
     * @return Const reference to the element
     *
     * No bounds checking is performed; use @c at() for bounds-checked access.
     * The element is located at: @code _ptr + c0*strides[0] + c1*strides[1] + ... @endcode
     *
     * @code
     * ysc::matrix<int, 3, 4> m{};
     * auto col = m.col(2);   // strided view of column 2
     * assert(col(1) == m(1, 2));
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <class... Coords>
        requires integral_coordinates<Coords...> && (sizeof...(Coords) == sizeof...(Dimensions))
    [[nodiscard]] constexpr const T& operator()(Coords... coords) const noexcept {
        const std::array<std::size_t, sizeof...(Dimensions)> c = {
            static_cast<std::size_t>(coords)...};
        std::size_t off = 0;
        for (std::size_t i = 0; i < sizeof...(Dimensions); ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            off += c[i] * _strides[i];
        }
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return *(_ptr + off);
    }

    /**
     * @brief Returns a mutable reference to the element at the given coordinates.
     * @tparam Coords Integral coordinate types
     * @param coords  Coordinates of the element
     * @return Mutable reference to the element
     *
     * No bounds checking is performed; use @c at() for bounds-checked access.
     * The element is located at: @code _ptr + c0*strides[0] + c1*strides[1] + ... @endcode
     *
     * @code
     * ysc::matrix<int, 3, 4> m{};
     * auto col = m.col(2);   // strided view of column 2
     * col(1) = 99;           // writes m(1, 2)
     * assert(m(1, 2) == 99);
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <class... Coords>
        requires integral_coordinates<Coords...> && (sizeof...(Coords) == sizeof...(Dimensions))
    [[nodiscard]] constexpr T& operator()(Coords... coords) noexcept {
        const std::array<std::size_t, sizeof...(Dimensions)> c = {
            static_cast<std::size_t>(coords)...};
        std::size_t off = 0;
        for (std::size_t i = 0; i < sizeof...(Dimensions); ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            off += c[i] * _strides[i];
        }
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return *(_ptr + off);
    }

    /**
     * @brief Returns a const reference to the element at the given coordinates, with bounds
     *        checking.
     * @tparam Coords Integral coordinate types
     * @param coords  Coordinates of the element
     * @return Const reference to the element
     * @throws std::out_of_range if any coordinate is negative or out of bounds, with a message
     *         of the form:
     *         @code
     *         "matrix_view::at: coordinate <c> is out of bounds for dimension <i> (size=<s>)"
     *         @endcode
     *
     * @code
     * ysc::matrix<int, 3, 4> m{};
     * const auto col = m.col(2);
     * assert(col.at(0) == m(0, 2));
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <class... Coords>
        requires integral_coordinates<Coords...> && (sizeof...(Coords) == sizeof...(Dimensions))
    [[nodiscard]] const T& at(Coords... coords) const {
        const std::array<std::ptrdiff_t, sizeof...(Coords)> coords_arr = {
            static_cast<std::ptrdiff_t>(coords)...};
        for (std::size_t i = 0; i < sizeof...(Coords); ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            if (coords_arr[i] < 0 || static_cast<std::size_t>(coords_arr[i]) >= dimensions[i]) {
                throw std::out_of_range(
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    "matrix_view::at: coordinate " + std::to_string(coords_arr[i]) +
                    " is out of bounds for dimension " + std::to_string(i) +
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    " (size=" + std::to_string(dimensions[i]) + ")");
            }
        }
        return (*this)(coords...);
    }

    /**
     * @brief Returns a mutable reference to the element at the given coordinates, with bounds
     *        checking.
     * @tparam Coords Integral coordinate types
     * @param coords  Coordinates of the element
     * @return Mutable reference to the element
     * @throws std::out_of_range if any coordinate is negative or out of bounds, with a message
     *         of the form:
     *         @code
     *         "matrix_view::at: coordinate <c> is out of bounds for dimension <i> (size=<s>)"
     *         @endcode
     *
     * @code
     * ysc::matrix<int, 3, 4> m{};
     * auto col = m.col(2);
     * col.at(1) = 99;
     * assert(m(1, 2) == 99);
     * @endcode
     *
     * @ingroup ysc_view
     */
    template <class... Coords>
        requires integral_coordinates<Coords...> && (sizeof...(Coords) == sizeof...(Dimensions))
    [[nodiscard]] T& at(Coords... coords) {
        const std::array<std::ptrdiff_t, sizeof...(Coords)> coords_arr = {
            static_cast<std::ptrdiff_t>(coords)...};
        for (std::size_t i = 0; i < sizeof...(Coords); ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            if (coords_arr[i] < 0 || static_cast<std::size_t>(coords_arr[i]) >= dimensions[i]) {
                throw std::out_of_range(
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    "matrix_view::at: coordinate " + std::to_string(coords_arr[i]) +
                    " is out of bounds for dimension " + std::to_string(i) +
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    " (size=" + std::to_string(dimensions[i]) + ")");
            }
        }
        return (*this)(coords...);
    }

    // ─── modifiers ───────────────────────────────────────────────────────────

    /**
     * @brief Returns a reference to the first element (all coordinates zero).
     *
     * Calling @c front() on an empty view is undefined behavior.
     *
     * @code
     * ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
     * auto col = m.col(1);
     * assert(col.front() == m(0, 1));
     * @endcode
     *
     * @ingroup ysc_view
     */
    constexpr reference front() noexcept { return (*this)(((void)Dimensions, std::size_t{0})...); }

    /**
     * @brief Returns a const reference to the first element (all coordinates zero).
     *
     * Calling @c front() on an empty view is undefined behavior.
     *
     * @ingroup ysc_view
     */
    [[nodiscard]] constexpr const_reference front() const noexcept {
        return (*this)(((void)Dimensions, std::size_t{0})...);
    }

    /**
     * @brief Returns a reference to the last element (each coordinate at its maximum).
     *
     * Calling @c back() on an empty view is undefined behavior.
     *
     * @code
     * ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
     * auto col = m.col(1);
     * assert(col.back() == m(2, 1));
     * @endcode
     *
     * @ingroup ysc_view
     */
    constexpr reference back() noexcept { return (*this)((Dimensions - 1)...); }

    /**
     * @brief Returns a const reference to the last element (each coordinate at its maximum).
     *
     * Calling @c back() on an empty view is undefined behavior.
     *
     * @ingroup ysc_view
     */
    [[nodiscard]] constexpr const_reference back() const noexcept {
        return (*this)((Dimensions - 1)...);
    }

    /**
     * @brief Assigns @p value to every element of the strided view.
     * @param value Value to assign
     *
     * Iterates over all logical indices and writes through @c operator(),
     * so each element is visited exactly once regardless of strides.
     * Modifications are reflected in the underlying @c matrix.
     *
     * @code
     * ysc::matrix<int, 3, 4> m{};
     * auto col = m.col(2);
     * col.fill(7);
     * assert(m(1, 2) == 7);
     * @endcode
     *
     * @ingroup ysc_view
     */
    constexpr void fill(const T& value) noexcept(std::is_nothrow_copy_assignable_v<T>) {
        for (std::size_t k = 0; k < linear_size; ++k) {
            const auto coords = detail::index_to_coordinates(dimensions, k);
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            std::apply([&](auto... c) { (*this)(c...) = value; }, coords);
        }
    }
};

// ─── contiguous → strided conversion (defined here, after strided is complete) ─

/**
 * @brief Converts a contiguous view to the equivalent strided view.
 *
 * The natural strides of a row-major contiguous view are computed at compile time.
 * Stride at dimension @c i = product of all dimensions after @c i.
 *
 * @ingroup ysc_view
 */
template <class T, std::size_t... Dimensions>
matrix_view<T, contiguous, Dimensions...>::operator matrix_view<T, strided, Dimensions...>()
    const noexcept {
    constexpr std::array<std::size_t, sizeof...(Dimensions)> dims_arr = {Dimensions...};
    std::array<std::size_t, sizeof...(Dimensions)> strides{};
    for (std::size_t i = 0; i < sizeof...(Dimensions); ++i) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
        strides[i] = 1;
        for (std::size_t j = i + 1; j < sizeof...(Dimensions); ++j) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            strides[i] *= dims_arr[j];
        }
    }
    return matrix_view<T, strided, Dimensions...>{_ptr, strides};
}

} // namespace ysc

#endif // YSC_MATRIX_VIEW_HPP
