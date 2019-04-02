cmake_minimum_required(VERSION 3.0)

##
# general configuration
#
set(TARGET_NAME "ysc::matrix")
project(${TARGET_NAME})

set(VERSION_MAJOR   1   CACHE STRING "Project major version number.")
set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)


##
# source configuration
#
set(CMAKE_CXX_STANDARD 17)
add_subdirectory(src)


##
# test configuration
#
enable_testing()
add_subdirectory(test)
add_custom_target(check # alias for make && make test
    COMMAND ${CMAKE_CTEST_COMMAND}
    DEPENDS ${TARGET_NAME}-test
)


##
# doc generation
#
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxyfile_in    ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(doxyfile       ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)
    set(doxy_main_page ${CMAKE_CURRENT_SOURCE_DIR}/README.md)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc)
endif()
