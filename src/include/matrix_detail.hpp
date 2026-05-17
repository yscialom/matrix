/**
 * @file matrix_detail.hpp
 * @author Yankel Scialom (YSC) <yankel-pro@scialom.org>
 * @date 2026
 *
 * @copyright This project is released under GNU Lesser General Public License; see
 *            COPYING and COPYING.LESSER files attached.
 *
 * Shared types and helpers used by both matrix.hpp and matrix_view.hpp.
 * Users should not include this header directly; include matrix.hpp instead.
 */
#ifndef YSC_MATRIX_DETAIL_HPP
#define YSC_MATRIX_DETAIL_HPP

#include <array>
#include <concepts>
#include <cstddef>
#include <ostream>
#include <type_traits>

namespace ysc {

// ─── storage tags ────────────────────────────────────────────────────────────

/** @brief Storage tag: view elements are contiguous in memory. @ingroup ysc_view */
struct contiguous {};
/** @brief Storage tag: view elements are non-contiguous (strided) in memory. @ingroup ysc_view */
struct strided {};

// ─── slice sentinel ──────────────────────────────────────────────────────────

/**
 * @brief Sentinel type used in slice() to indicate "keep this dimension".
 * @ingroup ysc_view
 */
struct all_t {};

/**
 * @brief Sentinel value: pass ysc::all to slice() to keep a dimension unchanged.
 *
 * @code
 * ysc::matrix<int, 3, 4, 5> m{};
 * auto v = m.slice(ysc::all, 2);  // keeps dims 0 and 2, fixes dim 1 at index 2
 * @endcode
 *
 * @ingroup ysc_view
 */
inline constexpr all_t all{};

// ─── forward declarations ─────────────────────────────────────────────────────

template <class T, std::size_t... Dims> class matrix;

template <class T, class Storage, std::size_t... Dims> class matrix_view;

// ─── concepts ─────────────────────────────────────────────────────────────────

/**
 * @brief Satisfied when all types in @c Coords are integral (ignoring cv-ref).
 * Used to constrain @c operator() and @c at() on matrix and matrix_view.
 */
template <class... Coords>
concept integral_coordinates = (std::integral<std::remove_cvref_t<Coords>> && ...);

// ─── detail namespace ─────────────────────────────────────────────────────────

namespace detail {

template <class T>
concept ostream_streamable = requires(std::ostream& os, const T& v) { os << v; };

// Print a hyperslice starting at `it`, covering dimension `dim_idx` onward.
// Returns an iterator past the last element printed.
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

// Inverse of coordinates_to_index: converts a flat row-major index to multi-dimensional
// coordinates. Rightmost coordinate varies fastest (row-major).
template <std::size_t N>
constexpr std::array<std::size_t, N> index_to_coordinates(std::array<std::size_t, N> dims,
                                                          std::size_t k) noexcept {
    std::array<std::size_t, N> coords{};
    auto it_c = coords.rbegin();
    auto it_d = dims.crbegin();
    for (; it_c != coords.rend(); ++it_c, ++it_d) {
        *it_c = k % *it_d;
        k /= *it_d;
    }
    return coords;
}

// ─── slice metaprogramming helpers ───────────────────────────────────────────

/** @brief True iff @c S is @c ysc::all_t. */
template <class S> inline constexpr bool is_all_v = std::same_as<S, all_t>;

/** @brief Number of @c all_t values in @c Specs... */
template <class... Specs>
inline constexpr std::size_t n_kept_v = (std::size_t{0} + ... + (is_all_v<Specs> ? 1 : 0));

/** @brief Non-type parameter pack container for @c std::size_t values. */
template <std::size_t... Vals> struct size_seq {};

/** @brief Prepend value @c V to a @c size_seq. */
template <std::size_t V, class Seq> struct prepend_val;
template <std::size_t V, std::size_t... Vals> struct prepend_val<V, size_seq<Vals...>> {
    using type = size_seq<V, Vals...>;
};

/** @brief Pad @c Specs... with @c all_t on the right until @c N elements total.
 *  Returns a @c std::tuple of the padded types. */
template <std::size_t ToAdd, class... Specs> struct pad_right_with_all {
    using type = typename pad_right_with_all<ToAdd - 1, Specs..., all_t>::type;
};
template <class... Specs> struct pad_right_with_all<0, Specs...> {
    using type = std::tuple<Specs...>;
};
template <std::size_t N, class... Specs>
    requires(sizeof...(Specs) <= N)
using pad_right_with_all_t = typename pad_right_with_all<N - sizeof...(Specs), Specs...>::type;

/** @brief True iff the padded spec pattern is @c (integrals)*(all*). */
template <class PaddedTuple> struct is_prefix_slice_helper;
template <class... PaddedSpecs> struct is_prefix_slice_helper<std::tuple<PaddedSpecs...>> {
    static constexpr bool value = [] {
        bool seen_all = false;
        bool ok = true;
        ((seen_all = seen_all || is_all_v<PaddedSpecs>,
          ok = ok && (!seen_all || is_all_v<PaddedSpecs>)),
         ...);
        return ok;
    }();
};
template <class PaddedTuple>
inline constexpr bool is_prefix_slice_v = is_prefix_slice_helper<PaddedTuple>::value;

/** @brief Produces a @c size_seq of dims at positions where the spec is @c all_t. */
template <class PaddedTuple, std::size_t... AllDims> struct filter_kept_dims;
template <> struct filter_kept_dims<std::tuple<>> {
    using type = size_seq<>;
};
template <class Spec, class... RestSpecs, std::size_t Dim, std::size_t... RestDims>
struct filter_kept_dims<std::tuple<Spec, RestSpecs...>, Dim, RestDims...> {
    using rest_type = typename filter_kept_dims<std::tuple<RestSpecs...>, RestDims...>::type;
    using type =
        std::conditional_t<is_all_v<Spec>, typename prepend_val<Dim, rest_type>::type, rest_type>;
};

/** @brief Builds @c matrix_view<T,Storage,KeptDims...> from a @c size_seq. */
template <class T, class Storage, class KeptSizeSeq> struct make_matrix_view;
template <class T, class Storage, std::size_t... KeptDims>
struct make_matrix_view<T, Storage, size_seq<KeptDims...>> {
    using type = matrix_view<T, Storage, KeptDims...>;
};
template <class T, class Storage, class KeptSizeSeq>
using make_matrix_view_t = typename make_matrix_view<T, Storage, KeptSizeSeq>::type;

/**
 * @brief Centralises runtime offset and stride computations for @c slice().
 * @tparam PaddedTuple @c std::tuple of padded spec types (all_t or integral)
 */
template <class PaddedTuple> struct slice_helper;
template <class... PaddedSpecs> struct slice_helper<std::tuple<PaddedSpecs...>> {
    static constexpr std::size_t n_specs = sizeof...(PaddedSpecs);
    static constexpr std::size_t n_kept = n_kept_v<PaddedSpecs...>;
    static constexpr std::array<bool, n_specs> is_kept = {is_all_v<PaddedSpecs>...};
    static constexpr std::array<bool, n_specs> is_fixed = {!is_all_v<PaddedSpecs>...};

    /**
     * @brief Computes the flat-array offset of the slice's base pointer.
     * @param dims      Original matrix dimensions
     * @param spec_vals Index values for fixed specs (0 for all_t positions)
     * @return Flat offset from the matrix's data() pointer
     */
    [[nodiscard]] static constexpr std::size_t
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    offset(std::array<std::size_t, n_specs> dims,
           std::array<std::size_t, n_specs> spec_vals) noexcept {
        std::size_t result = 0;
        for (std::size_t i = 0; i < n_specs; ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            if (is_fixed[i]) {
                std::size_t stride = 1;
                for (std::size_t j = i + 1; j < n_specs; ++j) {
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    stride *= dims[j];
                }
                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                result += spec_vals[i] * stride;
            }
        }
        return result;
    }

    /**
     * @brief Computes the strides array for a strided view.
     *
     * Stride at position k (for a kept dim) = product of original dims after position k.
     *
     * @param dims Original matrix dimensions
     * @return Array of strides, one per kept dimension
     */
    [[nodiscard]] static constexpr std::array<std::size_t, n_kept>
    strides(std::array<std::size_t, n_specs> dims) noexcept {
        std::array<std::size_t, n_kept> result{};
        std::size_t k = 0;
        for (std::size_t i = 0; i < n_specs; ++i) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
            if (is_kept[i]) {
                std::size_t stride = 1;
                for (std::size_t j = i + 1; j < n_specs; ++j) {
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                    stride *= dims[j];
                }
                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
                result[k++] = stride;
            }
        }
        return result;
    }
};

} // namespace detail

/**
 * @brief Alias for a read-only contiguous view over a @c ysc::matrix.
 * @tparam T    Element type (non-const; the alias adds @c const automatically)
 * @tparam Dims Dimensions of the view
 *
 * @c const_matrix_view<T, Dims...> is shorthand for
 * @c matrix_view<const T, contiguous, Dims...>.  It is the natural read-only
 * counterpart of @c matrix_view, analogous to @c std::string_view vs
 * @c std::string.
 *
 * @code
 * const ysc::matrix<int, 3, 3> m{1, 2, 3, 4, 5, 6, 7, 8, 9};
 * ysc::const_matrix_view<int, 3, 3> v{m};  // read-only view
 * assert(v(0, 0) == 1);
 * @endcode
 *
 * @ingroup ysc_view
 */
template <class T, std::size_t... Dims>
using const_matrix_view = matrix_view<const T, contiguous, Dims...>;

} // namespace ysc

#endif // YSC_MATRIX_DETAIL_HPP
