cmake_minimum_required(VERSION 3.31)

project(aviutl2-sdk-cpp
    VERSION 0.1.0
    DESCRIPTION "C++ wrapper for AviUtl2 SDK"
    LANGUAGES CXX
)

option(AVIUTL2_SDK_CPP_BUILD_EXAMPLES "Build examples" ON)
option(AVIUTL2_SDK_CPP_BUILD_TESTS "Build tests" ON)

add_library(aviutl2_sdk_cpp INTERFACE)
add_library(aviutl2::sdk_cpp ALIAS aviutl2_sdk_cpp)

target_compile_features(aviutl2_sdk_cpp INTERFACE cxx_std_20)

target_compile_options(aviutl2_sdk_cpp
    INTERFACE
        $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
)

target_compile_definitions(aviutl2_sdk_cpp
    INTERFACE
        UNICODE
        _UNICODE
        NOMINMAX
)

target_include_directories(aviutl2_sdk_cpp
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

set(AVIUTL2_SDK_DIR "" CACHE PATH "Path to aviutl2_sdk_mirror")
set(AVIUTL2_SDK_GIT_TAG "a0747d4cec0700a967b9c54b18c5257993d273f4" CACHE STRING "Git tag or commit hash of aviutl2_sdk_mirror to use")

if(AVIUTL2_SDK_DIR)
    target_include_directories(aviutl2_sdk_cpp
        INTERFACE
            ${AVIUTL2_SDK_DIR}/include
    )
else()
    include(FetchContent)
    FetchContent_Declare(
        aviutl2_sdk
        GIT_REPOSITORY https://github.com/aviutl2/aviutl2_sdk_mirror.git
        GIT_TAG ${AVIUTL2_SDK_GIT_TAG}
        GIT_SHALLOW FALSE
        SOURCE_SUBDIR cmake-do-not-add
    )
    FetchContent_MakeAvailable(aviutl2_sdk)
    target_include_directories(aviutl2_sdk_cpp
        INTERFACE
            ${aviutl2_sdk_SOURCE_DIR}/include
    )
endif()

if(AVIUTL2_SDK_CPP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(AVIUTL2_SDK_CPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
