#pragma once

#include <concepts>
#include <optional>
#include <string>
#include <type_traits>

#include <aviutl2_sdk_cpp/common.hpp>
#include <aviutl2_sdk_cpp/config.hpp>
#include <aviutl2_sdk_cpp/logger.hpp>
#include <aviutl2_sdk_cpp/raw/input.hpp>
#include <aviutl2_sdk_cpp/utils.hpp>

namespace aviutl2::input {

enum class InputInfoFlag : int {
    VIDEO = 1,          // 画像データあり
    AUDIO = 2,          // 音声データあり
    TIME_TO_FRAME = 16, // フレーム番号を時間から算出する ※func_time_to_frame()が呼ばれるようになる
};

struct InputInfo {
    InputInfoFlag flag{};
    int rate{};
    int scale{};
    int n{};
    BITMAPINFOHEADER* format{};
    int format_size{};
    int audio_n{};
    WAVEFORMATEX* audio_format{};
    int audio_format_size{};

    static InputInfo from_raw(const raw::INPUT_INFO& raw) {
        return {
            .flag = static_cast<InputInfoFlag>(raw.flag),
            .rate = raw.rate,
            .scale = raw.scale,
            .n = raw.n,
            .format = raw.format,
            .format_size = raw.format_size,
            .audio_n = raw.audio_n,
            .audio_format = raw.audio_format,
            .audio_format_size = raw.audio_format_size,
        };
    }

