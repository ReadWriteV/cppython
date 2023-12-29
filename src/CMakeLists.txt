add_subdirectory(object)
add_subdirectory(code)
add_subdirectory(runtime)
add_subdirectory(utils)
add_subdirectory(memory)

add_executable(cppython main.cpp)
target_link_libraries(cppython PRIVATE code runtime)
