//----------------------------------------------------------------------------------
// サンプルフィルタプラグイン(フィルタ効果) for AviUtl ExEdit2
//
// 公式SDKのサンプルコードを本ラッパー用に書き換えたものです
//----------------------------------------------------------------------------------

#include <Windows.h>

#include <algorithm>
#include <memory>

#include <aviutl2_sdk_cpp/filter.hpp>

using aviutl2::filter::FilterFlag;
using aviutl2::filter::FilterProcAudio;
using aviutl2::filter::FilterProcVideo;
using aviutl2::filter::group;
using aviutl2::filter::PixelRgba;
using aviutl2::filter::separator;
using aviutl2::utils::enum_utils::has_flag;

enum class Component : int {
    R = 1,
    G = 2,
    B = 4,
    All = R | G | B,
};
template <> struct aviutl2::utils::enum_utils::enable_bitmask<Component> : std::true_type {};

class MediaFilterPlugin : public aviutl2::filter::FilterPlugin<MediaFilterPlugin> {
  public:
    // フィルタ設定項目
    aviutl2::filter::Track<double> luminance{L"明るさ", 1.0, 0.0, 2.0, 0.01};
    aviutl2::filter::Select<Component> component{L"対象",
                                                 Component::All,
                                                 {
                                                     {L"R成分のみ", Component::R},
                                                     {L"G成分のみ", Component::G},
                                                     {L"B成分のみ", Component::B},
                                                     {L"RGB成分", Component::All},
                                                 }};
    aviutl2::filter::Track<double> volume{L"音量", 1.0, 0.0, 2.0, 0.01};
    aviutl2::filter::Check mono{L"モノラル化", false};
    aviutl2::filter::Check dummy1{L"ダミー1", false};

    explicit MediaFilterPlugin(token) {
        // フィルタプラグインの情報を設定
        flag_ = FilterFlag::VIDEO | FilterFlag::AUDIO;
        name_ = L"メディアフィルタ(sample)";
        label_ = L"サンプル";
        information_ = L"Sample MediaFilter version 2.00 By ＫＥＮくん";

        // フィルタ設定項目を登録
        // add_items(luminance, component, volume, mono);
        add_items(luminance,                       // 明るさ
                  group(L"グループ1", true,        // グループ1
                        component,                 //   対象
                        separator(L"セパレート2"), //   セパレート2
                        volume,                    //   音量
                        mono                       //   モノラル化
                        ),                         // グループ1の終端
                  dummy1                           // ダミー1
        );
    }

    // プラグイン初期化処理 (省略可能)
    bool initialize_impl(DWORD version) {
        aviutl2::logger::info(L"Initializing plugin");
        return true;
    }
    // プラグイン解放処理 (省略可能)
    void uninitialize_impl() { aviutl2::logger::info(L"Uninitializing plugin"); }

    // 画像フィルタ処理 (省略可能)
    bool proc_video_impl(FilterProcVideo* video) {
        aviutl2::logger::info(L"Processing video: frame={}, time={}", video->object->frame, video->object->time);

        auto w = video->object->width;
        auto h = video->object->height;
        auto buffer = std::make_unique<PixelRgba[]>(w * h);
        video->get_image_data(buffer.get());

        // 指定のRGB成分の明るさを調整
        auto r = has_flag(component.value(), Component::R) ? luminance.value() : 1.0;
        auto g = has_flag(component.value(), Component::G) ? luminance.value() : 1.0;
        auto b = has_flag(component.value(), Component::B) ? luminance.value() : 1.0;
        auto p = buffer.get();
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                p->r = (unsigned char)std::clamp(p->r * r, 0.0, 255.0);
                p->g = (unsigned char)std::clamp(p->g * g, 0.0, 255.0);
                p->b = (unsigned char)std::clamp(p->b * b, 0.0, 255.0);
                p++;
            }
        }

        video->set_image_data(buffer.get(), w, h);
        return true;
    }

    // 音声フィルタ処理 (省略可能)
    bool proc_audio_impl(FilterProcAudio* audio) {
        aviutl2::logger::info(L"Processing audio: sample_index={}, time={}", audio->object->sample_index,
                              audio->object->time);

        auto num = audio->object->sample_num;
        auto buffer0 = std::make_unique<float[]>(num);
        auto buffer1 = std::make_unique<float[]>(num);
        audio->get_sample_data(buffer0.get(), 0);
        audio->get_sample_data(buffer1.get(), 1);

        // 音量を調整
        auto v = (float)volume.value();
        auto p0 = buffer0.get();
        auto p1 = buffer1.get();
        for (int i = 0; i < num; i++) {
            *p0++ *= v;
            *p1++ *= v;
        }

        if (mono.value()) {
            // モノラル化
            p0 = buffer0.get();
            p1 = buffer1.get();
            for (int i = 0; i < num; i++) {
                float m = (*p0 + *p1) * 0.5f;
                *p0++ = m;
                *p1++ = m;
            }
        }

        audio->set_sample_data(buffer0.get(), 0);
        audio->set_sample_data(buffer1.get(), 1);
        return true;
    }
};

AVIUTL2_REGISTER_FILTER_PLUGIN(MediaFilterPlugin)
