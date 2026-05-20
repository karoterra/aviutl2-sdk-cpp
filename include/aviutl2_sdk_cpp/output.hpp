#pragma once

#include <concepts>
#include <string>
#include <type_traits>

#include <aviutl2_sdk_cpp/common.hpp>
#include <aviutl2_sdk_cpp/config.hpp>
#include <aviutl2_sdk_cpp/edit.hpp>
#include <aviutl2_sdk_cpp/logger.hpp>
#include <aviutl2_sdk_cpp/raw/output.hpp>
#include <aviutl2_sdk_cpp/utils.hpp>

namespace aviutl2::output::detail {

inline constexpr DWORD make_fourcc(char c0, char c1, char c2, char c3) {
    return static_cast<DWORD>(static_cast<unsigned char>(c0)) |
           (static_cast<DWORD>(static_cast<unsigned char>(c1)) << 8) |
           (static_cast<DWORD>(static_cast<unsigned char>(c2)) << 16) |
           (static_cast<DWORD>(static_cast<unsigned char>(c3)) << 24);
}

}; // namespace aviutl2::output::detail

namespace aviutl2::output {

enum class OutputInfoFlag : int {
    VIDEO = raw::OUTPUT_INFO::FLAG_VIDEO,
    AUDIO = raw::OUTPUT_INFO::FLAG_AUDIO,
};

enum class OutputVideoFormat : DWORD {
    RGB24 = 0, // BI_RGB
    PA64 = detail::make_fourcc('P', 'A', '6', '4'),
    HF64 = detail::make_fourcc('H', 'F', '6', '4'),
    YUY2 = detail::make_fourcc('Y', 'U', 'Y', '2'),
    YC48 = detail::make_fourcc('Y', 'C', '4', '8'),
};

enum class OutputAudioFormat : DWORD {
    PCM16 = 1,
    Float32 = 3,
};

class OutputInfo {
  public:
    explicit OutputInfo(raw::OUTPUT_INFO* raw) : raw_(raw) {}

    OutputInfoFlag flag() const { return static_cast<OutputInfoFlag>(raw_->flag); }

    int width() const { return raw_->w; }
    int height() const { return raw_->h; }
    int rate() const { return raw_->rate; }
    int scale() const { return raw_->scale; }
    int n() const { return raw_->n; }
    int audio_rate() const { return raw_->audio_rate; }
    int audio_ch() const { return raw_->audio_ch; }
    int audio_n() const { return raw_->audio_n; }
    LPCWSTR savefile() const { return raw_->savefile; }

    void* get_video(int frame, OutputVideoFormat format) const {
        if (!raw_ || !raw_->func_get_video) {
            return nullptr;
        }
        return raw_->func_get_video(frame, static_cast<DWORD>(format));
    }

    void* get_audio(int start, int length, int* readed, OutputAudioFormat format) const {
        if (!raw_ || !raw_->func_get_audio) {
            return nullptr;
        }
        return raw_->func_get_audio(start, length, readed, static_cast<DWORD>(format));
    }

    bool is_abort() const { return raw_->func_is_abort && raw_->func_is_abort(); }

    void display_rest_time(int now, int total) const {
        if (raw_->func_rest_time_disp) {
            raw_->func_rest_time_disp(now, total);
        }
    }

    void set_buffer_size(int video_size, int audio_size) const {
        if (raw_->func_set_buffer_size) {
            raw_->func_set_buffer_size(video_size, audio_size);
        }
    }

    raw::OUTPUT_INFO* raw() const { return raw_; }

