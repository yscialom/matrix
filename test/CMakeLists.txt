##
# Tets framework: gtest
#
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})


##
# Test definition
#
set(TARGET_NAME matrix-test)
add_executable(${TARGET_NAME}
    src/dummy.cpp
)
target_link_libraries(${TARGET_NAME} matrix)
target_link_libraries(${TARGET_NAME} gtest)


##
# Test run
#
add_test(NAME boby COMMAND ${TARGET_NAME})
