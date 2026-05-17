#include <Windows.h>
#include <cassert>

#include <aviutl2_sdk_cpp/input.hpp>
#include <aviutl2_sdk_cpp/utils.hpp>

class SkeletonInputPlugin : public aviutl2::input::InputPlugin<SkeletonInputPlugin> {
  public:
    explicit SkeletonInputPlugin(token) {
        // 入力プラグインの情報を設定
        flag_ = aviutl2::input::InputFlag::VIDEO | aviutl2::input::InputFlag::AUDIO;
        name_ = L"Skeleton Input Plugin";
        filefilter_ = aviutl2::utils::make_file_filter({
            {L"AVI File (*.avi)", L"*.avi"},
            {L"WAV File (*.wav)", L"*.wav"},
        });
        information_ = L"Skeleton Input Plugin with aviutl2-sdk-cpp";
    }

    // プラグイン初期化処理 (任意)
    bool initialize_impl(DWORD version) { return true; }

    // プラグイン開放処理 (任意)
    void uninitialize_impl() {}

    // ファイルオープン (必須)
    aviutl2::input::InputHandle open_impl(LPCWSTR file) { return nullptr; }

    // ファイルクローズ (必須)
    bool close_impl(aviutl2::input::InputHandle handle) { return true; }

    // ファイルの情報取得 (必須)
    std::optional<aviutl2::input::InputInfo> get_input_info_impl(aviutl2::input::InputHandle handle) {
        return std::nullopt;
    }

    // 画像データ読み込み (任意)
    int read_video_impl(aviutl2::input::InputHandle handle, int frame, void* buf) { return 0; }

    // 音声データ読み込み (任意)
    int read_audio_impl(aviutl2::input::InputHandle handle, int start, int length, void* buf) { return 0; }

    // 設定ダイアログの表示 (任意)
    bool config_impl(HWND hwnd, HINSTANCE dll_hinst) { return false; }

    // 読み込み対象トラックの設定 (任意)
    int set_track_impl(aviutl2::input::InputHandle handle, aviutl2::input::TrackType type, int index) { return 0; }

    // フレーム番号の算出 (任意)
    int time_to_frame_impl(aviutl2::input::InputHandle handle, double time) { return 0; }
};

// プラグイン初期化/解放処理の検証
static_assert(aviutl2::InitializablePlugin<SkeletonInputPlugin>, "プラグイン初期化処理のシグネチャが違います");
static_assert(aviutl2::UninitializablePlugin<SkeletonInputPlugin>, "プラグイン開放処理のシグネチャが違います");

// 入力プラグイン任意処理の検証
static_assert(aviutl2::input::VideoReadablePlugin<SkeletonInputPlugin>, "read_video_implのシグネチャが違います");
static_assert(aviutl2::input::AudioReadablePlugin<SkeletonInputPlugin>, "read_audio_implのシグネチャが違います");
static_assert(aviutl2::input::ConfigurablePlugin<SkeletonInputPlugin>, "config_implのシグネチャが違います");
static_assert(aviutl2::input::TrackSelectablePlugin<SkeletonInputPlugin>, "set_track_implのシグネチャが違います");
static_assert(aviutl2::input::TimeToFrameInputPlugin<SkeletonInputPlugin>, "time_to_frame_implのシグネチャが違います");

AVIUTL2_REGISTER_INPUT_PLUGIN(SkeletonInputPlugin)
