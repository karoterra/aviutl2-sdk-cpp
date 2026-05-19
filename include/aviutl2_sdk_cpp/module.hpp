#pragma once

#include <initializer_list>
#include <vector>

#include <aviutl2_sdk_cpp/cache.hpp>
#include <aviutl2_sdk_cpp/common.hpp>
#include <aviutl2_sdk_cpp/config.hpp>
#include <aviutl2_sdk_cpp/edit.hpp>
#include <aviutl2_sdk_cpp/logger.hpp>
#include <aviutl2_sdk_cpp/raw/module.hpp>
#include <aviutl2_sdk_cpp/utils.hpp>

namespace aviutl2::module {

using ScriptModuleParam = raw::SCRIPT_MODULE_PARAM;

using ScriptModuleFunction = raw::SCRIPT_MODULE_FUNCTION;

template <typename Derived> class ScriptModule : public aviutl2::Singleton<Derived> {
  protected:
    using token = typename aviutl2::Singleton<Derived>::token;

  public:
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

    const raw::SCRIPT_MODULE_TABLE* plugin_table() const {
        static raw::SCRIPT_MODULE_TABLE table = {
            .information = derived()->information_,
            .functions = const_cast<raw::SCRIPT_MODULE_FUNCTION*>(derived()->functions_.data()),
        };
        return &table;
    }

    void add_functions(std::initializer_list<ScriptModuleFunction> functions) {
        if (!functions_.empty()) {
            functions_.pop_back();
        }

        for (const auto& f : functions) {
            functions_.push_back(f);
        }

        functions_.push_back({nullptr, nullptr});
    }

    const std::vector<ScriptModuleFunction>& functions() const { return this->functions_; }

  private:
    std::vector<ScriptModuleFunction> functions_{
        {nullptr, nullptr},
    };

    Derived* derived() { return static_cast<Derived*>(this); }
    const Derived* derived() const { return static_cast<const Derived*>(this); }
};

}; // namespace aviutl2::module

#define AVIUTL2_REGISTER_SCRIPT_MODULE(PluginType)                                                                     \
    AVIUTL2_API void InitializeLogger(LOG_HANDLE* logger) { aviutl2::logger::initialize(logger); }                     \
    AVIUTL2_API void InitializeConfig(CONFIG_HANDLE* config) { aviutl2::config::initialize(config); }                  \
    AVIUTL2_API void InitializeCache(CACHE_HANDLE* cache) { aviutl2::cache::initialize(cache); }                       \
    AVIUTL2_API SCRIPT_MODULE_TABLE* GetScriptModuleTable() {                                                          \
        return const_cast<SCRIPT_MODULE_TABLE*>(PluginType::instance().plugin_table());                                \
    }                                                                                                                  \
    AVIUTL2_API bool InitializePlugin(DWORD version) { return PluginType::instance().initialize(version); }            \
    AVIUTL2_API void UninitializePlugin() { PluginType::instance().uninitialize(); }
