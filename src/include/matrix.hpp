#ifndef YSC_MATRIX_HPP
#define YSC_MATRIX_HPP

#include <array>
#include <numeric>

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
        std::array<std::size_t, dimensions.size()> dimension_product;
        using std::crbegin, std::crend, std::prev;
        partial_product(crbegin(dimensions), prev(crend(dimensions)), begin(dimension_product));
        return std::inner_product(cbegin(dimension_product), cend(dimension_product), crbegin(coords), 0);
    }
}

constexpr struct matrix_zero_t {} matrix_zero;

template<class T, std::size_t... Dimensions>
class matrix
{
template<class, std::size_t...> friend class matrix;

public:
    static constexpr std::size_t order      = sizeof...(Dimensions);
    static constexpr std::array  dimensions = { Dimensions... };

private:
    static constexpr std::size_t linear_size = (Dimensions * ...);
    std::array<T, linear_size> _data;

public:
    friend void swap(matrix& lhs, matrix& rhs)
    {
        using std::swap;
        swap(lhs._data, rhs._data);
    }

public:
    // default constructor
    matrix()                          = default;

    // value constructors
    matrix(matrix_zero_t) : _data({}) {}

    template<class ... Args>
    matrix(Args&& ... args)
        : _data{std::forward<Args>(args)...}
    {}

    // copy constructors
    matrix(matrix const& other) = default;

    template<class U>
    matrix(matrix<U, Dimensions...> const& other)
    { std::copy(cbegin(other._data), cend(other._data), begin(_data)); }

    // move constructors
    matrix(matrix&& other)            = default;
    matrix& operator=(matrix&& other) = default;

    template<class U>
    matrix& operator=(matrix<U, Dimensions...> const& other)
    {
        matrix o{other};
        swap(*this, o);
        return *this;
    }

public:
    template<class... Args>
    T const& operator()(Args... coordinates) const
    { return _data[_details::coordinates_to_index(dimensions, std::array{coordinates...})]; }

    template<class... Args>
    T& operator()(Args... coordinates)
    { return _data[_details::coordinates_to_index(dimensions, std::array{coordinates...})]; }
};
} // namespace ysc

#endif // YSC_MATRIX_HPP
