add_subdirectory(object)
add_subdirectory(code)
add_subdirectory(runtime)
add_subdirectory(utils)
add_subdirectory(lib)
add_subdirectory(inc)
add_subdirectory(extlib)

add_executable(cppython main.cpp)
target_link_libraries(cppython PRIVATE code runtime)

add_dependencies(cppython lib)
