file(GLOB UTILS_SRC *.cpp *.hpp)
add_library(utils ${UTILS_SRC})
target_include_directories(utils PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
target_link_libraries(utils PRIVATE runtime)
