add_library(skeleton_output_plugin SHARED skeleton_output_plugin.cpp)

target_link_libraries(skeleton_output_plugin PRIVATE aviutl2::sdk_cpp)

set_target_properties(skeleton_output_plugin PROPERTIES
    SUFFIX ".auo2"
)
