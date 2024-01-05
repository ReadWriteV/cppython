file(GLOB MEMORY_SRC *.cpp *.hpp)
add_library(memory ${MEMORY_SRC})
target_include_directories(memory PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
target_link_libraries(memory PRIVATE runtime utils)