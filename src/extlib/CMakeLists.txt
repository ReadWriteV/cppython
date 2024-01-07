add_library(math SHARED math.cpp)
target_link_libraries(math PRIVATE inc object code)
target_include_directories(math PRIVATE ${CMAKE_CURRENT_LIST_DIR}/..)

add_custom_command(
    TARGET math
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:math> ${CMAKE_SOURCE_DIR}/lib
    COMMENT "Copy math dll to lib dir"
)
