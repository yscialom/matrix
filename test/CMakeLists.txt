##
# Tests framework: gtest
#
find_package(GTest REQUIRED)
include_directories(include)


##
# Test definition
#
set(TARGET_NAME matrix-test)
add_executable(${TARGET_NAME}
    src/access.cpp
    src/assignment_returns_self.cpp
    src/comparison.cpp
    src/construct.cpp
    src/iterators.cpp
    src/size_data.cpp
    src/typedefs.cpp
    src/main.cpp
)

target_link_libraries(${TARGET_NAME} matrix GTest::gtest)


##
# Test run
#
add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
