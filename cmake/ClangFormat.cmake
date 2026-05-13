find_program(DOCKER_EXECUTABLE NAMES docker)

if(DOCKER_EXECUTABLE)
    file(GLOB_RECURSE ALL_SOURCES
        ${CMAKE_SOURCE_DIR}/src/*.hpp
        ${CMAKE_SOURCE_DIR}/test/include/*.hpp
        ${CMAKE_SOURCE_DIR}/test/src/*.cpp
    )

    set(CONTAINER_SOURCES "")
    foreach(ABS_PATH IN LISTS ALL_SOURCES)
        file(RELATIVE_PATH REL_PATH ${CMAKE_SOURCE_DIR} ${ABS_PATH})
        list(APPEND CONTAINER_SOURCES "/mnt/${REL_PATH}")
    endforeach()

    set(CLANG_FORMAT_DOCKERFILE ${CMAKE_SOURCE_DIR}/utils/clang-format/Dockerfile)
    set(CLANG_FORMAT_IMAGE ysc-matrix-clang-format)

    execute_process(COMMAND id -u OUTPUT_VARIABLE _docker_uid OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND id -g OUTPUT_VARIABLE _docker_gid OUTPUT_STRIP_TRAILING_WHITESPACE)

    add_custom_target(format
        COMMAND ${DOCKER_EXECUTABLE} build
            -t ${CLANG_FORMAT_IMAGE}
            -f ${CLANG_FORMAT_DOCKERFILE}
            ${CMAKE_SOURCE_DIR}/utils/clang-format
        COMMAND ${DOCKER_EXECUTABLE} run --rm
            --user ${_docker_uid}:${_docker_gid}
            -v ${CMAKE_SOURCE_DIR}:/mnt
            ${CLANG_FORMAT_IMAGE}
            -i ${CONTAINER_SOURCES}
        COMMENT "Formatting source files with clang-format (Docker)"
        VERBATIM
    )
    message(STATUS "docker found: ${DOCKER_EXECUTABLE} -- target 'format' available (via Docker clang-format 18.1.8)")
else()
    message(STATUS "docker not found -- target 'format' unavailable")
endif()
