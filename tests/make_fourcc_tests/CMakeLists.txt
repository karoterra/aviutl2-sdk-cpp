add_executable(make_fourcc_tests make_fourcc_tests.cpp)

target_link_libraries(make_fourcc_tests PRIVATE aviutl2::sdk_cpp)

add_test(NAME make_fourcc_tests COMMAND $<TARGET_FILE:make_fourcc_tests>)
