add_executable(valid_input_plugin_tests valid_input_plugin_tests.cpp)

target_link_libraries(valid_input_plugin_tests PRIVATE aviutl2::sdk_cpp)

add_test(NAME valid_input_plugin_tests COMMAND $<TARGET_FILE:valid_input_plugin_tests>)
