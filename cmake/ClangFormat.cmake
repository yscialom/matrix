find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format)

if(CLANG_FORMAT_EXECUTABLE)
    file(GLOB_RECURSE ALL_SOURCES
        ${CMAKE_SOURCE_DIR}/src/*.hpp
        ${CMAKE_SOURCE_DIR}/test/include/*.hpp
        ${CMAKE_SOURCE_DIR}/test/src/*.cpp
    )
    add_custom_target(format
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${ALL_SOURCES}
        COMMENT "Formatting source files with clang-format"
        VERBATIM
    )
    message(STATUS "clang-format found: ${CLANG_FORMAT_EXECUTABLE} -- target 'format' available")
else()
    message(STATUS "clang-format not found -- target 'format' unavailable")
endif()
