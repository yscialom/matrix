#include <matrix.hpp>

// Explicit instantiations force the compiler to emit all non-template member
// functions for these specialisations. Without this, gcov only tracks lines
// that are actually called in tests, making 100% coverage trivially easy on
// a header-only template library.
template class ysc::matrix<int, 3, 4>;
template class ysc::matrix<double, 2, 3, 4>;
template class ysc::matrix<float, 10>;
struct Scalar {};
template class ysc::matrix<struct Scalar, 1>;
