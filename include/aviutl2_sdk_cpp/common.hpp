#pragma once

#include <concepts>

#include <aviutl2_sdk_cpp/raw/filter.hpp>

#define AVIUTL2_API extern "C" __declspec(dllexport)

namespace aviutl2 {

template <typename T>
concept InitializablePlugin = requires(T& a, DWORD version) {
    { a.initialize_impl(version) } -> std::convertible_to<bool>;
};

template <typename T>
concept UninitializablePlugin = requires(T& a) {
    { a.uninitialize_impl() } -> std::same_as<void>;
};

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

/// @brief RGBA32bit構造体
using PixelRgba = raw::PIXEL_RGBA;

}; // namespace aviutl2
