file(GLOB RUNTIME_SRC *.cpp *.hpp)
add_library(runtime ${RUNTIME_SRC})
target_include_directories(runtime PUBLIC ${CMAKE_CURRENT_LIST_DIR}/..)
target_link_libraries(runtime PUBLIC object utils memory inc)
