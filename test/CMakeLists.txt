file(GLOB TEST_SRC *.py)

set(pyc_file_extension "cpython-39.pyc")

foreach(v ${TEST_SRC})
    get_filename_component(test_file ${v} NAME)
    string(REGEX REPLACE ".py" "" test_file_name ${test_file})
  
    set(pyc_file ${CMAKE_CURRENT_SOURCE_DIR}/__pycache__/${test_file_name}.${pyc_file_extension})

    add_test(
        NAME gen_${test_file_name}_pyc
        COMMAND python -m compileall ${CMAKE_CURRENT_SOURCE_DIR}/${test_file}
    )

    set_tests_properties(
        gen_${test_file_name}_pyc
        PROPERTIES FIXTURES_SETUP ${test_file_name}_pyc
    )

   add_test(
        NAME test_${test_file_name}
        COMMAND cppython ${pyc_file}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
    set_tests_properties(
        test_${test_file_name}
        PROPERTIES FIXTURES_REQUIRED ${test_file_name}_pyc
    )
endforeach()
