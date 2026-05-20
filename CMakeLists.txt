cmake_minimum_required(VERSION 3.20)

##
# general configuration
#
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   7   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   2   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

project(ysc-matrix
    VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}
    LANGUAGES CXX
)


##
# top-level detection (auto-disable tests/docs when used via FetchContent)
#
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(YSC_MATRIX_IS_TOP_LEVEL ON)
else()
    set(YSC_MATRIX_IS_TOP_LEVEL OFF)
endif()

option(YSC_MATRIX_BUILD_TESTING       "Build tests"       ${YSC_MATRIX_IS_TOP_LEVEL})
option(YSC_MATRIX_BUILD_DOCUMENTATION "Build Doxygen doc" ${YSC_MATRIX_IS_TOP_LEVEL})
option(YSC_MATRIX_BUILD_EXAMPLES      "Build examples"    OFF)


##
# source configuration
#
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(src)


##
# test configuration
#
if(YSC_MATRIX_BUILD_TESTING)
    include(cmake/ClangFormat.cmake)
    enable_testing()
    add_subdirectory(test)
    include(cmake/Coverage.cmake)
    include(cmake/Sanitizers.cmake)
    add_custom_target(check # alias for make && make test
        COMMAND ${CMAKE_CTEST_COMMAND}
        DEPENDS matrix-test
    )
endif()


##
# benchmark configuration
#
option(BUILD_BENCHMARKS "Build benchmarks" OFF)
if(BUILD_BENCHMARKS)
    add_subdirectory(bench)
endif()


##
# doc generation
#
if(YSC_MATRIX_BUILD_DOCUMENTATION)
    find_package(Doxygen REQUIRED)

    include(cmake/DoxygenAwesome.cmake)

    set(doxyfile_in    ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(doxyfile       ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)
    set(doxy_main_page ${CMAKE_CURRENT_SOURCE_DIR}/doc/mainpage.md)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc OPTIONAL)
endif()
