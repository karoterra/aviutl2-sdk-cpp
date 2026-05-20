#pragma once

#include <concepts>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include <aviutl2_sdk_cpp/cache.hpp>
#include <aviutl2_sdk_cpp/common.hpp>
#include <aviutl2_sdk_cpp/config.hpp>
#include <aviutl2_sdk_cpp/edit.hpp>
#include <aviutl2_sdk_cpp/logger.hpp>
#include <aviutl2_sdk_cpp/raw/filter.hpp>
#include <aviutl2_sdk_cpp/raw/plugin.hpp>
#include <aviutl2_sdk_cpp/utils.hpp>

namespace aviutl2::filter {

using EditSection = edit::EditSection;

/// @brief オブジェクトハンドル
using ObjectHandle = edit::ObjectHandle;

template <typename T>
concept FilterItem = requires(T& a) {
    { a.item() } -> std::convertible_to<const void*>;
};

/// @brief トラックバー項目
/// @tparam T 値の型
template <typename T> class Track {
  private:
    raw::FILTER_ITEM_TRACK item_;

  public:
    using value_type = T;

    Track(LPCWSTR name, double value, double s, double e, double step = 1.0) : item_(name, value, s, e, step) {}

    T value() const { return static_cast<T>(item_.value); }
    T s() const { return static_cast<T>(item_.s); }
    T e() const { return static_cast<T>(item_.e); }
    double step() const { return item_.step; }

    const raw::FILTER_ITEM_TRACK* item() const { return &item_; }
};

/// @brief チェックボックス項目
class Check {
  private:
    raw::FILTER_ITEM_CHECK item_;

  public:
    Check(LPCWSTR name, bool value) : item_(name, value) {}

    bool value() const { return item_.value; }

    const raw::FILTER_ITEM_CHECK* item() const { return &item_; }
};

/// @brief 色選択項目
class Color {
  private:
    raw::FILTER_ITEM_COLOR item_;

  public:
    Color(LPCWSTR name, int code) : item_(name, code) {}
    Color(LPCWSTR name, unsigned char r, unsigned char g, unsigned char b) : item_(name, 0) {
        item_.value.r = r;
        item_.value.g = g;
        item_.value.b = b;
    }

    int code() const { return item_.value.code; }
    unsigned char r() const { return item_.value.r; }
    unsigned char g() const { return item_.value.g; }
    unsigned char b() const { return item_.value.b; }

    const raw::FILTER_ITEM_COLOR* item() const { return &item_; }
};

/// @brief 選択リスト項目
/// @tparam T 値の型
template <typename T> class Select {
  private:
    std::vector<raw::FILTER_ITEM_SELECT::ITEM> item_list;
    raw::FILTER_ITEM_SELECT item_;

  public:
    using value_type = T;

    struct Choice {
        LPCWSTR name; // 選択肢の名前
        T value;      // 選択肢の値
    };

    Select(LPCWSTR name, T value, std::initializer_list<Choice> list) : item_(name, static_cast<int>(value), nullptr) {
        for (const auto& c : list) {
            item_list.push_back({c.name, static_cast<int>(c.value)});
        }
        item_list.push_back({nullptr, 0}); // 終端
        item_.list = item_list.data();
    }

    T value() const { return static_cast<T>(item_.value); }
    const raw::FILTER_ITEM_SELECT::ITEM* list() const { return item_.list; }

    const raw::FILTER_ITEM_SELECT* item() const { return &item_; }
};

/// @brief ファイル選択項目
class File {
  private:
    raw::FILTER_ITEM_FILE item_;
    std::wstring filefilter_;

  public:
    File(LPCWSTR name, LPCWSTR value, LPCWSTR filefilter) : item_(name, value, filefilter) {}
    File(LPCWSTR name, LPCWSTR value, std::initializer_list<utils::FileFilterItem> filefilter)
        : item_(name, value, nullptr) {
        filefilter_ = utils::make_file_filter(filefilter);
        item_.filefilter = this->filefilter_.c_str();
    }

    LPCWSTR value() const { return item_.value; }
    LPCWSTR filefilter() const { return item_.filefilter; }

    const raw::FILTER_ITEM_FILE* item() const { return &item_; }
};

/// @brief 汎用データ項目
/// @tparam T データの型 (サイズは1024バイト以下である必要があります)
template <typename T> class Data {
  private:
    raw::FILTER_ITEM_DATA<T> item_;

  public:
    using value_type = T;
    static_assert(sizeof(T) <= 1024, "Data item size must be 1024 bytes or less");

    Data(LPCWSTR name, T default_value) : item_(name) { item_.default_value = default_value; }

    const T* value() const { return item_.value; }
    T* value() { return item_.value; }
    int size() const { return item_.size; }

    const raw::FILTER_ITEM_DATA<T>* item() const { return &item_; }
};

/// @brief ボタン項目
class Button {
  private:
    raw::FILTER_ITEM_BUTTON item_;

