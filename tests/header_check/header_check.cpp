#include <Windows.h>
#include <cstdint>

#include <aviutl2_sdk_cpp/raw.hpp>

#include <aviutl2_sdk_cpp/raw/config.hpp>
#include <aviutl2_sdk_cpp/raw/filter.hpp>
#include <aviutl2_sdk_cpp/raw/input.hpp>
#include <aviutl2_sdk_cpp/raw/logger.hpp>
#include <aviutl2_sdk_cpp/raw/module.hpp>
#include <aviutl2_sdk_cpp/raw/output.hpp>
#include <aviutl2_sdk_cpp/raw/plugin.hpp>

#include <aviutl2_sdk_cpp/common.hpp>
#include <aviutl2_sdk_cpp/config.hpp>
#include <aviutl2_sdk_cpp/filter.hpp>
#include <aviutl2_sdk_cpp/logger.hpp>
#include <aviutl2_sdk_cpp/utils.hpp>

enum class Color : int {
    Red = 0xff0000,
    Green = 0x00ff00,
    Blue = 0x0000ff,
};
template <> struct aviutl2::utils::enum_utils::enable_bitmask<Color> : std::true_type {};

void f() {
    Color c = Color::Red | Color::Green;
    aviutl2::logger::log(L"test");
}
