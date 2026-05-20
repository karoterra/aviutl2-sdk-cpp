#pragma once

#include <concepts>
#include <functional>
#include <optional>
#include <string>
#include <vector>

#include <aviutl2_sdk_cpp/raw/plugin.hpp>
#include <aviutl2_sdk_cpp/utils.hpp>

namespace aviutl2::edit::detail {

inline std::string safe_string(LPCSTR s) {
    if (s) {
        return std::string{s};
    } else {
        return std::string{};
    }
}

inline std::wstring safe_wstring(LPCWSTR s) {
    if (s) {
        return std::wstring{s};
    } else {
        return std::wstring{};
    }
}

}; // namespace aviutl2::edit::detail

namespace aviutl2::edit {

class EditHandle;
class ReadSection;
class EditSection;

using ObjectHandle = raw::OBJECT_HANDLE;
using ObjectLayerFrame = raw::OBJECT_LAYER_FRAME;
using MediaInfo = raw::MEDIA_INFO;

struct LayerFrame {
    int layer;
    int frame;
};

enum class ModuleType : int {
    SCRIPT_FILTER = raw::MODULE_INFO::TYPE_SCRIPT_FILTER,
    SCRIPT_OBJECT = raw::MODULE_INFO::TYPE_SCRIPT_OBJECT,
    SCRIPT_CAMERA = raw::MODULE_INFO::TYPE_SCRIPT_CAMERA,
    SCRIPT_TRACK = raw::MODULE_INFO::TYPE_SCRIPT_TRACK,
    SCRIPT_MODULE = raw::MODULE_INFO::TYPE_SCRIPT_MODULE,
    PLUGIN_INPUT = raw::MODULE_INFO::TYPE_PLUGIN_INPUT,
    PLUGIN_OUTPUT = raw::MODULE_INFO::TYPE_PLUGIN_OUTPUT,
    PLUGIN_FILTER = raw::MODULE_INFO::TYPE_PLUGIN_FILTER,
    PLUGIN_COMMON = raw::MODULE_INFO::TYPE_PLUGIN_COMMON,
};

struct ModuleInfo {
    ModuleType type{};
    std::wstring name{};
    std::wstring information{};

    static ModuleInfo from_raw(raw::MODULE_INFO* raw) {
        if (!raw) {
            return {};
        }
        return ModuleInfo{
            .type = static_cast<ModuleType>(raw->type),
            .name = detail::safe_wstring(raw->name),
            .information = detail::safe_wstring(raw->information),
        };
    }
};

class ProjectFile {
  public:
    explicit ProjectFile(raw::PROJECT_FILE* raw) : raw_(raw) {}

    std::string get_param_string(LPCSTR key) const {
        if (!raw_ || !raw_->get_param_string) {
            return {};
        }
        return detail::safe_string(raw_->get_param_string(key));
    }

    void set_param_string(LPCSTR key, LPCSTR value) const {
        if (!raw_ || !raw_->set_param_string) {
            return;
        }
        raw_->set_param_string(key, value);
    }

    template <typename T> bool get_param_binary(LPCSTR key, T* data) const {
        if (!raw_ || !raw_->get_param_binary) {
            return false;
        }
        return raw_->get_param_binary(key, data, sizeof(T));
    }

    template <typename T> void set_param_binary(LPCSTR key, const T* data) const {
        static_assert(sizeof(T) <= 4096, "set_param_binary size must be 4096 bytes or less");

        if (!raw_ || !raw_->set_param_binary) {
            return;
        }
        raw_->set_param_binary(key, const_cast<T*>(data), sizeof(T));
    }

    void clear_params() const {
        if (!raw_ || !raw_->clear_params) {
            return;
        }
        raw_->clear_params();
    }

    std::wstring get_project_file_path() const {
        if (!raw_ || !raw_->get_project_file_path) {
            return {};
        }
        return raw_->get_project_file_path();
    }

