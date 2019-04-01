cmake_minimum_required(VERSION 3.0)

##
# general configuration
#
set(TARGET_NAME matrix)
project(${TARGET_NAME})


##
# source configuration
#
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)
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
