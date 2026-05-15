add_library(AviSaver SHARED AviSaver.cpp)

target_link_libraries(AviSaver PRIVATE aviutl2::sdk_cpp)

set_target_properties(AviSaver PROPERTIES
    SUFFIX ".auo2"
)