  public:
    using callback_type = void (*)(raw::EDIT_SECTION*);

    Button(LPCWSTR name, callback_type func) : item_(name, func) {}

    const raw::FILTER_ITEM_BUTTON* item() const { return &item_; }
};

/// @brief 文字列項目
class String {
  private:
    raw::FILTER_ITEM_STRING item_;

  public:
    String(LPCWSTR name, LPCWSTR value) : item_(name, value) {}

    LPCWSTR name() const { return item_.name; }
    LPCWSTR value() const { return item_.value; }

    const raw::FILTER_ITEM_STRING* item() const { return &item_; }
};

/// @brief テキスト項目
class Text {
  private:
    raw::FILTER_ITEM_TEXT item_;

  public:
    Text(LPCWSTR name, LPCWSTR value) : item_(name, value) {}

    LPCWSTR name() const { return item_.name; }
    LPCWSTR value() const { return item_.value; }

    const raw::FILTER_ITEM_TEXT* item() const { return &item_; }
};

/// @brief フォルダ選択項目
class Folder {
  private:
    raw::FILTER_ITEM_FOLDER item_;

  public:
    Folder(LPCWSTR name, LPCWSTR value) : item_(name, value) {}

    LPCWSTR name() const { return item_.name; }
    LPCWSTR value() const { return item_.value; }

    const raw::FILTER_ITEM_FOLDER* item() const { return &item_; }
};

struct LayoutItemBase {
    virtual ~LayoutItemBase() = default;
    virtual void* raw_item() = 0;
};

/// @brief 設定グループ項目
class Group : public LayoutItemBase {
  public:
    Group(LPCWSTR name, bool default_visible = true) : item_(name, default_visible) {}

    void* raw_item() override { return &item_; }

  private:
    raw::FILTER_ITEM_GROUP item_;
};

/// @brief セパレーター項目
class Separator : public LayoutItemBase {
  public:
    explicit Separator(LPCWSTR name) : item_(name) {}

    void* raw_item() override { return &item_; }