  private:
    raw::OUTPUT_INFO* raw_;
};

template <typename T>
concept OutputablePlugin = requires(T& plugin, OutputInfo& info) {
    { plugin.output_impl(info) } -> std::convertible_to<bool>;
};

template <typename T>
concept ConfigurablePlugin = requires(T& plugin, HWND hwnd, HINSTANCE dll_hinst) {
    { plugin.config_impl(hwnd, dll_hinst) } -> std::convertible_to<bool>;
};

template <typename T>
concept ConfigTextGettable = requires(T& plugin) {
    { plugin.get_config_text_impl() } -> std::same_as<LPCWSTR>;
};

template <typename T>
concept ProjectConfigLoadable = requires(T& plugin, edit::ProjectFile& project) {
    { plugin.load_project_config_impl(project) } -> std::convertible_to<bool>;
};

template <typename T>
concept ProjectConfigSavable = requires(T& plugin, edit::ProjectFile& project) {
    { plugin.save_project_config_impl(project) } -> std::convertible_to<bool>;
};

enum class OutputFlag : int {
    VIDEO = raw::OUTPUT_PLUGIN_TABLE::FLAG_VIDEO,
    AUDIO = raw::OUTPUT_PLUGIN_TABLE::FLAG_AUDIO,
    IMAGE = raw::OUTPUT_PLUGIN_TABLE::FLAG_IMAGE,
    PROJECT_CONFIG = raw::OUTPUT_PLUGIN_TABLE::FLAG_PROJECT_CONFIG,
};

class OutputPluginBase {};

template <typename Derived> class OutputPlugin : public aviutl2::Singleton<Derived>, public OutputPluginBase {
  protected:
    using token = typename aviutl2::Singleton<Derived>::token;

  public:
    OutputFlag flag_;
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

    const raw::OUTPUT_PLUGIN_TABLE* plugin_table() const {
        static raw::OUTPUT_PLUGIN_TABLE table = {
            .flag = static_cast<int>(derived()->flag_),
            .name = derived()->name_,
            .filefilter = derived()->filefilter_.c_str(),
            .information = derived()->information_,
            .func_output = &Derived::output,
            .func_config = ConfigurablePlugin<Derived> ? &Derived::config : nullptr,
            .func_get_config_text = ConfigTextGettable<Derived> ? &Derived::get_config_text : nullptr,
            .func_load_project_config = ProjectConfigLoadable<Derived> ? &Derived::load_project_config : nullptr,
            .func_save_project_config = ProjectConfigSavable<Derived> ? &Derived::save_project_config : nullptr,
        };
        return &table;
    }

    static bool output(raw::OUTPUT_INFO* oip) {
        static_assert(OutputablePlugin<Derived>, "OutputPlugin requires: bool output_impl(OutputInfo& info)");

        try {
            OutputInfo info{oip};
            return Derived::instance().output_impl(info);
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to output: {}", utils::to_wstring(e.what()));
            return false;
        } catch (...) {
            aviutl2::logger::error(L"Failed to output: unknown error");
            return false;
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

    static LPCWSTR get_config_text() {
        try {
            if constexpr (ConfigTextGettable<Derived>) {
                return Derived::instance().get_config_text_impl();
            } else {
                return nullptr;
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to get config text: {}", utils::to_wstring(e.what()));
            return nullptr;
        } catch (...) {
            aviutl2::logger::error(L"Failed to get config text: unknown error");
            return nullptr;
        }
    }

    static bool load_project_config(raw::PROJECT_FILE* project) {
        try {
            if constexpr (ProjectConfigLoadable<Derived>) {
                edit::ProjectFile wrapped{project};
                return Derived::instance().load_project_config_impl(wrapped);
            } else {
                return false;
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to load project config: {}", utils::to_wstring(e.what()));
            return false;
        } catch (...) {
            aviutl2::logger::error(L"Failed to load project config: unknown error");
            return false;
        }
    }

    static bool save_project_config(raw::PROJECT_FILE* project) {
        try {
            if constexpr (ProjectConfigSavable<Derived>) {
                edit::ProjectFile wrapped{project};
                return Derived::instance().save_project_config_impl(wrapped);
            } else {
                return false;
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to save project config: {}", utils::to_wstring(e.what()));
            return false;
        } catch (...) {
            aviutl2::logger::error(L"Failed to save project config: unknown error");
            return false;
        }
    }

  private:
    Derived* derived() { return static_cast<Derived*>(this); }
    const Derived* derived() const { return static_cast<const Derived*>(this); }
};

template <typename T>
concept OutputPluginType = std::derived_from<std::remove_cvref_t<T>, OutputPluginBase> && requires(T& plugin) {
    { plugin.plugin_table() } -> std::same_as<const raw::OUTPUT_PLUGIN_TABLE*>;
};

}; // namespace aviutl2::output

namespace aviutl2::utils::enum_utils {

template <> struct enable_bitmask<output::OutputInfoFlag> : std::true_type {};
template <> struct enable_bitmask<output::OutputFlag> : std::true_type {};

}; // namespace aviutl2::utils::enum_utils

#define AVIUTL2_REGISTER_OUTPUT_PLUGIN(PluginType)                                                                     \
    AVIUTL2_API void InitializeLogger(LOG_HANDLE* logger) { aviutl2::logger::initialize(logger); }                     \
    AVIUTL2_API void InitializeConfig(CONFIG_HANDLE* config) { aviutl2::config::initialize(config); }                  \
    AVIUTL2_API OUTPUT_PLUGIN_TABLE* GetOutputPluginTable() {                                                          \
        return const_cast<OUTPUT_PLUGIN_TABLE*>(PluginType::instance().plugin_table());                                \
    }                                                                                                                  \
    AVIUTL2_API bool InitializePlugin(DWORD version) { return PluginType::instance().initialize(version); }            \
    AVIUTL2_API void UninitializePlugin() { PluginType::instance().uninitialize(); }
