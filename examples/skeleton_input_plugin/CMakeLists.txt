add_library(skeleton_input_plugin SHARED skeleton_input_plugin.cpp)

target_link_libraries(skeleton_input_plugin PRIVATE aviutl2::sdk_cpp)

set_target_properties(skeleton_input_plugin PROPERTIES
    SUFFIX ".aui2"
)
