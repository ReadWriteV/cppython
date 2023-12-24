file(GLOB CODE_SRC *.cpp *.hpp)
add_library(code ${CODE_SRC})
target_include_directories(code PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
target_link_libraries(code PUBLIC object utils)
