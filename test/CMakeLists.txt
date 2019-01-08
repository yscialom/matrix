set(TARGET_NAME matrix-test)

add_executable(${TARGET_NAME}
    src/dummy.cpp
)
target_link_libraries(${TARGET_NAME} matrix)