  private:
    raw::PROJECT_FILE* raw_;
};

using EditInfo = raw::EDIT_INFO;

enum class EffectType : int {
    FILTER = raw::EDIT_HANDLE::EFFECT_TYPE_FILTER,
    INPUT = raw::EDIT_HANDLE::EFFECT_TYPE_INPUT,
    TRANSITION = raw::EDIT_HANDLE::EFFECT_TYPE_TRANSITION,
    CONTROL = raw::EDIT_HANDLE::EFFECT_TYPE_CONTROL,
    OUTPUT = raw::EDIT_HANDLE::EFFECT_TYPE_OUTPUT,
};

enum class EffectFlag : int {
    VIDEO = raw::EDIT_HANDLE::EFFECT_FLAG_VIDEO,
    AUDIO = raw::EDIT_HANDLE::EFFECT_FLAG_AUDIO,
    FILTER = raw::EDIT_HANDLE::EFFECT_FLAG_FILTER,
    CAMERA = raw::EDIT_HANDLE::EFFECT_FLAG_CAMERA,
};

enum class EditState : int {
    EDIT = raw::EDIT_HANDLE::EDIT_STATE_EDIT,
    PLAY = raw::EDIT_HANDLE::EDIT_STATE_PLAY,
    SAVE = raw::EDIT_HANDLE::EDIT_STATE_SAVE,
};

enum class EffectItemType : int {
    INTEGER = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_INTEGER,
    NUMBER = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_NUMBER,
    CHECK = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_CHECK,
    TEXT = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_TEXT,
    STRING = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_STRING,
    FILE = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_FILE,
    COLOR = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_COLOR,
    SELECT = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_SELECT,
    SCENE = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_SCENE,
    RANGE = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_RANGE,
    COMBO = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_COMBO,
    MASK = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_MASK,
    FONT = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_FONT,
    FIGURE = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_FIGURE,
    DATA = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_DATA,
    FOLDER = raw::EDIT_HANDLE::EFFECT_ITEM_TYPE_FOLDER,
};

struct Effect {
    std::wstring name;
    EffectType type;
    EffectFlag flag;
};

struct EffectItem {
    std::wstring name;
    EffectItemType type;
};

class EditHandle {
  public:
    EditHandle() : raw_(nullptr) {}
    explicit EditHandle(raw::EDIT_HANDLE* raw) : raw_(raw) {}

    void init(raw::EDIT_HANDLE* raw) { raw_ = raw; }

    template <typename F>
        requires std::invocable<F, EditSection&>
    bool call_edit_section(F&& func) {
        if (!raw_ || !raw_->call_edit_section_param) {
            return false;
        }

        struct Context {
            F& func;
        } ctx{func};

        auto callback = [](void* param, raw::EDIT_SECTION* raw_edit) {
            auto& ctx = *static_cast<Context*>(param);
            EditSection edit(raw_edit);
            ctx.func(edit);
        };
        return raw_->call_edit_section_param(&ctx, callback);
    }

    template <typename R, typename F>
        requires std::invocable<F, EditSection&> && std::convertible_to<std::invoke_result_t<F, EditSection&>, R>
    std::optional<R> call_edit_section_result(F&& func) {
        if (!raw_ || !raw_->call_edit_section_param) {
            return std::nullopt;
        }

        struct Context {
            F& func;
            std::optional<R> result{};
        } ctx{func};

        auto callback = [](void* param, raw::EDIT_SECTION* raw_edit) {
            auto& ctx = *static_cast<Context*>(param);
            EditSection edit(raw_edit);
            ctx.result.emplace(ctx.func(edit));
        };
        if (!raw_->call_edit_section_param(&ctx, callback)) {
            return std::nullopt;
        }
        return ctx.result;
    }

    template <typename F>
        requires std::invocable<F, ReadSection&>
    bool call_read_section(F&& func) {
        if (!raw_ || !raw_->call_read_section_param) {
            return false;
        }

        struct Context {
            F& func;
        } ctx{func};

        auto callback = [](void* param, raw::EDIT_SECTION* raw_edit) {
            auto& ctx = *static_cast<Context*>(param);
            ReadSection edit(raw_edit);
            ctx.func(edit);
        };
        return raw_->call_read_section_param(&ctx, callback);
    }

