#include <aviutl2_sdk_cpp/output.hpp>

#include <cassert>

class SkeletonOutputPlugin : public aviutl2::output::OutputPlugin<SkeletonOutputPlugin> {
  public:
    explicit SkeletonOutputPlugin(token) {
        // 出力プラグインの情報を設定
        flag_ = aviutl2::output::OutputFlag::VIDEO | aviutl2::output::OutputFlag::AUDIO;
        name_ = L"Skeleton Output Plugin";
        filefilter_ = aviutl2::utils::make_file_filter({
            {L"AVI File (*.avi)", L"*.avi"},
            {L"WAV File (*.wav)", L"*.wav"},
        });
        information_ = L"Skeleton Output Plugin with aviutl2-sdk-cpp";
    }

    // プラグイン初期化処理 (任意)
    bool initialize_impl(DWORD version) { return true; }

    // プラグイン開放処理 (任意)
    void uninitialize_impl() {}

    // 出力処理 (必須)
    bool output_impl(aviutl2::output::OutputInfo& info) { return false; }

    // 設定ダイアログの表示 (任意)
    bool config_impl(HWND hwnd, HINSTANCE dll_hinst) { return false; }

    // 出力設定のテキスト情報を取得 (任意)
    LPCWSTR get_config_text_impl() { return nullptr; }

    // プロジェクトファイル側から出力設定の読み込み (任意)
    bool load_project_config_impl(aviutl2::edit::ProjectFile& project) { return false; }

    // プロジェクトファイル側への出力設定の書き込み (任意)
    bool save_project_config_impl(aviutl2::edit::ProjectFile& project) { return false; }
};

// プラグイン初期化/解放処理の検証
static_assert(aviutl2::InitializablePlugin<SkeletonOutputPlugin>, "プラグイン初期化処理のシグネチャが違います");
static_assert(aviutl2::UninitializablePlugin<SkeletonOutputPlugin>, "プラグイン開放処理のシグネチャが違います");

// 出力プラグイン任意処理の検証
static_assert(aviutl2::output::ConfigurablePlugin<SkeletonOutputPlugin>, "config_implのシグネチャが違います");
static_assert(aviutl2::output::ConfigTextGettable<SkeletonOutputPlugin>, "get_config_text_implのシグネチャが違います");
static_assert(aviutl2::output::ProjectConfigLoadable<SkeletonOutputPlugin>,
              "load_project_config_implのシグネチャが違います");
static_assert(aviutl2::output::ProjectConfigSavable<SkeletonOutputPlugin>,
              "save_project_config_implのシグネチャが違います");

AVIUTL2_REGISTER_OUTPUT_PLUGIN(SkeletonOutputPlugin)
