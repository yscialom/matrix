#include <matrix.hpp>

int main()
{
    ysc::matrix<unsigned, 1> m;
    m(0) = 0;
    return m(0);
}
