#pragma once

#include <aviutl2_sdk_cpp/windows.hpp>

#include <concepts>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <type_traits>

// -----------------------------------------------------------------------------
// enum_utils
// -----------------------------------------------------------------------------

namespace aviutl2::utils::enum_utils {

template <typename T> struct enable_bitmask : std::false_type {};

template <typename T>
concept ScopedEnum = std::is_enum_v<T> && !std::is_convertible_v<T, std::underlying_type_t<T>>;

template <typename T>
concept BitmaskEnum = ScopedEnum<T> && enable_bitmask<T>::value;

template <BitmaskEnum E> constexpr bool has_flag(E value, E flag) { return (value & flag) == flag; }

}; // namespace aviutl2::utils::enum_utils

namespace aviutl2::utils::enum_utils::detail {

template <BitmaskEnum E> constexpr std::underlying_type_t<E> to_underlying(E e) {
    return static_cast<std::underlying_type_t<E>>(e);
}

template <BitmaskEnum E> constexpr E operator|(E lhs, E rhs) {
    return static_cast<E>(to_underlying(lhs) | to_underlying(rhs));
}

template <BitmaskEnum E> constexpr E operator&(E lhs, E rhs) {
    return static_cast<E>(to_underlying(lhs) & to_underlying(rhs));
}

template <BitmaskEnum E> constexpr E operator^(E lhs, E rhs) {
    return static_cast<E>(to_underlying(lhs) ^ to_underlying(rhs));
}

template <BitmaskEnum E> constexpr E operator~(E e) { return static_cast<E>(~to_underlying(e)); }

template <BitmaskEnum E> constexpr E& operator|=(E& lhs, E rhs) {
    lhs = lhs | rhs;
    return lhs;
}

template <BitmaskEnum E> constexpr E& operator&=(E& lhs, E rhs) {
    lhs = lhs & rhs;
    return lhs;
}

template <BitmaskEnum E> constexpr E& operator^=(E& lhs, E rhs) {
    lhs = lhs ^ rhs;
    return lhs;
}

}; // namespace aviutl2::utils::enum_utils::detail

using aviutl2::utils::enum_utils::detail::operator|;
using aviutl2::utils::enum_utils::detail::operator&;
using aviutl2::utils::enum_utils::detail::operator^;
using aviutl2::utils::enum_utils::detail::operator~;
using aviutl2::utils::enum_utils::detail::operator|=;
using aviutl2::utils::enum_utils::detail::operator&=;
using aviutl2::utils::enum_utils::detail::operator^=;

// -----------------------------------------------------------------------------
// Miscs
// -----------------------------------------------------------------------------

namespace aviutl2::utils {

inline std::wstring to_wstring(const char* str) {
    UINT code_page = CP_UTF8;
    int len = MultiByteToWideChar(code_page, 0, str, -1, nullptr, 0);
    if (len <= 0) {
        return L"";
    }
    std::wstring wstr(len, L'\0');
    MultiByteToWideChar(code_page, 0, str, -1, wstr.data(), len);
    return wstr;
}

struct FileFilterItem {
    std::wstring name;    // フィルタの名前 (例: "AviFile (*.avi)")
    std::wstring pattern; // ファイルパターン (例: "*.avi")
};

inline std::wstring make_file_filter(std::initializer_list<FileFilterItem> items) {
    std::wstring filefilter;
    for (const auto& i : items) {
        filefilter += i.name + L'\0' + i.pattern + L'\0';
    }
    filefilter += L'\0'; // 終端
    return filefilter;
}

} // namespace aviutl2::utils