  private:
    raw::FILTER_ITEM_SEPARATOR item_;
};

template <typename T>
using ItemArg = std::conditional_t<FilterItem<std::remove_reference_t<T>>,
                                   std::reference_wrapper<std::remove_reference_t<T>>, std::decay_t<T>>;

template <typename T> auto make_item_arg(T&& value) {
    if constexpr (FilterItem<std::remove_reference_t<T>>) {
        return std::ref(value);
    } else {
        return std::forward<T>(value);
    }
}

template <typename... Children> struct GroupSpec {
    LPCWSTR name;
    bool default_visible;
    std::tuple<Children...> children;
};

template <typename... Children> auto group(LPCWSTR name, bool default_visible, Children&&... children) {
    return GroupSpec<ItemArg<Children>...>{
        name,
        default_visible,
        std::tuple<ItemArg<Children>...>{make_item_arg(std::forward<Children>(children))...},
    };
}

struct SeparatorSpec {
    LPCWSTR name;
};

inline SeparatorSpec separator(LPCWSTR name) { return {name}; }

using VertexColor = raw::VERTEX_COLOR;
using VertexColorNorm = raw::VERTEX_COLOR_NORM;
using VertexTexture = raw::VERTEX_TEXTURE;
using VertexTextureNorm = raw::VERTEX_TEXTURE_NORM;

using VertexType = raw::VERTEX_TYPE;
using SamplerMode = raw::SAMPLER_MODE;
using BlendMode = raw::BLEND_MODE;
using BillboardMode = raw::BILLBOARD_MODE;

/// @brief シーン情報構造体
using SceneInfo = raw::SCENE_INFO;

/// @brief オブジェクト情報構造体
using ObjectInfo = raw::OBJECT_INFO;

using ObjectImageParam = raw::OBJECT_IMAGE_PARAM;
using ObjectAudioParam = raw::OBJECT_AUDIO_PARAM;

/// @brief 画像フィルタ処理用構造体
using FilterProcVideo = raw::FILTER_PROC_VIDEO;

/// @brief 音声フィルタ処理用構造体
using FilterProcAudio = raw::FILTER_PROC_AUDIO;

enum class FilterFlag : int {
    VIDEO = raw::FILTER_PLUGIN_TABLE::FLAG_VIDEO, // 画像フィルタをサポートする
    AUDIO = raw::FILTER_PLUGIN_TABLE::FLAG_AUDIO, // 音声フィルタをサポートする
    INPUT =
        raw::FILTER_PLUGIN_TABLE::FLAG_INPUT, // メディアオブジェクトの初期入力をする (メディアオブジェクトにする場合)
    FILTER = raw::FILTER_PLUGIN_TABLE::FLAG_FILTER, // フィルタオブジェクトをサポートする
                                                    // (フィルタオブジェクトに対応する場合)
};

template <typename T>
concept ProcVideoPlugin = requires(T& a, FilterProcVideo* video) {
    { a.proc_video_impl(video) } -> std::convertible_to<bool>;
};

template <typename T>
concept ProcAudioPlugin = requires(T& a, FilterProcAudio* audio) {
    { a.proc_audio_impl(audio) } -> std::convertible_to<bool>;
};

template <typename Derived> class FilterPlugin : public aviutl2::Singleton<Derived> {
  protected:
    using token = typename aviutl2::Singleton<Derived>::token;

  public:
    FilterFlag flag_;
    LPCWSTR name_;
    LPCWSTR label_;
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

    const raw::FILTER_PLUGIN_TABLE* plugin_table() const {
        static raw::FILTER_PLUGIN_TABLE table = {
            .flag = static_cast<int>(derived()->flag_),
            .name = derived()->name_,
            .label = derived()->label_,
            .information = derived()->information_,
            .items = const_cast<void**>(derived()->items().data()),
        };

        if constexpr (ProcVideoPlugin<Derived>) {
            table.func_proc_video = &Derived::proc_video;
        } else {
            table.func_proc_video = nullptr;
        }

        if constexpr (ProcAudioPlugin<Derived>) {
            table.func_proc_audio = &Derived::proc_audio;
        } else {
            table.func_proc_audio = nullptr;
        }

        return &table;
    }

    template <typename... Items> void add_items(Items&&... items) {
        raw_items_.clear();
        owned_layout_items_.clear();

        (append_item(std::forward<Items>(items)), ...);

        raw_items_.push_back(nullptr); // 終端
    }

    static bool proc_video(FilterProcVideo* video) {
        try {
            if constexpr (ProcVideoPlugin<Derived>) {
                return Derived::instance().proc_video_impl(video);
            } else {
                return false;
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to process video: {}", utils::to_wstring(e.what()));
            return false;
        } catch (...) {
            aviutl2::logger::error(L"Failed to process video: unknown error");
            return false;
        }
    }

    static bool proc_audio(FilterProcAudio* audio) {
        try {
            if constexpr (ProcAudioPlugin<Derived>) {
                return Derived::instance().proc_audio_impl(audio);
            } else {
                return false;
            }
        } catch (const std::exception& e) {
            aviutl2::logger::error(L"Failed to process audio: {}", utils::to_wstring(e.what()));
            return false;
        } catch (...) {
            aviutl2::logger::error(L"Failed to process audio: unknown error");
            return false;
        }
    }

    const std::vector<void*>& items() const { return this->raw_items_; }

  private:
    std::vector<void*> raw_items_;
    std::vector<std::unique_ptr<LayoutItemBase>> owned_layout_items_;

    Derived* derived() { return static_cast<Derived*>(this); }
    const Derived* derived() const { return static_cast<const Derived*>(this); }

    template <FilterItem Item> void append_item(Item& item) { raw_items_.push_back(to_void_ptr(item.item())); }

    template <typename T> void append_item(std::reference_wrapper<T> item) { append_item(item.get()); }

    template <typename... Children> void append_item(GroupSpec<Children...> group_spec) {
        auto item = std::make_unique<Group>(group_spec.name, group_spec.default_visible);
        raw_items_.push_back(item->raw_item());
        owned_layout_items_.push_back(std::move(item));

        std::apply([this](auto&... child) { (append_item(child), ...); }, group_spec.children);

        // グループの終端を追加
        auto item_end = std::make_unique<Group>(L"", false);
        raw_items_.push_back(item_end->raw_item());
        owned_layout_items_.push_back(std::move(item_end));
    }

    void append_item(SeparatorSpec separator_spec) {
        auto item = std::make_unique<Separator>(separator_spec.name);
        raw_items_.push_back(item->raw_item());
        owned_layout_items_.push_back(std::move(item));
    }

    static void* to_void_ptr(const void* ptr) { return const_cast<void*>(ptr); }
};

}; // namespace aviutl2::filter

namespace aviutl2::utils::enum_utils {
template <> struct enable_bitmask<filter::FilterFlag> : std::true_type {};
}; // namespace aviutl2::utils::enum_utils

#define AVIUTL2_REGISTER_FILTER_PLUGIN(PluginType)                                                                     \
    AVIUTL2_API void InitializeLogger(LOG_HANDLE* logger) { aviutl2::logger::initialize(logger); }                     \
    AVIUTL2_API void InitializeConfig(CONFIG_HANDLE* config) { aviutl2::config::initialize(config); }                  \
    AVIUTL2_API void InitializeCache(CACHE_HANDLE* cache) { aviutl2::cache::initialize(cache); }                       \
    AVIUTL2_API FILTER_PLUGIN_TABLE* GetFilterPluginTable() {                                                          \
        return const_cast<FILTER_PLUGIN_TABLE*>(PluginType::instance().plugin_table());                                \
    }                                                                                                                  \
    AVIUTL2_API bool InitializePlugin(DWORD version) { return PluginType::instance().initialize(version); }            \
    AVIUTL2_API void UninitializePlugin() { PluginType::instance().uninitialize(); }
