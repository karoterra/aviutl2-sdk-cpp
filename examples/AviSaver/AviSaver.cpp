//----------------------------------------------------------------------------------
// サンプルAVI(vfw経由)出力プラグイン for AviUtl ExEdit2
//
// 公式SDKのサンプルコードを本ラッパー用に書き換えたものです
//----------------------------------------------------------------------------------
#include <aviutl2_sdk_cpp/output.hpp>

#include <vfw.h>
#pragma comment(lib, "vfw32.lib")

using aviutl2::output::OutputAudioFormat;
using aviutl2::output::OutputFlag;
using aviutl2::output::OutputInfo;
using aviutl2::output::OutputVideoFormat;

// AVIファイルハンドル構造体
struct AviHandle {
    PAVIFILE pfile = nullptr;
    PAVISTREAM pvideo = nullptr;
    PAVISTREAM paudio = nullptr;
    ~AviHandle() {
        if (paudio)
            AVIStreamRelease(paudio);
        if (pvideo)
            AVIStreamRelease(pvideo);
        if (pfile)
            AVIFileRelease(pfile);
    }
};

class AviSaverPlugin : public aviutl2::output::OutputPlugin<AviSaverPlugin> {
  public:
    explicit AviSaverPlugin(token) {
        flag_ = OutputFlag::VIDEO | OutputFlag::AUDIO;
        name_ = L"AVI File Saver2 (sample)";
        filefilter_ = aviutl2::utils::make_file_filter({{L"AviFile (*.avi)", L"*.avi"}});
        information_ = L"Sample AVI File Saver2 version 2.01 By ＫＥＮくん";
    }

    // 出力プラグイン出力関数
    bool output_impl(OutputInfo& info) {
        AviHandle avi;
        if (AVIFileOpen(&avi.pfile, info.savefile(), OF_WRITE | OF_CREATE, NULL) != S_OK) {
            return false;
        }

        // ビデオストリームの設定
        AVISTREAMINFO video{};
        video.fccType = streamtypeVIDEO;
        video.fccHandler = BI_RGB;
        video.dwRate = info.rate();
        video.dwScale = info.scale();
        video.rcFrame.right = info.width();
        video.rcFrame.bottom = info.height();
        if (AVIFileCreateStream(avi.pfile, &avi.pvideo, &video) != S_OK) {
            return false;
        }
        BITMAPINFOHEADER bmih{};
        bmih.biSize = sizeof(BITMAPINFOHEADER);
        bmih.biWidth = info.width();
        bmih.biHeight = info.height();
        bmih.biPlanes = 1;
        bmih.biBitCount = 24;
        bmih.biCompression = BI_RGB;
        bmih.biSizeImage = info.width() * info.height() * 3;
        if (AVIStreamSetFormat(avi.pvideo, 0, &bmih, sizeof(bmih)) != S_OK) {
            return false;
        }

        // オーディオストリームの設定
        AVISTREAMINFO audio{};
        audio.fccType = streamtypeAUDIO;
        audio.fccHandler = WAVE_FORMAT_PCM;
        audio.dwSampleSize = info.audio_ch() * 2;
        audio.dwRate = info.audio_rate() * audio.dwSampleSize;
        audio.dwScale = audio.dwSampleSize;
        if (AVIFileCreateStream(avi.pfile, &avi.paudio, &audio) != S_OK) {
            return false;
        }
        WAVEFORMATEX wf{};
        wf.wFormatTag = WAVE_FORMAT_PCM;
        wf.nChannels = info.audio_ch();
        wf.nSamplesPerSec = info.audio_rate();
        wf.wBitsPerSample = 16;
        wf.nBlockAlign = wf.nChannels * (wf.wBitsPerSample / 8);
        wf.nAvgBytesPerSec = wf.nSamplesPerSec * wf.nBlockAlign;
        if (AVIStreamSetFormat(avi.paudio, 0, &wf, sizeof(wf)) != S_OK) {
            return false;
        }

        info.set_buffer_size(5, 10); // データ取得バッファ数を変更

        for (int frame = 0; frame < info.n(); frame++) {
            info.display_rest_time(frame, info.n()); // 残り時間を表示
            if (info.is_abort())
                break; // 中断の確認

            // ビデオの書き込み
            void* data = info.get_video(frame, OutputVideoFormat::RGB24);
            if (AVIStreamWrite(avi.pvideo, frame, 1, data, bmih.biSizeImage, AVIIF_KEYFRAME, NULL, NULL) != S_OK) {
                break;
            }

            // オーディオの書き込み
            int audioPos = (int)((double)frame / video.dwRate * video.dwScale * info.audio_rate());
            int audioNum = (int)((double)(frame + 1) / video.dwRate * video.dwScale * info.audio_rate()) - audioPos;
            int audioReaded = 0;
            data = info.get_audio(audioPos, audioNum, &audioReaded, OutputAudioFormat::PCM16);
            if (audioReaded == 0)
                continue;
            if (AVIStreamWrite(avi.paudio, audioPos, audioReaded, data, audioReaded * wf.nBlockAlign, 0, NULL, NULL) !=
                S_OK) {
                break;
            }
        }

        return true;
    }

    // 設定ダイアログ
    bool config_impl(HWND hwnd, HINSTANCE dll_hinst) {
        MessageBox(hwnd, L"サンプルダイアログ", L"出力設定", MB_OK);

        // DLLを開放されても設定が残るように保存しておいてください

        return true;
    }

    // 出力設定のテキスト情報 (設定ボタンの隣に表示される)
    LPCWSTR get_config_text_impl() { return L"サンプル設定情報"; }
};

AVIUTL2_REGISTER_OUTPUT_PLUGIN(AviSaverPlugin)