    raw::INPUT_INFO to_raw() const {
        return {
            .flag = static_cast<int>(flag),
            .rate = rate,
            .scale = scale,
            .n = n,
            .format = format,
            .format_size = format_size,
            .audio_n = audio_n,
            .audio_format = audio_format,
            .audio_format_size = audio_format_size,
        };
    }
};

using InputHandle = raw::INPUT_HANDLE;

enum class TrackType : int {
    VIDEO = raw::INPUT_PLUGIN_TABLE::TRACK_TYPE_VIDEO,
    AUDIO = raw::INPUT_PLUGIN_TABLE::TRACK_TYPE_AUDIO,
};

template <typename T>
concept OpenablePlugin = requires(T& plugin, LPCWSTR file) {
    { plugin.open_impl(file) } -> std::same_as<InputHandle>;
};

template <typename T>
concept CloseablePlugin = requires(T& plugin, InputHandle handle) {
    { plugin.close_impl(handle) } -> std::convertible_to<bool>;
};

template <typename T>
concept InputInfoGettablePlugin = requires(T& plugin, InputHandle handle) {
    { plugin.get_input_info_impl(handle) } -> std::same_as<std::optional<InputInfo>>;
};

template <typename T>
concept VideoReadablePlugin = requires(T& plugin, InputHandle handle, int frame, void* buf) {
    { plugin.read_video_impl(handle, frame, buf) } -> std::same_as<int>;
};

template <typename T>
concept AudioReadablePlugin = requires(T& plugin, InputHandle handle, int start, int length, void* buf) {
    { plugin.read_audio_impl(handle, start, length, buf) } -> std::convertible_to<int>;
};

template <typename T>
concept ConfigurablePlugin = requires(T& plugin, HWND hwnd, HINSTANCE dll_hinst) {
    { plugin.config_impl(hwnd, dll_hinst) } -> std::convertible_to<bool>;
};

template <typename T>
concept TrackSelectablePlugin = requires(T& plugin, InputHandle handle, TrackType type, int index) {
    { plugin.set_track_impl(handle, type, index) } -> std::convertible_to<int>;
};

template <typename T>
concept TimeToFrameInputPlugin = requires(T& plugin, InputHandle handle, double time) {
    { plugin.time_to_frame_impl(handle, time) } -> std::convertible_to<int>;
};

enum class InputFlag : int {
    VIDEO = 1,        // 画像をサポートする
    AUDIO = 2,        // 音声をサポートする
    CONCURRENT = 16,  // データの同時取得をサポートする
    MULTI_TRACK = 32, // マルチトラックをサポートする ※func_set_track()が呼ばれるようになる
};

class InputPluginBase {};

template <typename Derived> class InputPlugin : public aviutl2::Singleton<Derived>, public InputPluginBase {
  protected:
    using token = typename aviutl2::Singleton<Derived>::token;

  public:
    InputFlag flag_;
    LPCWSTR name_;
    std::wstring filefilter_;
    LPCWSTR information_;

    bool initialize(DWORD version) {
        try {
            if constexpr (InitializablePlugin<Derived>) {
                return derived()->initialize_impl(version);
            } else {
                return true;
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to initialize plugin: {}", utils::to_wstring(e.what()));
            return false;
        } catch (...) {
            aviutl2::logger::error(L"Failed to initialize plugin: unknown error");
            return false;
        }
    }

    void uninitialize() {
        try {
            if constexpr (UninitializablePlugin<Derived>) {
                derived()->uninitialize_impl();
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to uninitialize plugin: {}", utils::to_wstring(e.what()));
        } catch (...) {
            aviutl2::logger::error(L"Failed to uninitialize plugin: unknown error");
        }
    }

    const raw::INPUT_PLUGIN_TABLE* plugin_table() const {
        static raw::INPUT_PLUGIN_TABLE table = {
            .flag = static_cast<int>(derived()->flag_),
            .name = derived()->name_,
            .filefilter = derived()->filefilter_.c_str(),
            .func_open = &Derived::open,
            .func_close = &Derived::close,
            .func_info_get = &Derived::get_input_info,
            .func_read_video = VideoReadablePlugin<Derived> ? &Derived::read_video : nullptr,
            .func_read_audio = AudioReadablePlugin<Derived> ? &Derived::read_audio : nullptr,
            .func_config = ConfigurablePlugin<Derived> ? &Derived::config : nullptr,
            .func_set_track = TrackSelectablePlugin<Derived> ? &Derived::set_track : nullptr,
            .func_time_to_frame = TimeToFrameInputPlugin<Derived> ? &Derived::time_to_frame : nullptr,
        };
        return &table;
    }

    static InputHandle open(LPCWSTR file) {
        static_assert(OpenablePlugin<Derived>, "InputPlugin requires: InputHandle open_impl(LPCWSTR file)");

        try {
            return Derived::instance().open_impl(file);
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to open: {}: {}", file, utils::to_wstring(e.what()));
            return nullptr;
        } catch (...) {
            aviutl2::logger::error(L"Failed to open: {}: unknown error", file);
            return nullptr;
        }
    }

    static bool close(InputHandle ih) {
        static_assert(CloseablePlugin<Derived>, "InputPlugin requires: bool close_impl(InputHandle handle)");

        try {
            return Derived::instance().close_impl(ih);
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to close: {}", utils::to_wstring(e.what()));
            return false;
        } catch (...) {
            aviutl2::logger::error(L"Failed to close: unknown error");
            return false;
        }
    }

    static bool get_input_info(InputHandle ih, raw::INPUT_INFO* iip) {
        static_assert(InputInfoGettablePlugin<Derived>,
                      "InputPlugin requires: std::optional<InputInfo> get_input_info_impl(InputHandle handle)");

        try {
            if (iip == nullptr) {
                return false;
            }

            std::optional<InputInfo> info = Derived::instance().get_input_info_impl(ih);
            if (!info) {
                return false;
            }

            *iip = info->to_raw();
            return true;
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to get input info: {}", utils::to_wstring(e.what()));
            return false;
        } catch (...) {
            aviutl2::logger::error(L"Failed to get input info: unknown error");
            return false;
        }
    }

    static int read_video(InputHandle ih, int frame, void* buf) {
        try {
            if constexpr (VideoReadablePlugin<Derived>) {
                return Derived::instance().read_video_impl(ih, frame, buf);
            } else {
                return 0;
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to read video: {}", utils::to_wstring(e.what()));
            return 0;
        } catch (...) {
            aviutl2::logger::error(L"Failed to read video: unknown error");
            return 0;
        }
    }

    static int read_audio(InputHandle ih, int start, int length, void* buf) {
        try {
            if constexpr (AudioReadablePlugin<Derived>) {
                return Derived::instance().read_audio_impl(ih, start, length, buf);
            } else {
                return 0;
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to read audio: {}", utils::to_wstring(e.what()));
            return 0;
        } catch (...) {
            aviutl2::logger::error(L"Failed to read audio: unknown error");
            return 0;
        }
    }

    static bool config(HWND hwnd, HINSTANCE dll_hinst) {
        try {
            if constexpr (ConfigurablePlugin<Derived>) {
                return Derived::instance().config_impl(hwnd, dll_hinst);
            } else {
                return false;
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to config: {}", utils::to_wstring(e.what()));
            return false;
        } catch (...) {
            aviutl2::logger::error(L"Failed to config: unknown error");
            return false;
        }
    }

    static int set_track(InputHandle handle, int type, int index) {
        try {
            if constexpr (TrackSelectablePlugin<Derived>) {
                return Derived::instance().set_track_impl(handle, static_cast<TrackType>(type), index);
            } else {
                return -1;
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to set track: {}", utils::to_wstring(e.what()));
            return -1;
        } catch (...) {
            aviutl2::logger::error(L"Failed to set track: unknown error");
            return -1;
        }
    }

    static int time_to_frame(InputHandle handle, double time) {
        try {
            if constexpr (TimeToFrameInputPlugin<Derived>) {
                return Derived::instance().time_to_frame_impl(handle, time);
            } else {
                return 0;
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to time to frame: {}", utils::to_wstring(e.what()));
            return 0;
        } catch (...) {
            aviutl2::logger::error(L"Failed to time to frame: unknown error");
            return 0;
        }
    }

  private:
    Derived* derived() { return static_cast<Derived*>(this); }
    const Derived* derived() const { return static_cast<const Derived*>(this); }
};

template <typename T>
concept InputPluginType = std::derived_from<std::remove_cvref_t<T>, InputPluginBase> && requires(T& plugin) {
    { plugin.plugin_table() } -> std::same_as<const raw::INPUT_PLUGIN_TABLE*>;
};

}; // namespace aviutl2::input

namespace aviutl2::utils::enum_utils {

template <> struct enable_bitmask<input::InputInfoFlag> : std::true_type {};
template <> struct enable_bitmask<input::InputFlag> : std::true_type {};

}; // namespace aviutl2::utils::enum_utils

#define AVIUTL2_REGISTER_INPUT_PLUGIN(PluginType)                                                                      \
    AVIUTL2_API void InitializeLogger(LOG_HANDLE* logger) { aviutl2::logger::initialize(logger); }                     \
    AVIUTL2_API void InitializeConfig(CONFIG_HANDLE* config) { aviutl2::config::initialize(config); }                  \
    AVIUTL2_API INPUT_PLUGIN_TABLE* GetInputPluginTable() {                                                            \
        return const_cast<INPUT_PLUGIN_TABLE*>(PluginType::instance().plugin_table());                                 \
    }                                                                                                                  \
    AVIUTL2_API bool InitializePlugin(DWORD version) { return PluginType::instance().initialize(version); }            \
    AVIUTL2_API void UninitializePlugin() { PluginType::instance().uninitialize(); }
