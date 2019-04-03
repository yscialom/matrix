##
# Tets framework: gtest
#
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIR})
include_directories(include)


##
# Test definition
#
set(TARGET_NAME matrix-test)
add_executable(${TARGET_NAME}
    src/access.cpp
    src/construct.cpp
    src/main.cpp
)

target_link_libraries(${TARGET_NAME} matrix)
target_link_libraries(${TARGET_NAME} gtest)


##
# Test run
#
add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
