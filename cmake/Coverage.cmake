option(ENABLE_COVERAGE "Enable code coverage instrumentation (GCC/Clang only)" OFF)

if(ENABLE_COVERAGE)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR
            "ENABLE_COVERAGE=ON requires GCC or Clang. "
            "Current compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()

    message(STATUS "Coverage instrumentation enabled")

    target_compile_options(matrix-test PRIVATE
        -O0
        -g
        --coverage
        -fprofile-arcs
        -ftest-coverage
    )
    target_link_options(matrix-test PRIVATE
        --coverage
    )
endif()
