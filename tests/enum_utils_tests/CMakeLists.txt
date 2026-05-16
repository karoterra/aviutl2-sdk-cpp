add_executable(enum_utils_tests enum_utils_tests.cpp)

target_link_libraries(enum_utils_tests PRIVATE aviutl2::sdk_cpp)

add_test(NAME enum_utils_tests COMMAND $<TARGET_FILE:enum_utils_tests>)
