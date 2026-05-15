add_library(header_check OBJECT header_check.cpp)

target_link_libraries(header_check PRIVATE aviutl2::sdk_cpp)
