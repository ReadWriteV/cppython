add_subdirectory(object)
add_subdirectory(code)
add_subdirectory(runtime)
add_subdirectory(utils)

add_executable(cppython main.cpp)
target_link_libraries(cppython PRIVATE code runtime)

add_test(
    NAME read_pyc
    COMMAND cppython "./test/__pycache__/hello.cpython-39.pyc"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)