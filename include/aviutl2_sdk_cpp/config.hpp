#pragma once

#include <aviutl2_sdk_cpp/raw/config.hpp>

namespace aviutl2::config::detail {

inline raw::CONFIG_HANDLE* g_handle = nullptr;

}; // namespace aviutl2::config::detail

namespace aviutl2::config {

using FontInfo = raw::FONT_INFO;

inline void initialize(raw::CONFIG_HANDLE* handle) { detail::g_handle = handle; }
inline bool is_available() { return detail::g_handle != nullptr; }

inline LPCWSTR app_data_path() {
    if (!detail::g_handle) {
        return nullptr;
    }
    return detail::g_handle->app_data_path;
}

inline LPCWSTR translate(LPCWSTR text) {
    if (!detail::g_handle || !detail::g_handle->translate) {
        return text;
    }
    return detail::g_handle->translate(detail::g_handle, text);
}

inline LPCWSTR get_language_text(LPCWSTR section, LPCWSTR text) {
    if (!detail::g_handle || !detail::g_handle->get_language_text) {
        return text;
    }
    return detail::g_handle->get_language_text(detail::g_handle, section, text);
}

inline FontInfo* get_font_info(LPCSTR key) {
    if (!detail::g_handle || !detail::g_handle->get_font_info) {
        return nullptr;
    }
    return detail::g_handle->get_font_info(detail::g_handle, key);
}

inline int get_color_code(LPCSTR key) {
    if (!detail::g_handle || !detail::g_handle->get_color_code) {
        return 0;
    }
    return detail::g_handle->get_color_code(detail::g_handle, key);
}

inline int get_layout_size(LPCSTR key) {
    if (!detail::g_handle || !detail::g_handle->get_layout_size) {
        return 0;
    }
    return detail::g_handle->get_layout_size(detail::g_handle, key);
}

inline int get_color_code_index(LPCSTR key, int index) {
    if (!detail::g_handle || !detail::g_handle->get_color_code_index) {
        return 0;
    }
    return detail::g_handle->get_color_code_index(detail::g_handle, key, index);
}

}; // namespace aviutl2::config
