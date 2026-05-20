add_library(skeleton_generic_plugin SHARED skeleton_generic_plugin.cpp)

target_link_libraries(skeleton_generic_plugin PRIVATE aviutl2::sdk_cpp)

set_target_properties(skeleton_generic_plugin PROPERTIES
    SUFFIX ".aux2"
)
