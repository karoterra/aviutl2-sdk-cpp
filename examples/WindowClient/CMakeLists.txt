add_library(WindowClient SHARED WindowClient.cpp)

target_link_libraries(WindowClient PRIVATE aviutl2::sdk_cpp)

set_target_properties(WindowClient PROPERTIES
    SUFFIX ".aux2"
)
