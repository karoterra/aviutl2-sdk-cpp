#pragma once

#include <concepts>
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
// Singleton
// -----------------------------------------------------------------------------

namespace aviutl2::utils {

template <typename Derived> class Singleton {
  public:
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    static Derived& instance() {
        static Derived inst{token{}};
        return inst;
    }

  protected:
    struct token {};
    Singleton() = default;
};

}; // namespace aviutl2::utils

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

} // namespace aviutl2::utils
