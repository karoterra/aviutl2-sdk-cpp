//----------------------------------------------------------------------------------
// サンプルフィルタプラグイン(メディアオブジェクト) for AviUtl ExEdit2
//
// 公式SDKのサンプルコードを本ラッパー用に書き換えたものです
//----------------------------------------------------------------------------------
#include <aviutl2_sdk_cpp/filter.hpp>

#include <d3d11.h>
#include <wrl/client.h>

#include <algorithm>
#include <memory>

using Microsoft::WRL::ComPtr;

using aviutl2::filter::Color;
using aviutl2::filter::FilterFlag;
using aviutl2::filter::FilterProcAudio;
using aviutl2::filter::FilterProcVideo;
using aviutl2::filter::Track;

class MediaObjectPlugin : public aviutl2::filter::FilterPlugin<MediaObjectPlugin> {
  public:
    // フィルタ設定項目
    Track<int> width{L"横", 100, 1, 1000};
    Track<int> height{L"縦", 100, 1, 1000};
    Color color{L"色", 0xffffff};
    Track<double> frequency{L"周波数", 1000, 1, 24000};

    explicit MediaObjectPlugin(token) {
        // フィルタプラグインの情報を設定
        flag_ = FilterFlag::VIDEO | FilterFlag::AUDIO | FilterFlag::INPUT;
        name_ = L"メディアオブジェクト(sample)";
        label_ = L"サンプル";
        information_ = L"Sample MediaObject version 2.00 By ＫＥＮくん";

        // フィルタ設定項目を登録
        add_items(width, height, color, frequency);
    }

    // プラグイン初期化処理は省略
    // プラグイン解放処理は省略

    // 画像フィルタ処理 (FLAG_VIDEOが有効の時のみ呼ばれます)
    bool proc_video_impl(FilterProcVideo* video) {
        int w = width.value();
        int h = height.value();
        if (w <= 0 || h <= 0)
            return false;

        // 指定サイズの画像を設定してTexture2Dを取得
        video->set_image_data(nullptr, w, h);
        auto texture = video->get_image_texture2d();

        // D3DのDevice,DeviceContextを取得
        ComPtr<ID3D11Device> device;
        texture->GetDevice(&device);
        ComPtr<ID3D11DeviceContext> context;
        device->GetImmediateContext(&context);

        // Texture2DのRTVを取得
        D3D11_TEXTURE2D_DESC desc{};
        texture->GetDesc(&desc);
        D3D11_RENDER_TARGET_VIEW_DESC rtvDesc{};
        rtvDesc.Format = desc.Format;
        rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        ComPtr<ID3D11RenderTargetView> rtv;
        if (FAILED(device->CreateRenderTargetView(texture, &rtvDesc, &rtv))) {
            return false;
        }

        // 指定の色で塗りつぶす
        const float color[4] = {this->color.r() / 255.0f, this->color.g() / 255.0f, this->color.b() / 255.0f,
                                1.0f}; // 乗算済みアルファ
        context->ClearRenderTargetView(rtv.Get(), color);
        return true;
    }

    // 音声フィルタ処理 (FLAG_AUDIOが有効の時のみ呼ばれます)
    bool proc_audio_impl(FilterProcAudio* audio) {
        auto sample_index = audio->object->sample_index;
        auto sample_num = audio->object->sample_num;
        auto channel_num = audio->object->channel_num;

        // 指定周波数のサイン波の音声データを作成
        auto step = (3.141592653589793 * 2.0) * frequency.value() / audio->scene->sample_rate;
        auto buffer = std::make_unique<float[]>(sample_num);
        auto p = buffer.get();
        for (int i = 0; i < sample_num; i++) {
            *p++ = (float)sin(sample_index++ * step);
        }

        for (int i = 0; i < channel_num; i++) {
            audio->set_sample_data(buffer.get(), i);
        }
        return true;
    }
};

AVIUTL2_REGISTER_FILTER_PLUGIN(MediaObjectPlugin)
