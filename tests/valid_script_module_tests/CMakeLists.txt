add_executable(valid_script_module_tests valid_script_module_tests.cpp)

target_link_libraries(valid_script_module_tests PRIVATE aviutl2::sdk_cpp)

add_test(NAME valid_script_module_tests COMMAND $<TARGET_FILE:valid_script_module_tests>)
