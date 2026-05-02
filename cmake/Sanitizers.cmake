option(ENABLE_SANITIZERS "Enable ASan + UBSan instrumentation (Clang/GCC only)" OFF)

if(ENABLE_SANITIZERS)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        message(FATAL_ERROR
            "ENABLE_SANITIZERS=ON requires GCC or Clang. "
            "Current compiler: ${CMAKE_CXX_COMPILER_ID}")
    endif()

    message(STATUS "Sanitizer instrumentation enabled (ASan + UBSan)")

    target_compile_options(matrix-test PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
        -g
    )
    target_link_options(matrix-test PRIVATE
        -fsanitize=address,undefined
    )
    target_compile_definitions(matrix-test PRIVATE YSC_SANITIZERS_ENABLED)
endif()
