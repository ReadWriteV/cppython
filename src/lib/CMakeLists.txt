set(pyc_file_extension "cpython-39.pyc")

add_custom_target(
    lib
    COMMAND python -m compileall ${CMAKE_CURRENT_SOURCE_DIR}/builtin.py
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/__pycache__/builtin.${pyc_file_extension} ${CMAKE_SOURCE_DIR}/lib/builtin.pyc
    COMMENT "Generating builtin.pyc"
)