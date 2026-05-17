#pragma once

#include <concepts>

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

}; // namespace aviutl2
