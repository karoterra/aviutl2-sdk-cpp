add_library(MediaObject SHARED MediaObject.cpp)

target_link_libraries(MediaObject PRIVATE aviutl2::sdk_cpp)

set_target_properties(MediaObject PROPERTIES
    SUFFIX ".auf2"
)
