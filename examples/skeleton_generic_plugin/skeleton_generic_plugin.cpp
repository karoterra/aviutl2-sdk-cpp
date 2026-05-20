#include <aviutl2_sdk_cpp/generic.hpp>

#include <cassert>

class SkeletonGenericPlugin : public aviutl2::generic::GenericPlugin<SkeletonGenericPlugin> {
  public:
    explicit SkeletonGenericPlugin(token) {
        // 汎用プラグインの情報を設定
        name_ = L"Skeleton Generic Plugin";
        information_ = L"Skeleton Generic Plugin with aviutl2-sdk-cpp";
    }

    // プラグイン初期化処理 (任意)
    bool initialize_impl(DWORD version) { return true; }

    // プラグイン開放処理 (任意)
    void uninitialize_impl() {}

    // プラグイン登録処理 (必須)
    void register_impl(aviutl2::generic::HostApp& host) {}

    // プロジェクトファイルをロードした直後に呼ばれる関数 (任意)
    void on_project_load_impl(aviutl2::edit::ProjectFile& project) {}

    // プロジェクトファイルをセーブする直前に呼ばれる関数 (任意)
    void on_project_save_impl(aviutl2::edit::ProjectFile& project) {}

    // キャッシュを破棄の操作時に呼ばれる関数 (任意)
    void on_clear_cache_impl(aviutl2::edit::EditSection& edit) {}

    // シーンを変更した直後に呼ばれる関数 (任意)
    void on_change_scene_impl(aviutl2::edit::EditSection& edit) {}
};

// プラグイン初期化/解放処理の検証
static_assert(aviutl2::InitializablePlugin<SkeletonGenericPlugin>, "プラグイン初期化処理のシグネチャが違います");
static_assert(aviutl2::UninitializablePlugin<SkeletonGenericPlugin>, "プラグイン開放処理のシグネチャが違います");

// 汎用プラグイン任意処理の検証
static_assert(aviutl2::generic::ProjectLoadPlugin<SkeletonGenericPlugin>, "on_project_load_implのシグネチャが違います");
static_assert(aviutl2::generic::ProjectSavePlugin<SkeletonGenericPlugin>, "on_project_save_implのシグネチャが違います");
static_assert(aviutl2::generic::ClearCachePlugin<SkeletonGenericPlugin>, "on_clear_cache_implのシグネチャが違います");
static_assert(aviutl2::generic::ChangeScenePlugin<SkeletonGenericPlugin>, "on_change_scene_implのシグネチャが違います");

AVIUTL2_REGISTER_GENERIC_PLUGIN(SkeletonGenericPlugin)
