add_library(AviReader SHARED AviReader.cpp)

target_link_libraries(AviReader PRIVATE aviutl2::sdk_cpp)

set_target_properties(AviReader PROPERTIES
    SUFFIX ".aui2"
)