    template <typename R, typename F>
        requires std::invocable<F, ReadSection&> && std::convertible_to<std::invoke_result_t<F, ReadSection&>, R>
    std::optional<R> call_read_section_result(F&& func) {
        if (!raw_ || !raw_->call_read_section_param) {
            return std::nullopt;
        }

        struct Context {
            F& func;
            std::optional<R> result{};
        } ctx{func};

        auto callback = [](void* param, raw::EDIT_SECTION* raw_edit) {
            auto& ctx = *static_cast<Context*>(param);
            ReadSection edit(raw_edit);
            ctx.result.emplace(ctx.func(edit));
        };
        if (!raw_->call_read_section_param(&ctx, callback)) {
            return std::nullopt;
        }
        return ctx.result;
    }

    EditInfo get_edit_info() const {
        if (!raw_ || !raw_->get_edit_info) {
            return {};
        }
        EditInfo info;
        raw_->get_edit_info(&info, sizeof(EditInfo));
        return info;
    }

    void restart_host_app() const {
        if (!raw_ || !raw_->restart_host_app) {
            return;
        }
        raw_->restart_host_app();
    }

    HWND get_host_app_window() const {
        if (!raw_ || !raw_->get_host_app_window) {
            return 0;
        }
        return raw_->get_host_app_window();
    }

    EditState get_edit_state() const {
        if (!raw_ || !raw_->get_edit_state) {
            return EditState::EDIT;
        }
        return static_cast<EditState>(raw_->get_edit_state());
    }

    template <typename F>
        requires std::invocable<F, Effect>
    void enum_effects(F&& func) {
        if (!raw_ || !raw_->enum_effect_name) {
            return;
        }

        struct Context {
            F& func;
        } ctx{func};

        auto callback = [](void* param, LPCWSTR name, int type, int flag) {
            auto& ctx = *static_cast<Context*>(param);
            Effect effect{
                .name = detail::safe_wstring(name),
                .type = static_cast<EffectType>(type),
                .flag = static_cast<EffectFlag>(flag),
            };
            ctx.func(effect);
        };
        raw_->enum_effect_name(&ctx, callback);
    }

    std::vector<Effect> get_effects() {
        std::vector<Effect> effects;
        auto f = [&effects](Effect e) { effects.push_back(e); };
        enum_effects(f);
        return effects;
    }

    template <typename F>
        requires std::invocable<F, ModuleInfo>
    void enum_modules(F&& func) {
        if (!raw_ || !raw_->enum_module_info) {
            return;
        }

        struct Context {
            F& func;
        } ctx{func};

        auto callback = [](void* param, MODULE_INFO* info) {
            auto& ctx = *static_cast<Context*>(param);
            ModuleInfo mod = ModuleInfo::from_raw(info);
            ctx.func(mod);
        };
        raw_->enum_module_info(&ctx, callback);
    }

    std::vector<ModuleInfo> get_modules() {
        std::vector<ModuleInfo> modules;
        auto f = [&modules](ModuleInfo m) { modules.push_back(m); };
        enum_modules(f);
        return modules;
    }

    template <typename F>
        requires std::invocable<F, EffectItem>
    bool enum_effect_items(LPCWSTR effect, F&& func) {
        if (!raw_ || !raw_->enum_effect_item) {
            return false;
        }

        struct Context {
            F& func;
        } ctx{func};

        auto callback = [](void* param, LPCWSTR name, int type) {
            auto& ctx = *static_cast<Context*>(param);
            EffectItem item{
                .name = detail::safe_wstring(name),
                .type = static_cast<EffectItemType>(type),
            };
            ctx.func(item);
        };
        return raw_->enum_effect_item(effect, &ctx, callback);
    }

    std::vector<EffectItem> get_effect_items(LPCWSTR effect) {
        std::vector<EffectItem> items;
        auto f = [&items](EffectItem i) { items.push_back(i); };
        enum_effect_items(effect, f);
        return items;
    }

    raw::EDIT_HANDLE* raw() const { return raw_; }

  private:
    raw::EDIT_HANDLE* raw_;
};

class ReadSection {
  public:
    explicit ReadSection(raw::EDIT_SECTION* raw) : raw_(raw) {}

    ObjectHandle find_object(int layer, int frame) const {
        if (!raw_ || !raw_->find_object) {
            return nullptr;
        }
        return raw_->find_object(layer, frame);
    }

    int count_object_effect(ObjectHandle object, LPCWSTR effect) const {
        if (!raw_ || !raw_->count_object_effect) {
            return 0;
        }
        return raw_->count_object_effect(object, effect);
    }

