add_library(utils INTERFACE singleton.hpp vector.hpp)
target_include_directories(utils INTERFACE ${CMAKE_CURRENT_LIST_DIR}/..)
target_link_libraries(utils INTERFACE runtime)

