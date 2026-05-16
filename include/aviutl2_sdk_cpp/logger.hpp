#pragma once

#include <format>
#include <string>
#include <string_view>

#include <aviutl2_sdk_cpp/raw/logger.hpp>

namespace aviutl2::logger::detail {

using LogFn = void (*)(raw::LOG_HANDLE*, LPCWSTR);

inline raw::LOG_HANDLE* g_handle = nullptr;

static void dispatch(LogFn raw::LOG_HANDLE::* fn, std::wstring_view message) {
    if (!g_handle || !fn) {
        return;
    }

    auto fp = g_handle->*fn;

    constexpr size_t max_length = 1024;
    if (message.length() <= max_length) {
        fp(g_handle, message.data());
    } else {
        // メッセージが長すぎる場合は切り捨ててログ出力する
        std::wstring truncated_message(message.substr(0, max_length - 1));
        fp(g_handle, truncated_message.c_str());
    }
}
}; // namespace aviutl2::logger::detail

namespace aviutl2::logger {

inline void initialize(raw::LOG_HANDLE* handle) { detail::g_handle = handle; }
inline bool is_available() { return detail::g_handle != nullptr; }

inline void log(std::wstring_view message) { detail::dispatch(&raw::LOG_HANDLE::log, message); }
inline void info(std::wstring_view message) { detail::dispatch(&raw::LOG_HANDLE::info, message); }
inline void warn(std::wstring_view message) { detail::dispatch(&raw::LOG_HANDLE::warn, message); }
inline void error(std::wstring_view message) { detail::dispatch(&raw::LOG_HANDLE::error, message); }
inline void verbose(std::wstring_view message) { detail::dispatch(&raw::LOG_HANDLE::verbose, message); }

// std::format
template <typename... Args> inline void log(std::wformat_string<Args...> fmt, Args&&... args) {
    log(std::format(fmt, std::forward<Args>(args)...));
}
template <typename... Args> inline void info(std::wformat_string<Args...> fmt, Args&&... args) {
    info(std::format(fmt, std::forward<Args>(args)...));
}
template <typename... Args> inline void warn(std::wformat_string<Args...> fmt, Args&&... args) {
    warn(std::format(fmt, std::forward<Args>(args)...));
}
template <typename... Args> inline void error(std::wformat_string<Args...> fmt, Args&&... args) {
    error(std::format(fmt, std::forward<Args>(args)...));
}
template <typename... Args> inline void verbose(std::wformat_string<Args...> fmt, Args&&... args) {
    verbose(std::format(fmt, std::forward<Args>(args)...));
}

}; // namespace aviutl2::logger
