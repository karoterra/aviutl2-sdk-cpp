add_library(skeleton_script_module SHARED skeleton_script_module.cpp)

target_link_libraries(skeleton_script_module PRIVATE aviutl2::sdk_cpp)

set_target_properties(skeleton_script_module PROPERTIES
    SUFFIX ".mod2"
)
