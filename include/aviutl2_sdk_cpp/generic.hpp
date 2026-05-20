#pragma once

#include <concepts>
#include <type_traits>

#include <aviutl2_sdk_cpp/cache.hpp>
#include <aviutl2_sdk_cpp/common.hpp>
#include <aviutl2_sdk_cpp/config.hpp>
#include <aviutl2_sdk_cpp/edit.hpp>
#include <aviutl2_sdk_cpp/filter.hpp>
#include <aviutl2_sdk_cpp/input.hpp>
#include <aviutl2_sdk_cpp/logger.hpp>
#include <aviutl2_sdk_cpp/module.hpp>
#include <aviutl2_sdk_cpp/output.hpp>
#include <aviutl2_sdk_cpp/utils.hpp>

namespace aviutl2::generic {

class HostApp {
  public:
    explicit HostApp(raw::HOST_APP_TABLE* raw) : raw_(raw) {}

    void register_window_client(LPCWSTR name, HWND hwnd) const {
        if (!raw_ || !raw_->register_window_client) {
            return;
        }
        raw_->register_window_client(name, hwnd);
    }

    edit::EditHandle create_edit_handle() const {
        if (!raw_ || !raw_->create_edit_handle) {
            return {};
        }
        return edit::EditHandle(raw_->create_edit_handle());
    }

    template <input::InputPluginType Plugin> void register_input_plugin(Plugin& plugin) const {
        if (!raw_ || !raw_->register_input_plugin) {
            return;
        }
        raw_->register_input_plugin(const_cast<raw::INPUT_PLUGIN_TABLE*>(plugin.plugin_table()));
    }

    template <output::OutputPluginType Plugin> void register_output_plugin(Plugin& plugin) const {
        if (!raw_ || !raw_->register_output_plugin) {
            return;
        }
        raw_->register_output_plugin(const_cast<raw::OUTPUT_PLUGIN_TABLE*>(plugin.plugin_table()));
    }

    template <filter::FilterPluginType Plugin> void register_filter_plugin(Plugin& plugin) const {
        if (!raw_ || !raw_->register_filter_plugin) {
            return;
        }
        raw_->register_filter_plugin(const_cast<raw::FILTER_PLUGIN_TABLE*>(plugin.plugin_table()));
    }

    template <module::ScriptModuleType Plugin> void register_script_module(Plugin& plugin) const {
        if (!raw_ || !raw_->register_script_module) {
            return;
        }
        raw_->register_script_module(const_cast<raw::SCRIPT_MODULE_TABLE*>(plugin.plugin_table()));
    }

    template <module::ScriptModuleType Plugin> void register_script_module_name(Plugin& plugin, LPCWSTR name) const {
        if (!raw_ || !raw_->register_script_module_name) {
            return;
        }
        raw_->register_script_module_name(const_cast<raw::SCRIPT_MODULE_TABLE*>(plugin.plugin_table()), name);
    }

    void register_import_menu_raw(LPCWSTR name, void (*callback)(raw::EDIT_SECTION*)) const {
        if (!raw_ || !raw_->register_import_menu) {
            return;
        }
        raw_->register_import_menu(name, callback);
    }

    template <auto Callback> void register_import_menu(LPCWSTR name) const {
        if (!raw_ || !raw_->register_import_menu) {
            return;
        }
        raw_->register_import_menu(name, [](raw::EDIT_SECTION* raw_edit) {
            edit::EditSection edit{raw_edit};
            Callback(edit);
        });
    }

    void register_export_menu_raw(LPCWSTR name, void (*callback)(raw::EDIT_SECTION*)) const {
        if (!raw_ || !raw_->register_export_menu) {
            return;
        }
        raw_->register_export_menu(name, callback);
    }

    template <auto Callback> void register_export_menu(LPCWSTR name) const {
        if (!raw_ || !raw_->register_export_menu) {
            return;
        }
        raw_->register_export_menu(name, [](raw::EDIT_SECTION* raw_edit) {
            edit::EditSection edit{raw_edit};
            Callback(edit);
        });
    }

    void register_layer_menu_raw(LPCWSTR name, void (*callback)(raw::EDIT_SECTION*)) const {
        if (!raw_ || !raw_->register_layer_menu) {
            return;
        }
        raw_->register_layer_menu(name, callback);
    }

    template <auto Callback> void register_layer_menu(LPCWSTR name) const {
        if (!raw_ || !raw_->register_layer_menu) {
            return;
        }
        raw_->register_layer_menu(name, [](raw::EDIT_SECTION* raw_edit) {
            edit::EditSection edit{raw_edit};
            Callback(edit);
        });
    }

    void register_object_menu_raw(LPCWSTR name, void (*callback)(raw::EDIT_SECTION*)) const {
        if (!raw_ || !raw_->register_object_menu) {
            return;
        }
        raw_->register_object_menu(name, callback);
    }

