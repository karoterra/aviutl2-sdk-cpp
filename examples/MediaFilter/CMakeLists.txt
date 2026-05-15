add_library(MediaFilter SHARED MediaFilter.cpp)

target_link_libraries(MediaFilter PRIVATE aviutl2::sdk_cpp)

set_target_properties(MediaFilter PROPERTIES
    SUFFIX ".auf2"
)
