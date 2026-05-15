cmake_minimum_required(VERSION 3.31)

project(aviutl2-sdk-cpp
    VERSION 0.1.0
    DESCRIPTION "C++ wrapper for AviUtl2 SDK"
    LANGUAGES CXX
)

option(AVIUTL2_SDK_CPP_BUILD_EXAMPLES "Build examples" ON)
option(AVIUTL2_SDK_CPP_BUILD_TESTS "Build tests" OFF)

add_library(aviutl2_sdk_cpp INTERFACE)
add_library(aviutl2::sdk_cpp ALIAS aviutl2_sdk_cpp)

target_compile_features(aviutl2_sdk_cpp INTERFACE cxx_std_20)

target_compile_options(aviutl2_sdk_cpp
    INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

target_include_directories(aviutl2_sdk_cpp
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set(AVIUTL2_SDK_DIR "" CACHE PATH "Path to aviutl2_sdk_mirror")

if(AVIUTL2_SDK_DIR)
    target_include_directories(aviutl2_sdk_cpp
        INTERFACE
            ${AVIUTL2_SDK_DIR}/include
    )
endif()

if(AVIUTL2_SDK_CPP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(AVIUTL2_SDK_CPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
