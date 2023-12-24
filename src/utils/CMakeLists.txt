add_library(utils INTERFACE singleton.hpp)
target_include_directories(utils INTERFACE ${CMAKE_CURRENT_LIST_DIR}/..)