    ObjectLayerFrame get_object_layer_frame(ObjectHandle object) const {
        if (!raw_ || !raw_->get_object_layer_frame) {
            return {};
        }
        return raw_->get_object_layer_frame(object);
    }

    std::string get_object_alias(ObjectHandle object) const {
        if (!raw_ || !raw_->get_object_alias) {
            return {};
        }
        return detail::safe_string(raw_->get_object_alias(object));
    }

    std::string get_object_item_value(ObjectHandle object, LPCWSTR effect, LPCWSTR item) const {
        if (!raw_ || !raw_->get_object_item_value) {
            return {};
        }
        return detail::safe_string(raw_->get_object_item_value(object, effect, item));
    }

    ObjectHandle get_focus_object() const {
        if (!raw_ || !raw_->get_focus_object) {
            return nullptr;
        }
        return raw_->get_focus_object();
    }

    ObjectHandle get_selected_object(int index) const {
        if (!raw_ || !raw_->get_selected_object) {
            return nullptr;
        }
        return raw_->get_selected_object(index);
    }

    int get_selected_object_num() const {
        if (!raw_ || !raw_->get_selected_object_num) {
            return 0;
        }
        return raw_->get_selected_object_num();
    }

    bool is_support_media_file(LPCWSTR file, bool strict) const {
        if (!raw_ || !raw_->is_support_media_file) {
            return false;
        }
        return raw_->is_support_media_file(file, strict);
    }

    std::optional<MediaInfo> get_media_info(LPCWSTR file) const {
        if (!raw_ || !raw_->get_media_info) {
            return std::nullopt;
        }
        MediaInfo info;
        if (!raw_->get_media_info(file, &info, sizeof(MediaInfo))) {
            return std::nullopt;
        }
        return info;
    }

    std::wstring get_object_name(ObjectHandle object) const {
        if (!raw_ || !raw_->get_object_name) {
            return {};
        }
        return detail::safe_wstring(raw_->get_object_name(object));
    }

    std::wstring get_layer_name(int layer) const {
        if (!raw_ || !raw_->get_layer_name) {
            return {};
        }
        return detail::safe_wstring(raw_->get_layer_name(layer));
    }

    std::wstring get_scene_name() const {
        if (!raw_ || !raw_->get_scene_name) {
            return {};
        }
        return detail::safe_wstring(raw_->get_scene_name());
    }

    bool get_layer_enable(int layer) const {
        if (!raw_ || !raw_->get_layer_enable) {
            return false;
        }
        return raw_->get_layer_enable(layer);
    }

    bool get_layer_lock(int layer) const {
        if (!raw_ || !raw_->get_layer_lock) {
            return false;
        }
        return raw_->get_layer_lock(layer);
    }

    int get_object_section_num(ObjectHandle object) const {
        if (!raw_ || !raw_->get_object_section_num) {
            return 0;
        }
        return raw_->get_object_section_num(object);
    }

    int get_focus_object_section() const {
        if (!raw_ || !raw_->get_focus_object_section) {
            return -1;
        }
        return raw_->get_focus_object_section();
    }

  protected:
    raw::EDIT_SECTION* raw_;
};

class EditSection : public ReadSection {
  public:
    using ReadSection::ReadSection;

    EditInfo* info() const {
        if (!raw_) {
            return nullptr;
        }
        return raw_->info;
    }

    ObjectHandle create_object_from_alias(LPCSTR alias, int layer, int frame, int length) const {
        if (!raw_ || !raw_->create_object_from_alias) {
            return nullptr;
        }
        return raw_->create_object_from_alias(alias, layer, frame, length);
    }

    bool set_object_item_value(ObjectHandle object, LPCWSTR effect, LPCWSTR item, LPCSTR value) const {
        if (!raw_ || !raw_->set_object_item_value) {
            return false;
        }
        return raw_->set_object_item_value(object, effect, item, value);
    }

    bool move_object(ObjectHandle object, int layer, int frame) const {
        if (!raw_ || !raw_->move_object) {
            return false;
        }
        return raw_->move_object(object, layer, frame);
    }

    void delete_object(ObjectHandle object) const {
        if (!raw_ || !raw_->delete_object) {
            return;
        }
        raw_->delete_object(object);
    }

