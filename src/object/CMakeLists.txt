file(GLOB OBJECT_SRC *.cpp *.hpp)
add_library(object ${OBJECT_SRC})
target_include_directories(object PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
target_link_libraries(object PRIVATE runtime utils)