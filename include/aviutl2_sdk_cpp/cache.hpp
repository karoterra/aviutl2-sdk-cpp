#pragma once

#include <memory>
#include <span>

#include <aviutl2_sdk_cpp/common.hpp>
#include <aviutl2_sdk_cpp/raw/cache.hpp>

namespace aviutl2::cache::detail {

inline raw::CACHE_HANDLE* g_handle = nullptr;

}; // namespace aviutl2::cache::detail

namespace aviutl2::cache {

inline void initialize(raw::CACHE_HANDLE* handle) { detail::g_handle = handle; }
inline bool is_available() { return detail::g_handle != nullptr; }

class CacheImage {
  public:
    static CacheImage get(void* identifier, LPCWSTR name) {
        if (!is_available() || !detail::g_handle->get_image_cache) {
            return {};
        }
        auto p = std::unique_ptr<raw::CACHE_IMAGE>(
            new raw::CACHE_IMAGE{detail::g_handle->get_image_cache(identifier, name)});
        return CacheImage{std::move(p)};
    }

    static CacheImage create(void* identifier, LPCWSTR name, int width, int height) {
        if (!is_available() || !detail::g_handle->create_image_cache) {
            return {};
        }
        auto p = std::unique_ptr<raw::CACHE_IMAGE>(
            new raw::CACHE_IMAGE{detail::g_handle->create_image_cache(identifier, name, width, height)});
        return CacheImage{std::move(p)};
    }

    static CacheImage get_file(LPCWSTR file) {
        if (!is_available() || !detail::g_handle->get_image_file_cache) {
            return {};
        }
        auto p = std::unique_ptr<raw::CACHE_IMAGE>(new raw::CACHE_IMAGE{detail::g_handle->get_image_file_cache(file)});
        return CacheImage{std::move(p)};
    }

    CacheImage() = default;

    CacheImage(const CacheImage&) = delete;
    CacheImage& operator=(const CacheImage&) = delete;

    CacheImage(CacheImage&&) = default;
    CacheImage& operator=(CacheImage&&) = default;

    std::span<aviutl2::PixelRgba> data() {
        if (!raw_ || !*raw_ || !raw_->buffer) {
            return {};
        }
        return {raw_->buffer, static_cast<std::size_t>(width() * height())};
    }

    int width() const { return raw_ ? raw_->width : 0; }
    int height() const { return raw_ ? raw_->height : 0; }

    explicit operator bool() const { return raw_ && static_cast<bool>(*raw_); }

  private:
    explicit CacheImage(std::unique_ptr<raw::CACHE_IMAGE> p) : raw_{std::move(p)} {}

    std::unique_ptr<raw::CACHE_IMAGE> raw_;
};

class CacheAudio {
  public:
    static CacheAudio get(void* identifier, LPCWSTR name) {
        if (!is_available() || !detail::g_handle->get_audio_cache) {
            return {};
        }
        auto p = std::unique_ptr<raw::CACHE_AUDIO>(
            new raw::CACHE_AUDIO{detail::g_handle->get_audio_cache(identifier, name)});
        return CacheAudio{std::move(p)};
    }

    static CacheAudio create(void* identifier, LPCWSTR name, int sample_num, int channel_num) {
        if (!is_available() || !detail::g_handle->create_audio_cache) {
            return {};
        }
        auto p = std::unique_ptr<raw::CACHE_AUDIO>(
            new raw::CACHE_AUDIO{detail::g_handle->create_audio_cache(identifier, name, sample_num, channel_num)});
        return CacheAudio{std::move(p)};
    }

    CacheAudio() = default;

    CacheAudio(const CacheAudio&) = delete;
    CacheAudio& operator=(const CacheAudio&) = delete;

    CacheAudio(CacheAudio&&) = default;
    CacheAudio& operator=(CacheAudio&&) = default;

    std::span<float> left() {
        if (!raw_ || !raw_->buffer0) {
            return {};
        }
        return {raw_->buffer0, static_cast<std::size_t>(sample_num())};
    }

    std::span<float> right() {
        if (!raw_ || !raw_->buffer1) {
            return {};
        }
        return {raw_->buffer1, static_cast<std::size_t>(sample_num())};
    }

    int sample_num() const { return raw_ ? raw_->sample_num : 0; }
    int channel_num() const { return raw_ ? raw_->channel_num : 0; }

    std::span<float> operator[](size_t index) {
        if (index == 0)
            return left();
        if (index == 1)
            return right();
        return {};
    }

    explicit operator bool() const { return raw_ && static_cast<bool>(*raw_); }

  private:
    explicit CacheAudio(std::unique_ptr<raw::CACHE_AUDIO> p) : raw_{std::move(p)} {}

    std::unique_ptr<raw::CACHE_AUDIO> raw_;
};

}; // namespace aviutl2::cache
