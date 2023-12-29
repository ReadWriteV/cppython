file(GLOB OBJECT_SRC *.cpp *.hpp)
add_library(memory ${OBJECT_SRC})
target_include_directories(memory PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
target_link_libraries(memory PRIVATE runtime utils)