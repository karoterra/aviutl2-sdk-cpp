add_library(ScriptModule SHARED ScriptModule.cpp)

target_link_libraries(ScriptModule PRIVATE aviutl2::sdk_cpp)

set_target_properties(ScriptModule PROPERTIES
    SUFFIX ".mod2"
)
