#include <aviutl2_sdk_cpp/module.hpp>

#include <cassert>

// モジュールに登録する関数
void func(aviutl2::module::ScriptModuleParam* param) {}

class SkeletonScriptModule : public aviutl2::module::ScriptModule<SkeletonScriptModule> {
  public:
    explicit SkeletonScriptModule(token) {
        // スクリプトモジュールの情報を設定
        information_ = L"Skeleton Script Module with aviutl2-sdk-cpp";
        add_functions({
            {L"func", func},
        });
    }

    // プラグイン初期化処理 (任意)
    bool initialize_impl(DWORD version) { return true; }

    // プラグイン開放処理 (任意)
    void uninitialize_impl() {}
};

// プラグイン初期化/解放処理の検証
static_assert(aviutl2::InitializablePlugin<SkeletonScriptModule>, "プラグイン初期化処理のシグネチャが違います");
static_assert(aviutl2::UninitializablePlugin<SkeletonScriptModule>, "プラグイン開放処理のシグネチャが違います");

AVIUTL2_REGISTER_SCRIPT_MODULE(SkeletonScriptModule)