    template <auto Callback> void register_object_menu(LPCWSTR name) const {
        if (!raw_ || !raw_->register_object_menu) {
            return;
        }
        raw_->register_object_menu(name, [](raw::EDIT_SECTION* raw_edit) {
            edit::EditSection edit{raw_edit};
            Callback(edit);
        });
    }

    void register_config_menu(LPCWSTR name, void (*callback)(HWND, HINSTANCE)) const {
        if (!raw_ || !raw_->register_config_menu) {
            return;
        }
        raw_->register_config_menu(name, callback);
    }

    void register_edit_menu_raw(LPCWSTR name, void (*callback)(raw::EDIT_SECTION*)) const {
        if (!raw_ || !raw_->register_edit_menu) {
            return;
        }
        raw_->register_edit_menu(name, callback);
    }

    template <auto Callback> void register_edit_menu(LPCWSTR name) const {
        if (!raw_ || !raw_->register_edit_menu) {
            return;
        }
        raw_->register_edit_menu(name, [](raw::EDIT_SECTION* raw_edit) {
            edit::EditSection edit{raw_edit};
            Callback(edit);
        });
    }

    void register_file_drop_handler_raw(LPCWSTR name, LPCWSTR filefilter,
                                        void (*callback)(raw::EDIT_SECTION*, LPCWSTR)) const {
        if (!raw_ || !raw_->register_file_drop_handler) {
            return;
        }
        raw_->register_file_drop_handler(name, filefilter, callback);
    }

    template <auto Callback> void register_file_drop_handler(LPCWSTR name, LPCWSTR filefilter) const {
        if (!raw_ || !raw_->register_file_drop_handler) {
            return;
        }
        raw_->register_file_drop_handler(name, filefilter, [](raw::EDIT_SECTION* raw_edit, LPCWSTR file) {
            edit::EditSection edit{raw_edit};
            Callback(edit, file);
        });
    }

    void register_object_item_menu_raw(LPCWSTR name, bool allow_effect_only,
                                       void (*callback)(raw::EDIT_SECTION*, edit::ObjectHandle, LPCWSTR,
                                                        LPCWSTR)) const {
        if (!raw_ || !raw_->register_object_item_menu) {
            return;
        }
        raw_->register_object_item_menu(name, allow_effect_only, callback);
    }

    template <auto Callback> void register_object_item_menu(LPCWSTR name, bool allow_effect_only) const {
        if (!raw_ || !raw_->register_object_item_menu) {
            return;
        }
        raw_->register_object_item_menu(
            name, allow_effect_only,
            [](raw::EDIT_SECTION* raw_edit, edit::ObjectHandle object, LPCWSTR effect, LPCWSTR item) {
                edit::EditSection edit{raw_edit};
                Callback(edit, object, effect, item);
            });
    }

    void register_project_load_handler(void (*callback)(raw::PROJECT_FILE*)) const {
        if (!raw_ || !raw_->register_project_load_handler) {
            return;
        }
        raw_->register_project_load_handler(callback);
    }

    void register_project_save_handler(void (*callback)(raw::PROJECT_FILE*)) const {
        if (!raw_ || !raw_->register_project_save_handler) {
            return;
        }
        raw_->register_project_save_handler(callback);
    }

    void register_clear_cache_handler(void (*callback)(raw::EDIT_SECTION*)) const {
        if (!raw_ || !raw_->register_clear_cache_handler) {
            return;
        }
        raw_->register_clear_cache_handler(callback);
    }

    void register_change_scene_handler(void (*callback)(raw::EDIT_SECTION*)) const {
        if (!raw_ || !raw_->register_change_scene_handler) {
            return;
        }
        raw_->register_change_scene_handler(callback);
    }

    raw::HOST_APP_TABLE* raw() const { return raw_; }

