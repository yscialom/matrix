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
add_subdirectory(test)
enable_testing()
#add_test(NAME ${TARGET_NAME}_test COMMAND test)
