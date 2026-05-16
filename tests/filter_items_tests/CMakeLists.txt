add_executable(filter_items_tests filter_items_tests.cpp)

target_link_libraries(filter_items_tests PRIVATE aviutl2::sdk_cpp)

add_test(NAME filter_items_tests COMMAND $<TARGET_FILE:filter_items_tests>)