  private:
    raw::HOST_APP_TABLE* raw_;
};

template <typename T>
concept RegisterablePlugin = requires(T& plugin, HostApp& host) {
    { plugin.register_impl(host) } -> std::same_as<void>;
};

template <typename T>
concept ProjectLoadPlugin = requires(T& plugin, edit::ProjectFile& project) {
    { plugin.on_project_load_impl(project) } -> std::same_as<void>;
};

template <typename T>
concept ProjectSavePlugin = requires(T& plugin, edit::ProjectFile& project) {
    { plugin.on_project_save_impl(project) } -> std::same_as<void>;
};

template <typename T>
concept ClearCachePlugin = requires(T& plugin, edit::EditSection& edit) {
    { plugin.on_clear_cache_impl(edit) } -> std::same_as<void>;
};

template <typename T>
concept ChangeScenePlugin = requires(T& plugin, edit::EditSection& edit) {
    { plugin.on_change_scene_impl(edit) } -> std::same_as<void>;
};

class GenericPluginBase {};

template <typename Derived> class GenericPlugin : public aviutl2::Singleton<Derived>, public GenericPluginBase {
  protected:
    using token = typename aviutl2::Singleton<Derived>::token;

  public:
    LPCWSTR name_ = nullptr;
    LPCWSTR information_ = nullptr;

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

    const raw::COMMON_PLUGIN_TABLE* plugin_table() const {
        static raw::COMMON_PLUGIN_TABLE table = {
            .name = derived()->name_,
            .information = derived()->information_,
        };
        return &table;
    }

    static void register_plugin(raw::HOST_APP_TABLE* host_raw) {
        static_assert(RegisterablePlugin<Derived>, "GenericPlugin requires: void register_impl(HostApp& host)");

        try {
            HostApp host(host_raw);

            if constexpr (ProjectLoadPlugin<Derived>) {
                host.register_project_load_handler(&Derived::on_project_load);
            }
            if constexpr (ProjectSavePlugin<Derived>) {
                host.register_project_save_handler(&Derived::on_project_save);
            }
            if constexpr (ClearCachePlugin<Derived>) {
                host.register_clear_cache_handler(&Derived::on_clear_cache);
            }
            if constexpr (ChangeScenePlugin<Derived>) {
                host.register_change_scene_handler(&Derived::on_change_scene);
            }

            Derived::instance().register_impl(host);
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to register plugin: {}", utils::to_wstring(e.what()));
        } catch (...) {
            aviutl2::logger::error(L"Failed to register plugin: unknown error");
        }
    }

    static void on_project_load(raw::PROJECT_FILE* project_raw) {
        try {
            if constexpr (ProjectLoadPlugin<Derived>) {
                edit::ProjectFile project{project_raw};
                Derived::instance().on_project_load_impl(project);
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to project load handler: {}", utils::to_wstring(e.what()));
        } catch (...) {
            aviutl2::logger::error(L"Failed to project load handler: unknown error");
        }
    }

    static void on_project_save(raw::PROJECT_FILE* project_raw) {
        try {
            if constexpr (ProjectSavePlugin<Derived>) {
                edit::ProjectFile project{project_raw};
                Derived::instance().on_project_save_impl(project);
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to project save handler: {}", utils::to_wstring(e.what()));
        } catch (...) {
            aviutl2::logger::error(L"Failed to project save handler: unknown error");
        }
    }

    static void on_clear_cache(raw::EDIT_SECTION* edit_raw) {
        try {
            if constexpr (ClearCachePlugin<Derived>) {
                edit::EditSection edit{edit_raw};
                Derived::instance().on_clear_cache_impl(edit);
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to clear cache handler: {}", utils::to_wstring(e.what()));
        } catch (...) {
            aviutl2::logger::error(L"Failed to clear cache handler: unknown error");
        }
    }

    static void on_change_scene(raw::EDIT_SECTION* edit_raw) {
        try {
            if constexpr (ChangeScenePlugin<Derived>) {
                edit::EditSection edit{edit_raw};
                Derived::instance().on_change_scene_impl(edit);
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to change scene handler: {}", utils::to_wstring(e.what()));
        } catch (...) {
            aviutl2::logger::error(L"Failed to change scene handler: unknown error");
        }
    }

  private:
    Derived* derived() { return static_cast<Derived*>(this); }
    const Derived* derived() const { return static_cast<const Derived*>(this); }
};

template <typename T>
concept GenericPluginType = std::derived_from<std::remove_cvref_t<T>, GenericPluginBase> && requires(T& plugin) {
    { plugin.plugin_table() } -> std::same_as<const raw::COMMON_PLUGIN_TABLE*>;
};

}; // namespace aviutl2::generic

#define AVIUTL2_REGISTER_GENERIC_PLUGIN(PluginType)                                                                    \
    AVIUTL2_API void InitializeLogger(LOG_HANDLE* logger) { aviutl2::logger::initialize(logger); }                     \
    AVIUTL2_API void InitializeConfig(CONFIG_HANDLE* config) { aviutl2::config::initialize(config); }                  \
    AVIUTL2_API void InitializeCache(CACHE_HANDLE* cache) { aviutl2::cache::initialize(cache); }                       \
    AVIUTL2_API COMMON_PLUGIN_TABLE* GetCommonPluginTable() {                                                          \
        return const_cast<COMMON_PLUGIN_TABLE*>(PluginType::instance().plugin_table());                                \
    }                                                                                                                  \
    AVIUTL2_API void RegisterPlugin(HOST_APP_TABLE* host) { PluginType::instance().register_plugin(host); }            \
    AVIUTL2_API bool InitializePlugin(DWORD version) { return PluginType::instance().initialize(version); }            \
    AVIUTL2_API void UninitializePlugin() { PluginType::instance().uninitialize(); }