    void set_focus_object(ObjectHandle object) const {
        if (!raw_ || !raw_->set_focus_object) {
            return;
        }
        raw_->set_focus_object(object);
    }

    std::optional<ProjectFile> get_project_file(EditHandle& edit) const {
        if (!raw_ || !raw_->get_project_file) {
            return std::nullopt;
        }
        raw::PROJECT_FILE* project = raw_->get_project_file(edit.raw());
        if (!project) {
            return std::nullopt;
        }
        return ProjectFile(project);
    }

    std::optional<LayerFrame> get_mouse_layer_frame() const {
        if (!raw_ || !raw_->get_mouse_layer_frame) {
            return std::nullopt;
        }
        LayerFrame result;
        if (!raw_->get_mouse_layer_frame(&result.layer, &result.frame)) {
            return std::nullopt;
        }
        return result;
    }

    std::optional<LayerFrame> pos_to_layer_frame(int x, int y) const {
        if (!raw_ || !raw_->pos_to_layer_frame) {
            return std::nullopt;
        }
        LayerFrame result;
        if (!raw_->pos_to_layer_frame(x, y, &result.layer, &result.frame)) {
            return std::nullopt;
        }
        return result;
    }

    ObjectHandle create_object_from_media_file(LPCWSTR file, int layer, int frame, int length) const {
        if (!raw_ || !raw_->create_object_from_media_file) {
            return nullptr;
        }
        return raw_->create_object_from_media_file(file, layer, frame, length);
    }

    ObjectHandle create_object(LPCWSTR effect, int layer, int frame, int length) const {
        if (!raw_ || !raw_->create_object) {
            return nullptr;
        }
        return raw_->create_object(effect, layer, frame, length);
    }

    void set_cursor_layer_frame(int layer, int frame) const {
        if (!raw_ || !raw_->set_cursor_layer_frame) {
            return;
        }
        raw_->set_cursor_layer_frame(layer, frame);
    }

    void set_display_layer_frame(int layer, int frame) const {
        if (!raw_ || !raw_->set_display_layer_frame) {
            return;
        }
        raw_->set_display_layer_frame(layer, frame);
    }

    void set_select_range(int start, int end) const {
        if (!raw_ || !raw_->set_select_range) {
            return;
        }
        raw_->set_select_range(start, end);
    }

    void set_grid_bpm(float tempo, int beat, float offset) const {
        if (!raw_ || !raw_->set_grid_bpm) {
            return;
        }
        raw_->set_grid_bpm(tempo, beat, offset);
    }

    void set_object_name(ObjectHandle object, LPCWSTR name) const {
        if (!raw_ || !raw_->set_object_name) {
            return;
        }
        raw_->set_object_name(object, name);
    }

    void set_layer_name(int layer, LPCWSTR name) const {
        if (!raw_ || !raw_->set_layer_name) {
            return;
        }
        raw_->set_layer_name(layer, name);
    }

    void set_scene_name(LPCWSTR name) const {
        if (!raw_ || !raw_->set_scene_name) {
            return;
        }
        raw_->set_scene_name(name);
    }

    void set_scene_size(int width, int height) const {
        if (!raw_ || !raw_->set_scene_size) {
            return;
        }
        raw_->set_scene_size(width, height);
    }

    void set_scene_frame_rate(int rate, int scale) const {
        if (!raw_ || !raw_->set_scene_frame_rate) {
            return;
        }
        raw_->set_scene_frame_rate(rate, scale);
    }

    void set_scene_sample_rate(int sample_rate) const {
        if (!raw_ || !raw_->set_scene_sample_rate) {
            return;
        }
        raw_->set_scene_sample_rate(sample_rate);
    }

    void set_layer_enable(int layer, bool enable) const {
        if (!raw_ || !raw_->set_layer_enable) {
            return;
        }
        raw_->set_layer_enable(layer, enable);
    }

    void set_layer_lock(int layer, bool lock) const {
        if (!raw_ || !raw_->set_layer_lock) {
            return;
        }
        raw_->set_layer_lock(layer, lock);
    }
};

// using EditHandle = raw::EDIT_HANDLE;

}; // namespace aviutl2::edit

namespace aviutl2::utils::enum_utils {

template <> struct enable_bitmask<edit::EffectFlag> : std::true_type {};

}; // namespace aviutl2::utils::enum_utils
