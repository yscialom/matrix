# `ysc::matrix`

[![CI](https://github.com/yscialom/matrix/actions/workflows/ci.yml/badge.svg)](https://github.com/yscialom/matrix/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/yscialom/matrix/graph/badge.svg)](https://codecov.io/gh/yscialom/matrix)
[![docs](https://github.com/yscialom/matrix/actions/workflows/docs.yml/badge.svg)](https://yscialom.github.io/matrix/)

A general-purpose multi-dimension container of static dimensions. Requires **C++20** (`__cplusplus >= 202002L`).

## Example


    #include <matrix.hpp>
    #include <iostream>
    #include <string>

    int main()
    {
        ysc::matrix<int, 3, 3> const data = {
            1, 2, 3,
            4, 5, 6,
            7, 8, 9
        };

        std::cout << data(0, 0) + data(1, 1) + data(2, 2) << '\n';

        ysc::matrix<std::string, 3, 3> data_as_string;
        for (int i=0 ; i < data_as_string.dimensions[0] ; ++i) {
            for (int j=0 ; j < data_as_string.dimensions[1] ; ++j) {
                data_as_string(i, j) = std::to_string(data(i, j));
            }
        }

        std::cout << "|_log(data(1, 1))_| is " << data_as_string(1, 1).size() << ".\n";
    }
    
## Feature

todo...
